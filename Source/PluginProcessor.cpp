#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace CR {

CallResponseAudioProcessor::CallResponseAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    // Generate initial phrase pair
    GeneratorEngine::generatePair(config, callPhrase, responsePhrase, 42);
    applyCurrentPerformance();
}

CallResponseAudioProcessor::~CallResponseAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout CallResponseAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("density", 1), "Density",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("noteLength", 1), "Gate Length",
        juce::NormalisableRange<float>(0.25f, 2.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("octaveJump", 1), "Octave Jump",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("resolve", 1), "Resolve",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("cutoff", 1), "Filter Cutoff",
        juce::NormalisableRange<float>(60.0f, 14000.0f, 1.0f, 0.35f), 1200.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("resonance", 1), "Resonance",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.72f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("decay", 1), "Decay Time",
        juce::NormalisableRange<float>(0.05f, 1.5f, 0.01f, 0.5f), 0.25f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("drive", 1), "Drive",
        juce::NormalisableRange<float>(1.0f, 4.0f, 0.01f), 1.8f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("syncMode", 1), "Sync Mode",
        juce::StringArray{"Timeline", "Restart", "Free"}, 0));

    return { params.begin(), params.end() };
}

const juce::String CallResponseAudioProcessor::getName() const { return "Call & Response MIDI"; }
bool CallResponseAudioProcessor::acceptsMidi() const { return true; }
bool CallResponseAudioProcessor::producesMidi() const { return true; }
bool CallResponseAudioProcessor::isMidiEffect() const { return false; }
double CallResponseAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int CallResponseAudioProcessor::getNumPrograms() { return 1; }
int CallResponseAudioProcessor::getCurrentProgram() { return 0; }
void CallResponseAudioProcessor::setCurrentProgram (int) {}
const juce::String CallResponseAudioProcessor::getProgramName (int) { return {}; }
void CallResponseAudioProcessor::changeProgramName (int, const juce::String&) {}

bool CallResponseAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void CallResponseAudioProcessor::prepareToPlay (double newSampleRate, int samplesPerBlock) {
    juce::ignoreUnused(samplesPerBlock);
    sampleRate = newSampleRate;
    synth.prepare(sampleRate);
    activeNotes.clear();
    lastStepIndex = -1;
    lastPhaseIndex = -1;
    freeRunningPhase = 0.0;
}

void CallResponseAudioProcessor::releaseResources() {
    activeNotes.clear();
}

void CallResponseAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;
    auto numSamples = buffer.getNumSamples();

    buffer.clear();
    midiMessages.clear();

    // Update synth parameters from APVTS
    float cutoff = apvts.getRawParameterValue("cutoff")->load();
    float res = apvts.getRawParameterValue("resonance")->load();
    float decay = apvts.getRawParameterValue("decay")->load();
    float drive = apvts.getRawParameterValue("drive")->load();
    synth.setParameters(cutoff, res, decay, drive);
    synth.setMuted(muteAudio);

    // Update performance controls
    perf.density = apvts.getRawParameterValue("density")->load();
    perf.noteLength = apvts.getRawParameterValue("noteLength")->load();
    perf.octaveJump = apvts.getRawParameterValue("octaveJump")->load();
    perf.resolve = apvts.getRawParameterValue("resolve")->load();

    int syncMode = (int)apvts.getRawParameterValue("syncMode")->load();

    // Read Transport info
    bool hostPlaying = false;
    double bpm = 140.0;
    double ppqPosition = 0.0;

    if (auto* playHead = getPlayHead()) {
        if (auto posOpt = playHead->getPosition()) {
            if (posOpt->getIsPlaying()) hostPlaying = true;
            if (auto b = posOpt->getBpm()) bpm = *b;
            if (auto ppq = posOpt->getPpqPosition()) ppqPosition = *ppq;
        }
    }

    currentBpm.store(bpm);
    isHostPlaying.store(hostPlaying);

    std::vector<Step> currentCall, currentResp;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        currentCall = renderedCall;
        currentResp = renderedResponse;
    }

    int callSteps = (int)currentCall.size();
    int respSteps = (int)currentResp.size();
    int totalSteps = callSteps + respSteps;
    if (totalSteps <= 0) return;

    // Calculate current step in 16th notes (0.25 beats per step)
    const double stepPpq = 0.25;
    int stepInCycle = 0;

    if (syncMode == 0) { // Timeline mode (absolute DAW timeline sync)
        if (hostPlaying) {
            double cyclePpq = (double)totalSteps * stepPpq;
            double modPpq = std::fmod(ppqPosition, cyclePpq);
            if (modPpq < 0.0) modPpq += cyclePpq;
            stepInCycle = (int)(modPpq / stepPpq) % totalSteps;
        } else {
            stepInCycle = -1;
        }
    } else if (syncMode == 1) { // Restart mode (restarts when DAW starts)
        if (hostPlaying) {
            double cyclePpq = (double)totalSteps * stepPpq;
            double modPpq = std::fmod(ppqPosition, cyclePpq);
            if (modPpq < 0.0) modPpq += cyclePpq;
            stepInCycle = (int)(modPpq / stepPpq) % totalSteps;
        } else {
            stepInCycle = -1;
        }
    } else { // Free running mode
        double stepSamples = (60.0 / bpm / 4.0) * sampleRate;
        freeRunningPhase += (double)numSamples;
        double cycleSamples = stepSamples * (double)totalSteps;
        freeRunningPhase = std::fmod(freeRunningPhase, cycleSamples);
        stepInCycle = (int)(freeRunningPhase / stepSamples) % totalSteps;
        hostPlaying = true;
    }

    // Handle panic request
    if (panicRequested.exchange(false)) {
        for (int ch = 1; ch <= 16; ++ch) {
            midiMessages.addEvent(juce::MidiMessage::allNotesOff(ch), 0);
            midiMessages.addEvent(juce::MidiMessage::allSoundOff(ch), 0);
        }
        activeNotes.clear();
        synth.reset();
    }

    // Step change handling
    if (hostPlaying && stepInCycle >= 0 && stepInCycle != lastStepIndex) {
        // Handle quantized regeneration trigger at cycle boundary (step 0)
        if (stepInCycle == 0 && isRegenQueued.load()) {
            isRegenQueued.store(false);
            generateNewPair();
            {
                std::lock_guard<std::recursive_mutex> lock(phraseMutex);
                currentCall = renderedCall;
                currentResp = renderedResponse;
            }
        }

        int phase = (stepInCycle < callSteps) ? 0 : 1;
        int localStepIdx = (phase == 0) ? stepInCycle : (stepInCycle - callSteps);

        currentPlayingStep.store(localStepIdx);
        currentPlayingPhase.store(phase);

        const auto& phrase = (phase == 0) ? currentCall : currentResp;
        bool isMuted = (phase == 0) ? muteCall : muteResponse;
        int channel = (phase == 0) ? config.callChannel : config.responseChannel;

        if (localStepIdx >= 0 && localStepIdx < (int)phrase.size()) {
            const auto& s = phrase[(size_t)localStepIdx];
            if (s.active) {
                int noteNum = s.getEffectiveMidiNote();
                float vel = s.accent ? 1.0f : 0.75f;

                // Send MIDI Note On to DAW / track
                if (!isMuted) {
                    midiMessages.addEvent(juce::MidiMessage::noteOn(channel, noteNum, vel), 0);
                    if (phase == 0) callMidiFlashed.store(true);
                    else            respMidiFlashed.store(true);
                }

                // Trigger Preview Synth
                synth.noteOn(noteNum, vel, s.accent, s.slide);

                // Calculate NoteOff sample position
                double stepDurationSec = (60.0 / bpm / 4.0);
                int noteDurationSamples = (int)(stepDurationSec * (double)s.gate * sampleRate);
                if (s.slide) noteDurationSamples += (int)(stepDurationSec * 0.25 * sampleRate);
                noteDurationSamples = std::max((int)(sampleRate * 0.02), noteDurationSamples);

                ActiveNote an;
                an.channel = channel;
                an.noteNumber = noteNum;
                an.sampleOff = noteDurationSamples;
                activeNotes.push_back(an);
            }
        }
        lastStepIndex = stepInCycle;
    } else if (!hostPlaying) {
        currentPlayingStep.store(-1);
        lastStepIndex = -1;

        // Clean up any remaining notes when host transport stops
        if (!activeNotes.empty()) {
            for (const auto& an : activeNotes) {
                midiMessages.addEvent(juce::MidiMessage::noteOff(an.channel, an.noteNumber), 0);
            }
            activeNotes.clear();
            midiMessages.addEvent(juce::MidiMessage::allNotesOff(config.callChannel), 0);
            midiMessages.addEvent(juce::MidiMessage::allNotesOff(config.responseChannel), 0);
            synth.reset();
        }
    }

    // Process NoteOffs in this block
    for (auto it = activeNotes.begin(); it != activeNotes.end(); ) {
        it->sampleOff -= numSamples;
        if (it->sampleOff <= 0) {
            int offSample = std::max(0, numSamples + it->sampleOff);
            offSample = std::min(numSamples - 1, offSample);

            midiMessages.addEvent(juce::MidiMessage::noteOff(it->channel, it->noteNumber), offSample);
            synth.noteOff(it->noteNumber);

            it = activeNotes.erase(it);
        } else {
            ++it;
        }
    }

    // Render Preview Synth audio
    synth.renderNextBlock(buffer, 0, numSamples);
}

