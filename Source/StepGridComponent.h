#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "GeneratorEngine.h"
#include <functional>

namespace CR {

class StepGridComponent : public juce::Component {
public:
    StepGridComponent(const juce::String& laneTitle, juce::Colour accentColor);
    ~StepGridComponent() override;

    void setPhrase(const std::vector<Step>& steps, const GeneratorConfig& config);
    const std::vector<Step>& getPhrase() const { return phrase; }

    void setCurrentStep(int stepIndex);
    void setMuted(bool mute);
    bool isMuted() const { return isMute; }

    void setChannel(int channelNumber);
    int getChannel() const;

    void triggerMidiActivity();

    std::function<void()> onPhraseChanged;
    std::function<void(int delta)> onShiftRequested;
    std::function<void()> onInvertRequested;
    std::function<void()> onReverseRequested;
    std::function<void(int octaves)> onOctaveRequested;
    std::function<void(int channel)> onChannelChanged;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

private:
    juce::String title;
    juce::Colour laneAccent;
    std::vector<Step> phrase;
    GeneratorConfig currentConfig;
    int playingStep = -1;
    bool isMute = false;
    float midiActivityIntensity = 0.0f;

    // Header controls
    juce::ComboBox   cbChannel;
    juce::TextButton btnShiftLeft  { "<" };
    juce::TextButton btnShiftRight { ">" };
    juce::TextButton btnOctUp      { "+8ve" };
    juce::TextButton btnOctDown    { "-8ve" };
    juce::TextButton btnInvert     { "Inv" };
    juce::TextButton btnReverse    { "Rev" };
    juce::TextButton btnMute       { "MUTE" };

    // Interaction tracking
    int draggedStepIndex = -1;
    float dragStartY = 0.0f;
    int dragStartDegree = 0;

    juce::Rectangle<int> getGridBounds() const;
    int getStepIndexAtPoint(juce::Point<int> pt) const;
};

} // namespace CR
