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
    bool lockCall = false;
    bool lockResponse = false;
    bool muteAudio = false;
};

struct UndoStep {
    std::vector<Step> call;
    std::vector<Step> response;
};

class PresetManager {
public:
    PresetManager();

    // 8 Preset Slots (A - H)
    void saveToSlot(int slotIndex, const StateSnapshot& snapshot);
    bool loadFromSlot(int slotIndex, StateSnapshot& outSnapshot);
    bool isSlotOccupied(int slotIndex) const;
    int getCurrentSlot() const { return currentSlot; }
    void setCurrentSlot(int slot) { currentSlot = slot; }

    // Undo / Redo (24 levels, affects last phrase change without overwriting global plugin state)
    void pushUndo(const UndoStep& step);
    bool canUndo() const;
    bool canRedo() const;
    UndoStep undo(const UndoStep& current);
    UndoStep redo(const UndoStep& current);

    // Serialization
    static juce::ValueTree snapshotToValueTree(const StateSnapshot& snapshot, const juce::String& name = "Snapshot");
    static StateSnapshot valueTreeToSnapshot(const juce::ValueTree& vt);

    juce::ValueTree toValueTree() const;
    void fromValueTree(const juce::ValueTree& vt);

private:
    int currentSlot = -1;
    std::vector<StateSnapshot> slots;
    std::vector<bool> slotFilled;

    std::deque<UndoStep> undoStack;
    std::deque<UndoStep> redoStack;
    static constexpr size_t maxHistory = 24;
};

} // namespace CR