void CallResponseAudioProcessor::applyCurrentPerformanceInternal() {
    renderedCall = callPhrase;
    renderedResponse = responsePhrase;
    GeneratorEngine::applyPerformance(renderedCall, config, perf, 100);
    GeneratorEngine::applyPerformance(renderedResponse, config, perf, 200);
}

void CallResponseAudioProcessor::applyCurrentPerformance() {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    applyCurrentPerformanceInternal();
}

void CallResponseAudioProcessor::generateNewPair() {
    pushUndo();
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    GeneratorEngine::generatePair(config, callPhrase, responsePhrase, 0);
    applyCurrentPerformanceInternal();
}

void CallResponseAudioProcessor::rerollUnlocked() {
    pushUndo();
    auto& rng = juce::Random::getSystemRandom();

    if (!lockStyle) {
        config.styleIndex = rng.nextInt((int)GeneratorEngine::getStyles().size());
    }
    if (!lockKeyScale) {
        config.scaleIndex = rng.nextInt((int)GeneratorEngine::getScales().size());
    }
    if (!lockMotif) {
        config.motifTypeIndex = rng.nextInt((int)GeneratorEngine::getMotifTypes().size());
    }
    if (!lockStrategy) {
        config.responseStrategyIndex = rng.nextInt((int)GeneratorEngine::getResponseStrategies().size());
    }

    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    std::mt19937 randGen((uint32_t)rng.nextInt());

    if (!lockRhythm && !lockPitches) {
        GeneratorEngine::generatePair(config, callPhrase, responsePhrase, (uint32_t)rng.nextInt());
    } else if (lockRhythm && !lockPitches) {
        // Change pitches only
        for (auto& s : callPhrase) if (s.active) s.degree = rng.nextInt(10);
        callPhrase = GeneratorEngine::buildCall(config, randGen);
        responsePhrase = GeneratorEngine::buildResponse(callPhrase, config, randGen);
    } else {
        callPhrase = GeneratorEngine::buildCall(config, randGen);
        responsePhrase = GeneratorEngine::buildResponse(callPhrase, config, randGen);
    }

    applyCurrentPerformanceInternal();
}

