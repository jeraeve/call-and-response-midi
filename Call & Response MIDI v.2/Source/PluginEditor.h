#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "StepperComponent.h"
#include "InspectorPanel.h"
#include "StepGridComponent.h"
#include "PianoRollComponent.h"
#include "MidiDragComponent.h"

namespace CR {

class CallResponseAudioProcessorEditor : public juce::AudioProcessorEditor,
                                         private juce::Timer {
public:
    explicit CallResponseAudioProcessorEditor (CallResponseAudioProcessor&);
    ~CallResponseAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateUiFromProcessor();
    void syncConfigToProcessor();

    CallResponseAudioProcessor& processorRef;
    CustomLookAndFeel customLookAndFeel;

    // 1. Top Ribbon Controls
    juce::Label lblLogo;
    StepperComponent stepOctave    { "OCT",   StepperComponent::LayoutMode::RibbonInline };
    StepperComponent stepKey       { "KEY",   StepperComponent::LayoutMode::RibbonInline };
    StepperComponent stepScale     { "SCALE", StepperComponent::LayoutMode::RibbonInline };
    juce::TextButton btnEnharmonic { "b/#" };
    StepperComponent stepSteps     { "STEPS", StepperComponent::LayoutMode::RibbonInline };
    StepperComponent stepRate      { "RATE",  StepperComponent::LayoutMode::RibbonInline };
    StepperComponent stepSwing     { "SWING", StepperComponent::LayoutMode::RibbonInline };
    juce::TextButton btnPanic      { "PANIC" };

    // Utility icons on top right
    std::array<PresetSlotButton, 8> presetSlotButtons;
    juce::TextButton btnUndo       { "Undo" };
    juce::TextButton btnRedo       { "Redo" };
    juce::TextButton btnAudioMute  { "PREVIEW: ON" };

    // 2. Upper Dashboard - Left Column (Quick Controls & Knobs)
    juce::TextButton btnNewPair { "NEW PAIR" };
    juce::TextButton btnDice    { "DICE" };

    juce::Slider sldDensity;
    juce::Slider sldMutate;
    juce::Slider sldOctaveJump;

    juce::Label lblDensityVal;
    juce::Label lblMutateVal;
    juce::Label lblOctJumpVal;

    // Rectangular shift buttons under knobs (< OFFSET >)
    juce::TextButton btnShiftPhraseLeft  { "<" };
    juce::Label      lblOffset;
    juce::TextButton btnShiftPhraseRight { ">" };
    juce::Label lblCredits;

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> densityAttach;
    std::unique_ptr<SliderAttachment> octaveJumpAttach;
    std::unique_ptr<SliderAttachment> resolveAttach;
    std::unique_ptr<SliderAttachment> gateAttach;
    std::unique_ptr<SliderAttachment> cutoffAttach;
    std::unique_ptr<SliderAttachment> resAttach;
    std::unique_ptr<SliderAttachment> decayAttach;

    // 3. Upper Dashboard - Center Column (Dual Step Lanes)
    StepGridComponent callGrid;
    StepGridComponent responseGrid;

    // 4. Upper Dashboard - Right Column (Tabbed Inspector Panel)
    InspectorPanel inspectorPanel;

    // 5. Middle Section (Piano Roll Melody Editor)
    PianoRollComponent pianoRollComp;

    // 6. Bottom Bar (Mutation & MIDI Drag & Drop)
    juce::ComboBox cbMutateTarget;
    juce::ComboBox cbMutateAmount;
    juce::TextButton btnMutate { "MUTATE" };

    juce::ComboBox cbExportMode;
    juce::ToggleButton toggleWholeBars { "Whole Bars" };
    MidiDragComponent midiDragComp;
    juce::Label lblStatus;

    bool enharmonicFlats = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CallResponseAudioProcessorEditor)
};

} // namespace CR
