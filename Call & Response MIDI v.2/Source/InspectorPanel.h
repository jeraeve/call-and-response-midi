#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "StepperComponent.h"
#include "GeneratorEngine.h"
#include "PluginProcessor.h"

namespace CR {

class InspectorPanel : public juce::Component {
public:
    enum class MainTab {
        Generate,
        Shape,
        Articulation,
        Options
    };

    enum class OptionsSubTab {
        Routing,
        TimingClips
    };

    explicit InspectorPanel (CallResponseAudioProcessor& processor);
    ~InspectorPanel() override;

    void updateFromConfig (const GeneratorConfig& config);
    void updateFromProcessor();

    void paint (juce::Graphics& g) override;
    void resized() override;

    std::function<void()> onConfigChanged;
    std::function<void(int mode)> onClipModeChanged;
    std::function<void(bool wholeBars)> onLoopLengthChanged;

private:
    CallResponseAudioProcessor& processorRef;
    bool isInternalUpdating = false;

    MainTab currentTab = MainTab::Generate;
    OptionsSubTab currentSubTab = OptionsSubTab::Routing;
    void setTab (MainTab tab);
    void setOptionsSubTab (OptionsSubTab subTab);

    // Tab Bar
    juce::TextButton btnTabGenerate    { "GENERATE" };
    juce::TextButton btnTabShape       { "SHAPE" };
    juce::TextButton btnTabArticulation{ "ARTICULATION" };
    juce::TextButton btnTabOptions     { "OPTIONS" };

    // Subtabs for Options
    juce::TextButton btnSubRouting     { "ROUTING" };
    juce::TextButton btnSubTimingClips { "TIMING / CLIPS" };

    // Tab 1: GENERATE
    StepperComponent stepStyle   { "STYLE" };
    StepperComponent stepMotif   { "MOTIF" };
    StepperComponent stepAnswer  { "ANSWER" };
    StepperComponent stepMode    { "MODE" };
    StepperComponent stepEnding  { "ENDING" };

    // Tab 2: SHAPE
    StepperComponent stepRegister  { "REGISTER" };
    StepperComponent stepContour   { "CONTOUR" };
    StepperComponent stepRepetition{ "REPETITION" };
    StepperComponent stepNotePool  { "NOTE POOL" };
    StepperComponent stepResolve   { "RESOLVE" };

    // Tab 3: ARTICULATION
    StepperComponent stepArticProfile { "PROFILE" };
    StepperComponent stepGateFeel     { "GATE" };
    StepperComponent stepJoinTies     { "JOIN" };
    StepperComponent stepSlideProfile { "SLIDE" };
    StepperComponent stepAccentProfile{ "ACCENT" };
    StepperComponent stepVelocity     { "VELOCITY" };
    StepperComponent stepAccentAmount { "ACCENT AMT" };

    // Tab 4: OPTIONS (Routing)
    StepperComponent stepCallOut   { "CALL OUT" };
    StepperComponent stepRespOut   { "RESP OUT" };
    StepperComponent stepSync      { "SYNC" };
    StepperComponent stepVariation { "VARIATION" };
    StepperComponent stepSynthMute { "SYNTH" };

    // Tab 4: OPTIONS (Timing / Clips)
    StepperComponent stepNewPairTiming { "NEW PAIR" };
    StepperComponent stepClipMode      { "CLIP" };
    StepperComponent stepLoopLength    { "LOOP LENGTH" };

    // Articulation Legend
    juce::Label lblLegend;

    void setupSteppers();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorPanel)
};

} // namespace CR
