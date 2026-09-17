#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "GeneratorEngine.h"
#include <functional>

namespace CR {

class PianoRollComponent : public juce::Component {
public:
    enum class ViewLane {
        Call,
        Response,
        Both
    };

    PianoRollComponent();
    ~PianoRollComponent() override;

    void setPhrases(const std::vector<Step>& call, const std::vector<Step>& response, const GeneratorConfig& config);
    void setViewLane(ViewLane lane);
    ViewLane getViewLane() const { return currentView; }

    void setCurrentStep(int stepIndex, int phase); // phase: 0=call, 1=response

    std::function<void(const std::vector<Step>&, bool isCall)> onPhraseEdited;
    std::function<void(int midiNote, bool isNoteOn)> onAuditionNote;
    std::function<void(ViewLane)> onViewLaneChanged;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;

private:
    ViewLane currentView = ViewLane::Call;
    std::vector<Step> callPhrase;
    std::vector<Step> responsePhrase;
    GeneratorConfig currentConfig;

    int playingStep = -1;
    int playingPhase = 0;

    // View range
    int minMidi = 36; // C2
    int maxMidi = 72; // C5
    static constexpr int numKeys = 37; // 36 to 72

    static constexpr int pianoKeyWidth = 54;
    static constexpr int headerHeight = 32;

    // Tab buttons
    juce::TextButton btnCallTab     { "CALL" };
    juce::TextButton btnResponseTab { "RESPONSE" };
    juce::TextButton btnBothTab     { "BOTH (SPLIT)" };

    // Interaction state
    enum class DragAction {
        None,
        PianoKeyAudition,
        MoveNote,
        ResizeNoteGate
    };

    DragAction currentDragAction = DragAction::None;
    int draggedStep = -1;
    bool draggedIsCall = true;
    int auditioningMidi = -1;
    float dragStartY = 0.0f;
    int dragStartMidi = 60;
    int dragStartStep = -1;
    float dragStartGate = 0.82f;
    std::vector<Step> dragInitialPhrase;

    // Helper geometry
    juce::Rectangle<int> getGridArea() const;
    juce::Rectangle<int> getKeyboardArea() const;
    float getKeyHeight() const;
    float getStepWidth(int totalSteps) const;
    int midiNoteAtY(int y) const;
    float yForMidiNote(int midi) const;
    int stepAtX(int x, int totalSteps) const;

    bool isBlackKey(int midi) const;
    bool isInCurrentScale(int midi) const;

    void updateAutoRange();
};

} // namespace CR