void CallResponseAudioProcessor::mutateSelected(const juce::String& target) {
    pushUndo();
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    std::mt19937 rng((uint32_t)juce::Random::getSystemRandom().nextInt());

    if (target == "Call only") {
        GeneratorEngine::mutatePhrase(callPhrase, config, target, config.mutationAmountIndex, rng);
    } else if (target == "Response only") {
        GeneratorEngine::mutatePhrase(responsePhrase, config, target, config.mutationAmountIndex, rng);
    } else if (target == "Call + response linked") {
        GeneratorEngine::mutatePhrase(callPhrase, config, "Call only", config.mutationAmountIndex, rng);
        responsePhrase = GeneratorEngine::buildResponse(callPhrase, config, rng);
    } else if (target == "Ending only") {
        if (!responsePhrase.empty())
            GeneratorEngine::mutatePhrase(responsePhrase, config, target, config.mutationAmountIndex, rng);
        else
            GeneratorEngine::mutatePhrase(callPhrase, config, target, config.mutationAmountIndex, rng);
    } else if (target == "Motif only") {
        GeneratorEngine::mutatePhrase(callPhrase, config, target, config.mutationAmountIndex, rng);
        if (!responsePhrase.empty())
            GeneratorEngine::mutatePhrase(responsePhrase, config, target, config.mutationAmountIndex, rng);
    } else {
        // "Rhythm only", "Pitch only", "Articulation only", "Whole phrase"
        GeneratorEngine::mutatePhrase(callPhrase, config, target, config.mutationAmountIndex, rng);
        GeneratorEngine::mutatePhrase(responsePhrase, config, target, config.mutationAmountIndex, rng);
    }

    applyCurrentPerformanceInternal();
}

GeneratorConfig CallResponseAudioProcessor::getConfig() const {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    return config;
}

void CallResponseAudioProcessor::setConfig(const GeneratorConfig& newConfig) {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    bool scaleChanged = (newConfig.scaleIndex != config.scaleIndex);
    bool keyChanged   = (newConfig.keyIndex != config.keyIndex);
    bool octChanged   = (newConfig.rootOctaveIndex != config.rootOctaveIndex);

    if (scaleChanged || keyChanged || octChanged) {
        pushUndo();
        GeneratorEngine::transposePhraseToScale(callPhrase, config, newConfig);
        GeneratorEngine::transposePhraseToScale(responsePhrase, config, newConfig);
    }

    config = newConfig;
    applyCurrentPerformanceInternal();
}

std::vector<Step> CallResponseAudioProcessor::getCallPhrase() const {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    return callPhrase;
}

std::vector<Step> CallResponseAudioProcessor::getResponsePhrase() const {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    return responsePhrase;
}

void CallResponseAudioProcessor::setCallPhrase(const std::vector<Step>& phrase) {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    callPhrase = phrase;
    applyCurrentPerformanceInternal();
}

void CallResponseAudioProcessor::setResponsePhrase(const std::vector<Step>& phrase) {
    std::lock_guard<std::recursive_mutex> lock(phraseMutex);
    responsePhrase = phrase;
    applyCurrentPerformanceInternal();
}

void CallResponseAudioProcessor::pushUndo() {
    StateSnapshot snap;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        snap.config = config;
        snap.perf = perf;
        snap.call = callPhrase;
        snap.response = responsePhrase;
        snap.lockStyle = lockStyle;
        snap.lockKeyScale = lockKeyScale;
        snap.lockMotif = lockMotif;
        snap.lockStrategy = lockStrategy;
        snap.lockRhythm = lockRhythm;
        snap.lockPitches = lockPitches;
        snap.muteAudio = muteAudio;
    }
    presetManager.pushUndo(snap);
}

void CallResponseAudioProcessor::undo() {
    if (!presetManager.canUndo()) return;
    StateSnapshot current;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        current.config = config;
        current.perf = perf;
        current.call = callPhrase;
        current.response = responsePhrase;
    }
    auto prev = presetManager.undo(current);
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        config = prev.config;
        perf = prev.perf;
        callPhrase = prev.call;
        responsePhrase = prev.response;
        lockStyle = prev.lockStyle;
        lockKeyScale = prev.lockKeyScale;
        lockMotif = prev.lockMotif;
        lockStrategy = prev.lockStrategy;
        lockRhythm = prev.lockRhythm;
        lockPitches = prev.lockPitches;
        muteAudio = prev.muteAudio;
    }
    applyCurrentPerformance();
}

