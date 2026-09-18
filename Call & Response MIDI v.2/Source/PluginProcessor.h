#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "GeneratorEngine.h"
#include "SynthEngine.h"
#include "PresetManager.h"
#include <atomic>
#include <mutex>

namespace CR {

class CallResponseAudioProcessor : public juce::AudioProcessor {
public:
    CallResponseAudioProcessor();
    ~CallResponseAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Generation and State
    void generateNewPair();
    void rerollUnlocked();
    void mutateSelected(const juce::String& target, int percentage = -1);
    void applyCurrentPerformance();

    // Phrases and config access
    GeneratorConfig getConfig() const;
    void setConfig(const GeneratorConfig& newConfig);

    std::vector<Step> getCallPhrase() const;
    std::vector<Step> getResponsePhrase() const;
    void setCallPhrase(const std::vector<Step>& phrase);
    void setResponsePhrase(const std::vector<Step>& phrase);

    // Locks
    bool lockStyle = false;
    bool lockKeyScale = false;
    bool lockMotif = false;
    bool lockStrategy = false;
    bool lockMode = false;
    bool lockEnding = false;
    bool lockRhythm = false;
    bool lockPitches = false;
    bool lockCall = false;
    bool lockResponse = false;
    void setCallLocked(bool lock) { lockCall = lock; }
    void setResponseLocked(bool lock) { lockResponse = lock; }
    bool isCallLocked() const { return lockCall; }
    bool isResponseLocked() const { return lockResponse; }

    // Mutes
    bool muteCall = false;
    bool muteResponse = false;
    bool muteAudio = false;
    void setCallMuted(bool mute);
    void setResponseMuted(bool mute);
    std::atomic<bool> muteCallRequested { false };
    std::atomic<bool> muteRespRequested { false };

    // Playback Rate & Swing
    std::atomic<float> swingPercent { 0.0f }; // 0.0f to 1.0f (0% to 100%)
    std::atomic<int> playbackRateIndex { 1 };  // 0=Half speed, 1=0 (1x), 2=Double speed
    void setSwingPercent (float pct) { swingPercent.store (juce::jlimit (0.0f, 1.0f, pct)); }
    float getSwingPercent() const { return swingPercent.load(); }
    void setPlaybackRateIndex (int idx) { playbackRateIndex.store (juce::jlimit (0, 2, idx)); }
    int getPlaybackRateIndex() const { return playbackRateIndex.load(); }

    // Quantized Regen (Now vs Next Phrase)
    bool nextPhraseRegen = false;
    std::atomic<bool> isRegenQueued { false };

    // Playback info
    int getCurrentPlayingStep() const { return currentPlayingStep.load(); }
    int getCurrentPlayingPhase() const { return currentPlayingPhase.load(); } // 0 = call, 1 = response
    bool getIsPlaying() const { return isHostPlaying.load(); }
    double getCurrentBpm() const { return currentBpm.load(); }

    // MIDI routing and activity
    std::atomic<bool> callMidiFlashed { false };
    std::atomic<bool> respMidiFlashed { false };
    std::atomic<bool> panicRequested { false };
    void triggerPanic() { panicRequested.store(true); }

    void setCallChannel(int ch) {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        config.callChannel = juce::jlimit(1, 16, ch);
    }
    void setResponseChannel(int ch) {
        std::lock_guard<std::recursive_mutex> lock(phraseMutex);
        config.responseChannel = juce::jlimit(1, 16, ch);
    }

    void triggerAuditionNote(int midiNote, bool isNoteOn) {
        if (isNoteOn) {
            synth.noteOn(midiNote, 0.8f, false, false);
        } else {
            synth.noteOff(midiNote);
        }
    }

    // Preset & Undo
    PresetManager& getPresetManager() { return presetManager; }
    void savePresetSlot(int slotIndex);
    void recallPresetSlot(int slotIndex);
    void undo();
    void redo();
    void pushUndo();

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    GeneratorConfig config;
    PerformanceParams perf;

    std::vector<Step> callPhrase;
    std::vector<Step> responsePhrase;
    std::vector<Step> renderedCall;
    std::vector<Step> renderedResponse;

    void applyCurrentPerformanceInternal();
    mutable std::recursive_mutex phraseMutex;

    SynthEngine synth;
    PresetManager presetManager;

    // Sequencer engine tracking
    std::atomic<int> currentPlayingStep { -1 };
    std::atomic<int> currentPlayingPhase { 0 };
    std::atomic<bool> isHostPlaying { false };
    std::atomic<double> currentBpm { 140.0 };

    double sampleRate = 44100.0;
    int lastStepIndex = -1;
    int lastPhaseIndex = -1;
    double freeRunningPhase = 0.0; // for free mode

    // Active MIDI note tracking for noteOff generation
    struct ActiveNote {
        int channel;
        int noteNumber;
        int sampleOff;
    };
    std::vector<ActiveNote> activeNotes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CallResponseAudioProcessor)
};

} // namespace CR
