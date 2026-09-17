#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
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

    // Header Components
    juce::Label lblTitle;
    juce::Label lblSubtitle;
    juce::TextButton btnUndo { "Undo" };
    juce::TextButton btnRedo { "Redo" };
    PresetSlotButton btnPresetSlots[8];
    juce::TextButton btnAudioMute { "PREVIEW: ON" };
    juce::TextButton btnPanic { "PANIC" };

    // Generation Controls
    juce::ComboBox cbStyle;
    juce::ComboBox cbStrategy;
    juce::ComboBox cbKey;
    juce::ComboBox cbScale;
    juce::ComboBox cbRootOctave;
    juce::ComboBox cbMotifType;
    juce::ComboBox cbEnding;
    juce::ComboBox cbPhraseMode;
    juce::ComboBox cbSteps;

    // Locks
    LockIconButton lockStyle    { "Lock" };
    LockIconButton lockKeyScale { "Lock" };
    LockIconButton lockMotif    { "Lock" };
    LockIconButton lockStrategy { "Lock" };

    // Generation Action Buttons
    juce::TextButton btnNewPair { "NEW PAIR" };
    juce::TextButton btnDice    { "DICE" };
    juce::TextButton btnRegenTiming { "NOW" }; // NOW vs NEXT PHRASE (shows QUEUED)

    // Performance Knobs & Sliders
    juce::Slider sldDensity;
    juce::Slider sldGate;
    juce::Slider sldOctaveJump;
    juce::Slider sldResolve;

    juce::Label lblDensity;
    juce::Label lblGate;
    juce::Label lblOctaveJump;
    juce::Label lblResolve;

    // Synth mini controls
    juce::Slider sldCutoff;
    juce::Slider sldResonance;
    juce::Slider sldDecay;
    juce::Label lblCutoff;
    juce::Label lblRes;
    juce::Label lblDecay;

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> densityAttach;
    std::unique_ptr<SliderAttachment> gateAttach;
    std::unique_ptr<SliderAttachment> octaveJumpAttach;
    std::unique_ptr<SliderAttachment> resolveAttach;
    std::unique_ptr<SliderAttachment> cutoffAttach;
    std::unique_ptr<SliderAttachment> resAttach;
    std::unique_ptr<SliderAttachment> decayAttach;

    // Dual Sequencer Lanes
    StepGridComponent callGrid;
    StepGridComponent responseGrid;

    // Interactive Piano Roll
    PianoRollComponent pianoRollComp;

    enum class EditorViewMode {
        StepGrid,
        PianoRoll,
        SplitView
    };
    EditorViewMode currentViewMode = EditorViewMode::StepGrid;
    void setViewMode(EditorViewMode mode);

    enum class SplitLaneMode {
        Call,
        Response,
        Both
    };
    SplitLaneMode splitLaneMode = SplitLaneMode::Both;
    void setSplitLaneMode(SplitLaneMode mode);

    juce::TextButton btnViewStepGrid  { "STEPS" };
    juce::TextButton btnViewPianoRoll { "PIANO ROLL" };
    juce::TextButton btnViewSplit     { "SPLIT" };

    // Split View Sub-Options
    juce::TextButton btnSplitCall     { "CALL" };
    juce::TextButton btnSplitResponse { "RESP" };
    juce::TextButton btnSplitBoth     { "BOTH" };

    // Bottom Bar (Mutation & Drag & Drop)
    juce::ComboBox cbMutateTarget;
    juce::ComboBox cbMutateAmount;
    juce::TextButton btnMutate { "MUTATE" };

    juce::ComboBox cbExportMode;
    juce::ToggleButton toggleWholeBars { "Whole Bars" };
    MidiDragComponent midiDragComp;

    juce::Label lblStatus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CallResponseAudioProcessorEditor)
};

} // namespace CR