void CallResponseAudioProcessor::redo() {
    if (!presetManager.canRedo()) return;
    StateSnapshot current;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        current.config = config;
        current.perf = perf;
        current.call = callPhrase;
        current.response = responsePhrase;
    }
    auto next = presetManager.redo(current);
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        config = next.config;
        perf = next.perf;
        callPhrase = next.call;
        responsePhrase = next.response;
        lockStyle = next.lockStyle;
        lockKeyScale = next.lockKeyScale;
        lockMotif = next.lockMotif;
        lockStrategy = next.lockStrategy;
        lockRhythm = next.lockRhythm;
        lockPitches = next.lockPitches;
        muteAudio = next.muteAudio;
    }
    applyCurrentPerformance();
}

void CallResponseAudioProcessor::savePresetSlot(int slotIndex) {
    StateSnapshot snap;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        snap.config = config;
        snap.perf = perf;
        snap.call = callPhrase;
        snap.response = responsePhrase;
        snap.lockStyle = lockStyle;
        snap.lockKeyScale = lockKeyScale;
        snap.lockMotif = lockMotif;
        snap.lockStrategy = lockStrategy;
        snap.lockRhythm = lockRhythm;
        snap.lockPitches = lockPitches;
        snap.muteAudio = muteAudio;
    }
    presetManager.saveToSlot(slotIndex, snap);
}

void CallResponseAudioProcessor::recallPresetSlot(int slotIndex) {
    StateSnapshot snap;
    if (presetManager.loadFromSlot(slotIndex, snap)) {
        pushUndo();
        {
            std::lock_guard<std::recursive_mutex> lock(phraseMutex);
            config = snap.config;
            perf = snap.perf;
            callPhrase = snap.call;
            responsePhrase = snap.response;
            lockStyle = snap.lockStyle;
            lockKeyScale = snap.lockKeyScale;
            lockMotif = snap.lockMotif;
            lockStrategy = snap.lockStrategy;
            lockRhythm = snap.lockRhythm;
            lockPitches = snap.lockPitches;
            muteAudio = snap.muteAudio;
        }
        applyCurrentPerformance();
    }
}

void CallResponseAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    StateSnapshot snap;
    {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        snap.config = config;
        snap.perf = perf;
        snap.call = callPhrase;
        snap.response = responsePhrase;
        snap.lockStyle = lockStyle;
        snap.lockKeyScale = lockKeyScale;
        snap.lockMotif = lockMotif;
        snap.lockStrategy = lockStrategy;
        snap.lockRhythm = lockRhythm;
        snap.lockPitches = lockPitches;
        snap.muteAudio = muteAudio;
    }

    state.addChild(PresetManager::snapshotToValueTree(snap, "CurrentSnapshot"), -1, nullptr);
    state.addChild(presetManager.toValueTree(), -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CallResponseAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType())) {
        auto vt = juce::ValueTree::fromXml (*xmlState);
        apvts.replaceState (vt);

        auto snapVt = vt.getChildWithName("CurrentSnapshot");
        if (snapVt.isValid()) {
            auto snap = PresetManager::valueTreeToSnapshot(snapVt);
            std::lock_guard<std::recursive_mutex> lock(phraseMutex);
            config = snap.config;
            perf = snap.perf;
            callPhrase = snap.call;
            responsePhrase = snap.response;
            lockStyle = snap.lockStyle;
            lockKeyScale = snap.lockKeyScale;
            lockMotif = snap.lockMotif;
            lockStrategy = snap.lockStrategy;
            lockRhythm = snap.lockRhythm;
            lockPitches = snap.lockPitches;
            muteAudio = snap.muteAudio;
        }

        auto pmVt = vt.getChildWithName("PresetManager");
        if (pmVt.isValid()) {
            presetManager.fromValueTree(pmVt);
        }

        applyCurrentPerformance();
    }
}

juce::AudioProcessorEditor* CallResponseAudioProcessor::createEditor() {
    return new CallResponseAudioProcessorEditor (*this);
}

bool CallResponseAudioProcessor::hasEditor() const { return true; }

} // namespace CR

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new CR::CallResponseAudioProcessor();
}
