#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "GeneratorEngine.h"
#include <functional>

namespace CR {

class StepGridComponent : public juce::Component {
public:
    StepGridComponent (const juce::String& laneTitle, juce::Colour accentColor);
    ~StepGridComponent() override;

    void setPhrase (const std::vector<Step>& steps, const GeneratorConfig& config);
    const std::vector<Step>& getPhrase() const { return phrase; }

    void setCurrentStep (int stepIndex);
    void setMuted (bool mute);
    bool isMuted() const { return isMute; }

    void setLocked (bool locked);
    bool isLocked() const { return isLock; }

    void setChannel (int channelNumber);
    int getChannel() const;

    void triggerMidiActivity();

    std::function<void()> onPhraseChanged;
    std::function<void()> onEditGestureStarted;
    std::function<void(bool isMuted)> onMuteToggled;
    std::function<void(bool isLocked)> onLockToggled;
    std::function<void()> onDiceRequested;
    std::function<void(int percent)> onMutatePercentChanged;
    std::function<void(int delta)> onShiftRequested;
    std::function<void()> onInvertRequested;
    std::function<void()> onReverseRequested;
    std::function<void(int octaves)> onOctaveRequested;
    std::function<void(int channel)> onChannelChanged;
    std::function<void(int midiNote, bool isNoteOn)> onAuditionNote;

    void setMutatePercent (int pct);
    int getMutatePercent() const;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

private:
    juce::String title;
    juce::Colour laneAccent;
    std::vector<Step> phrase;
    GeneratorConfig currentConfig;
    int playingStep = -1;
    bool isMute = false;
    bool isLock = false;
    float midiActivityIntensity = 0.0f;
    int auditioningMidiNote = -1;

    class LaneLockButton;
    std::unique_ptr<LaneLockButton> btnLockLane;

    // Header Controls on the left under badge
    juce::TextButton btnMuteLane   { "MUTE" };
    juce::TextButton btnDiceLane   { juce::CharPointer_UTF8 ("\xe2\x9a\xb2") }; // Dice
    juce::TextButton btnMutatePct  { "MUTATE  0%" };
    int mutatePercent = 0;

    // Compact utilities
    juce::ComboBox   cbChannel;
    juce::TextButton btnCopy       { "C" };
    juce::TextButton btnShiftLeft  { "<" };
    juce::TextButton btnShiftRight { ">" };
    juce::TextButton btnOctUp      { "+8ve" };
    juce::TextButton btnOctDown    { "-8ve" };
    juce::TextButton btnInvert     { "Inv" };
    juce::TextButton btnReverse    { "Rev" };

    // Interaction tracking
    int draggedStepIndex = -1;
    float dragStartY = 0.0f;
    int dragStartDegree = 0;

    juce::Rectangle<int> getGridBounds() const;
    int getStepIndexAtPoint (juce::Point<int> pt) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepGridComponent)
};

} // namespace CR
