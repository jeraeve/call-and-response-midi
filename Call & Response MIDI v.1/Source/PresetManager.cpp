#include "PresetManager.h"

namespace CR {

PresetManager::PresetManager() {
    slots.resize(8);
    slotFilled.assign(8, false);
}

void PresetManager::saveToSlot(int slotIndex, const StateSnapshot& snapshot) {
    if (slotIndex >= 0 && slotIndex < 8) {
        slots[(size_t)slotIndex] = snapshot;
        slotFilled[(size_t)slotIndex] = true;
    }
}

bool PresetManager::loadFromSlot(int slotIndex, StateSnapshot& outSnapshot) const {
    if (slotIndex >= 0 && slotIndex < 8 && slotFilled[(size_t)slotIndex]) {
        outSnapshot = slots[(size_t)slotIndex];
        return true;
    }
    return false;
}

bool PresetManager::isSlotOccupied(int slotIndex) const {
    if (slotIndex >= 0 && slotIndex < 8) {
        return slotFilled[(size_t)slotIndex];
    }
    return false;
}

void PresetManager::pushUndo(const StateSnapshot& snapshot) {
    undoStack.push_back(snapshot);
    if (undoStack.size() > maxHistory)
        undoStack.pop_front();
    redoStack.clear();
}

bool PresetManager::canUndo() const {
    return !undoStack.empty();
}

bool PresetManager::canRedo() const {
    return !redoStack.empty();
}

StateSnapshot PresetManager::undo(const StateSnapshot& current) {
    if (undoStack.empty()) return current;
    redoStack.push_back(current);
    auto prev = undoStack.back();
    undoStack.pop_back();
    return prev;
}

StateSnapshot PresetManager::redo(const StateSnapshot& current) {
    if (redoStack.empty()) return current;
    undoStack.push_back(current);
    auto next = redoStack.back();
    redoStack.pop_back();
    return next;
}

static juce::ValueTree phraseToValueTree(const std::vector<Step>& phrase, const juce::String& tag) {
    juce::ValueTree vt(tag);
    for (size_t i = 0; i < phrase.size(); ++i) {
        const auto& s = phrase[i];
        juce::ValueTree stepVt("Step");
        stepVt.setProperty("idx", (int)i, nullptr);
        stepVt.setProperty("active", s.active, nullptr);
        stepVt.setProperty("degree", s.degree, nullptr);
        stepVt.setProperty("midiNote", s.midiNote, nullptr);
        stepVt.setProperty("gate", (double)s.gate, nullptr);
        stepVt.setProperty("accent", s.accent, nullptr);
        stepVt.setProperty("slide", s.slide, nullptr);
        stepVt.setProperty("tie", s.tie, nullptr);
        stepVt.setProperty("octaveOffset", s.octaveOffset, nullptr);
        vt.addChild(stepVt, -1, nullptr);
    }
    return vt;
}

static std::vector<Step> valueTreeToPhrase(const juce::ValueTree& vt) {
    std::vector<Step> phrase;
    for (int i = 0; i < vt.getNumChildren(); ++i) {
        auto child = vt.getChild(i);
        Step s;
        s.active = child.getProperty("active", false);
        s.degree = child.getProperty("degree", 0);
        s.midiNote = child.getProperty("midiNote", 60);
        s.gate = (float)(double)child.getProperty("gate", 0.82);
        s.accent = child.getProperty("accent", false);
        s.slide = child.getProperty("slide", false);
        s.tie = child.getProperty("tie", false);
        s.octaveOffset = child.getProperty("octaveOffset", 0);
        phrase.push_back(s);
    }
    return phrase;
}

juce::ValueTree PresetManager::snapshotToValueTree(const StateSnapshot& snap, const juce::String& name) {
    juce::ValueTree vt(name);

    // Config
    juce::ValueTree cfgVt("Config");
    cfgVt.setProperty("keyIndex", snap.config.keyIndex, nullptr);
    cfgVt.setProperty("scaleIndex", snap.config.scaleIndex, nullptr);
    cfgVt.setProperty("rootOctaveIndex", snap.config.rootOctaveIndex, nullptr);
    cfgVt.setProperty("steps", snap.config.steps, nullptr);
    cfgVt.setProperty("styleIndex", snap.config.styleIndex, nullptr);
    cfgVt.setProperty("responseStrategyIndex", snap.config.responseStrategyIndex, nullptr);
    cfgVt.setProperty("motifTypeIndex", snap.config.motifTypeIndex, nullptr);
    cfgVt.setProperty("phraseModeIndex", snap.config.phraseModeIndex, nullptr);
    cfgVt.setProperty("endingBehaviourIndex", snap.config.endingBehaviourIndex, nullptr);
    cfgVt.setProperty("registerProfileIndex", snap.config.registerProfileIndex, nullptr);
    cfgVt.setProperty("densityPresetIndex", snap.config.densityPresetIndex, nullptr);
    cfgVt.setProperty("contourPresetIndex", snap.config.contourPresetIndex, nullptr);
    cfgVt.setProperty("repetitionPresetIndex", snap.config.repetitionPresetIndex, nullptr);
    cfgVt.setProperty("mutationAmountIndex", snap.config.mutationAmountIndex, nullptr);
    cfgVt.setProperty("mutationTargetIndex", snap.config.mutationTargetIndex, nullptr);
    cfgVt.setProperty("notePoolModeIndex", snap.config.notePoolModeIndex, nullptr);
    cfgVt.setProperty("articulationProfileIndex", snap.config.articulationProfileIndex, nullptr);
    cfgVt.setProperty("gateFeelIndex", snap.config.gateFeelIndex, nullptr);
    cfgVt.setProperty("connectionFeelIndex", snap.config.connectionFeelIndex, nullptr);
    cfgVt.setProperty("slideProfileIndex", snap.config.slideProfileIndex, nullptr);
    cfgVt.setProperty("accentProfileIndex", snap.config.accentProfileIndex, nullptr);
    cfgVt.setProperty("callChannel", snap.config.callChannel, nullptr);
    cfgVt.setProperty("responseChannel", snap.config.responseChannel, nullptr);
    vt.addChild(cfgVt, -1, nullptr);

    // Performance
    juce::ValueTree perfVt("Performance");
    perfVt.setProperty("density", (double)snap.perf.density, nullptr);
    perfVt.setProperty("noteLength", (double)snap.perf.noteLength, nullptr);
    perfVt.setProperty("octaveJump", (double)snap.perf.octaveJump, nullptr);
    perfVt.setProperty("resolve", (double)snap.perf.resolve, nullptr);
    vt.addChild(perfVt, -1, nullptr);

    // Locks & flags
    juce::ValueTree lockVt("Locks");
    lockVt.setProperty("style", snap.lockStyle, nullptr);
    lockVt.setProperty("keyScale", snap.lockKeyScale, nullptr);
    lockVt.setProperty("motif", snap.lockMotif, nullptr);
    lockVt.setProperty("strategy", snap.lockStrategy, nullptr);
    lockVt.setProperty("rhythm", snap.lockRhythm, nullptr);
    lockVt.setProperty("pitches", snap.lockPitches, nullptr);
    lockVt.setProperty("muteAudio", snap.muteAudio, nullptr);
    vt.addChild(lockVt, -1, nullptr);

    // Phrases
    vt.addChild(phraseToValueTree(snap.call, "CallPhrase"), -1, nullptr);
    vt.addChild(phraseToValueTree(snap.response, "ResponsePhrase"), -1, nullptr);

    return vt;
}

StateSnapshot PresetManager::valueTreeToSnapshot(const juce::ValueTree& vt) {
    StateSnapshot snap;
    auto cfgVt = vt.getChildWithName("Config");
    if (cfgVt.isValid()) {
        snap.config.keyIndex = cfgVt.getProperty("keyIndex", snap.config.keyIndex);
        snap.config.scaleIndex = cfgVt.getProperty("scaleIndex", snap.config.scaleIndex);
        snap.config.rootOctaveIndex = cfgVt.getProperty("rootOctaveIndex", snap.config.rootOctaveIndex);
        snap.config.steps = cfgVt.getProperty("steps", snap.config.steps);
        snap.config.styleIndex = cfgVt.getProperty("styleIndex", snap.config.styleIndex);
        snap.config.responseStrategyIndex = cfgVt.getProperty("responseStrategyIndex", snap.config.responseStrategyIndex);
        snap.config.motifTypeIndex = cfgVt.getProperty("motifTypeIndex", snap.config.motifTypeIndex);
        snap.config.phraseModeIndex = cfgVt.getProperty("phraseModeIndex", snap.config.phraseModeIndex);
        snap.config.endingBehaviourIndex = cfgVt.getProperty("endingBehaviourIndex", snap.config.endingBehaviourIndex);
        snap.config.registerProfileIndex = cfgVt.getProperty("registerProfileIndex", snap.config.registerProfileIndex);
        snap.config.densityPresetIndex = cfgVt.getProperty("densityPresetIndex", snap.config.densityPresetIndex);
        snap.config.contourPresetIndex = cfgVt.getProperty("contourPresetIndex", snap.config.contourPresetIndex);
        snap.config.repetitionPresetIndex = cfgVt.getProperty("repetitionPresetIndex", snap.config.repetitionPresetIndex);
        snap.config.mutationAmountIndex = cfgVt.getProperty("mutationAmountIndex", snap.config.mutationAmountIndex);
        snap.config.mutationTargetIndex = cfgVt.getProperty("mutationTargetIndex", snap.config.mutationTargetIndex);
        snap.config.notePoolModeIndex = cfgVt.getProperty("notePoolModeIndex", snap.config.notePoolModeIndex);
        snap.config.articulationProfileIndex = cfgVt.getProperty("articulationProfileIndex", snap.config.articulationProfileIndex);
        snap.config.gateFeelIndex = cfgVt.getProperty("gateFeelIndex", snap.config.gateFeelIndex);
        snap.config.connectionFeelIndex = cfgVt.getProperty("connectionFeelIndex", snap.config.connectionFeelIndex);
        snap.config.slideProfileIndex = cfgVt.getProperty("slideProfileIndex", snap.config.slideProfileIndex);
        snap.config.accentProfileIndex = cfgVt.getProperty("accentProfileIndex", snap.config.accentProfileIndex);
        snap.config.callChannel = cfgVt.getProperty("callChannel", snap.config.callChannel);
        snap.config.responseChannel = cfgVt.getProperty("responseChannel", snap.config.responseChannel);
    }

    auto perfVt = vt.getChildWithName("Performance");
    if (perfVt.isValid()) {
        snap.perf.density = (float)(double)perfVt.getProperty("density", 0.5);
        snap.perf.noteLength = (float)(double)perfVt.getProperty("noteLength", 1.0);
        snap.perf.octaveJump = (float)(double)perfVt.getProperty("octaveJump", 0.0);
        snap.perf.resolve = (float)(double)perfVt.getProperty("resolve", 0.5);
    }

    auto lockVt = vt.getChildWithName("Locks");
    if (lockVt.isValid()) {
        snap.lockStyle = lockVt.getProperty("style", false);
        snap.lockKeyScale = lockVt.getProperty("keyScale", false);
        snap.lockMotif = lockVt.getProperty("motif", false);
        snap.lockStrategy = lockVt.getProperty("strategy", false);
        snap.lockRhythm = lockVt.getProperty("rhythm", false);
        snap.lockPitches = lockVt.getProperty("pitches", false);
        snap.muteAudio = lockVt.getProperty("muteAudio", false);
    }

    auto callVt = vt.getChildWithName("CallPhrase");
    if (callVt.isValid()) snap.call = valueTreeToPhrase(callVt);

    auto respVt = vt.getChildWithName("ResponsePhrase");
    if (respVt.isValid()) snap.response = valueTreeToPhrase(respVt);

    return snap;
}

juce::ValueTree PresetManager::toValueTree() const {
    juce::ValueTree root("PresetManager");
    for (size_t i = 0; i < 8; ++i) {
        if (slotFilled[i]) {
            juce::String name = "Slot_" + juce::String((int)i);
            root.addChild(snapshotToValueTree(slots[i], name), -1, nullptr);
        }
    }
    return root;
}

void PresetManager::fromValueTree(const juce::ValueTree& vt) {
    if (!vt.isValid() || vt.getType() != juce::Identifier("PresetManager")) return;
    for (size_t i = 0; i < 8; ++i) {
        juce::String name = "Slot_" + juce::String((int)i);
        auto child = vt.getChildWithName(name);
        if (child.isValid()) {
            slots[i] = valueTreeToSnapshot(child);
            slotFilled[i] = true;
        } else {
            slotFilled[i] = false;
        }
    }
}

} // namespace CR
