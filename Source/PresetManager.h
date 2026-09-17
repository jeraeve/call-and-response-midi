#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include "GeneratorEngine.h"
#include <vector>
#include <deque>

namespace CR {

struct StateSnapshot {
    GeneratorConfig config;
    PerformanceParams perf;
    std::vector<Step> call;
    std::vector<Step> response;
    bool lockStyle = false;
    bool lockKeyScale = false;
    bool lockMotif = false;
    bool lockStrategy = false;
    bool lockRhythm = false;
    bool lockPitches = false;
    bool muteAudio = false;
};

class PresetManager {
public:
    PresetManager();

    // 8 Preset Slots (A - H)
    void saveToSlot(int slotIndex, const StateSnapshot& snapshot);
    bool loadFromSlot(int slotIndex, StateSnapshot& outSnapshot) const;
    bool isSlotOccupied(int slotIndex) const;

    // Undo / Redo (24 levels)
    void pushUndo(const StateSnapshot& snapshot);
    bool canUndo() const;
    bool canRedo() const;
    StateSnapshot undo(const StateSnapshot& current);
    StateSnapshot redo(const StateSnapshot& current);

    // Serialization
    static juce::ValueTree snapshotToValueTree(const StateSnapshot& snapshot, const juce::String& name = "Snapshot");
    static StateSnapshot valueTreeToSnapshot(const juce::ValueTree& vt);

    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& vt);

private:
    std::vector<StateSnapshot> slots;
    std::vector<bool> slotFilled;

    std::deque<StateSnapshot> undoStack;
    std::deque<StateSnapshot> redoStack;
    static constexpr size_t maxHistory = 24;
};

} // namespace CR
