#include "PluginEditor.h"

namespace CR {

CallResponseAudioProcessorEditor::CallResponseAudioProcessorEditor (CallResponseAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      callGrid ("CALL LANE", CustomLookAndFeel::getNeonCyan()),
      responseGrid ("RESPONSE LANE", CustomLookAndFeel::getNeonOrange())
{
    setLookAndFeel (&customLookAndFeel);

    // Make editor modern and resizable
    setResizable (true, true);
    setResizeLimits (940, 660, 1920, 1200);
    setSize (1080, 720);

    // Title & Header
    lblTitle.setText ("CALL & RESPONSE", juce::dontSendNotification);
    lblTitle.setFont (juce::FontOptions (20.0f, juce::Font::bold));
    lblTitle.setColour (juce::Label::textColourId, CustomLookAndFeel::getActiveText());
    addAndMakeVisible (lblTitle);

    lblSubtitle.setText ("VIRAL SAMPLES  |  v1.0.5", juce::dontSendNotification);
    lblSubtitle.setFont (juce::FontOptions (10.5f, juce::Font::bold));
    lblSubtitle.setColour (juce::Label::textColourId, CustomLookAndFeel::getNeonCyan());
    addAndMakeVisible (lblSubtitle);

    // Preset Slots A-H
    for (int i = 0; i < 8; ++i) {
        char slotChar = (char)('A' + i);
        btnPresetSlots[i].setButtonText (juce::String::charToString(slotChar));
        btnPresetSlots[i].slotIndex = i;
        btnPresetSlots[i].setTooltip ("Double-click to load Preset " + juce::String::charToString(slotChar) + " (Cmd+click to save)");
        addAndMakeVisible (btnPresetSlots[i]);

        btnPresetSlots[i].onSave = [this, slotChar](int slot) {
            processorRef.savePresetSlot (slot);
            updateUiFromProcessor();
            lblStatus.setText ("Saved current state to Preset Slot " + juce::String::charToString(slotChar), juce::dontSendNotification);
        };

        btnPresetSlots[i].onLoad = [this, slotChar](int slot) {
            if (processorRef.getPresetManager().isSlotOccupied (slot)) {
                processorRef.recallPresetSlot (slot);
                updateUiFromProcessor();
                lblStatus.setText ("Loaded Preset Slot " + juce::String::charToString(slotChar), juce::dontSendNotification);
            } else {
                lblStatus.setText ("Preset Slot " + juce::String::charToString(slotChar) + " is empty. Cmd+click to save current state.", juce::dontSendNotification);
            }
        };

        btnPresetSlots[i].onSelect = [this, slotChar](int slot) {
            bool occ = processorRef.getPresetManager().isSlotOccupied (slot);
            lblStatus.setText ("Preset " + juce::String::charToString(slotChar) + ": " + (occ ? "Occupied" : "Empty") + " - Double-click to load, Cmd+click to save", juce::dontSendNotification);
        };
    }

    // Undo / Redo
    btnUndo.setTooltip ("Undo last generation / change (24 steps)");
    btnRedo.setTooltip ("Redo last undone change");
    addAndMakeVisible (btnUndo);
    addAndMakeVisible (btnRedo);
    btnUndo.onClick = [this] {
        processorRef.undo();
        updateUiFromProcessor();
        lblStatus.setText("Undo applied", juce::dontSendNotification);
    };
    btnRedo.onClick = [this] {
        processorRef.redo();
        updateUiFromProcessor();
        lblStatus.setText("Redo applied", juce::dontSendNotification);
    };

    // Preview Synth Mute toggle
    btnAudioMute.setClickingTogglesState (true);
    btnAudioMute.setToggleState (!processorRef.muteAudio, juce::dontSendNotification);
    btnAudioMute.setTooltip ("Enable/disable internal 303 preview synth audio (mute when routing MIDI to other synths)");
    addAndMakeVisible (btnAudioMute);
    btnAudioMute.onClick = [this] {
        processorRef.muteAudio = !btnAudioMute.getToggleState();
        btnAudioMute.setButtonText (btnAudioMute.getToggleState() ? "PREVIEW: ON" : "PREVIEW: MUTED");
    };

    btnPanic.setTooltip ("Panic: Send All Notes Off & All Sound Off on all 16 MIDI channels");
    addAndMakeVisible (btnPanic);
    btnPanic.onClick = [this] {
        processorRef.triggerPanic();
        lblStatus.setText ("Panic: All notes off sent on all 16 MIDI channels", juce::dontSendNotification);
    };

    // Helper for ComboBox setup
    auto setupBox = [this](juce::ComboBox& box, const std::vector<juce::String>& items, int selectedId) {
        box.clear();
        for (size_t i = 0; i < items.size(); ++i) {
            box.addItem (items[i], (int)i + 1);
        }
        box.setSelectedId (selectedId + 1, juce::dontSendNotification);
        addAndMakeVisible (box);
    };

    auto cfg = processorRef.getConfig();
    setupBox (cbStyle, GeneratorEngine::getStyles(), cfg.styleIndex);
    setupBox (cbStrategy, GeneratorEngine::getResponseStrategies(), cfg.responseStrategyIndex);
    setupBox (cbKey, GeneratorEngine::getKeys(), cfg.keyIndex);
    setupBox (cbScale, GeneratorEngine::getScales(), cfg.scaleIndex);
    setupBox (cbRootOctave, GeneratorEngine::getOctaves(), cfg.rootOctaveIndex);
    setupBox (cbMotifType, GeneratorEngine::getMotifTypes(), cfg.motifTypeIndex);
    setupBox (cbEnding, GeneratorEngine::getEndingBehaviours(), cfg.endingBehaviourIndex);
    setupBox (cbPhraseMode, GeneratorEngine::getPhraseModes(), cfg.phraseModeIndex);

    cbSteps.addItemList ({"4 Steps", "8 Steps", "12 Steps", "16 Steps"}, 1);
    cbSteps.setSelectedId (cfg.steps == 16 ? 4 : (cfg.steps == 12 ? 3 : (cfg.steps == 8 ? 2 : 1)), juce::dontSendNotification);
    addAndMakeVisible (cbSteps);

    // Locks
    auto setupLock = [this](LockIconButton& lock, bool& stateRef, const juce::String& tip) {
        lock.setToggleState (stateRef, juce::dontSendNotification);
        lock.setTooltip (tip);
        addAndMakeVisible (lock);
        lock.onClick = [&stateRef, &lock] { stateRef = lock.getToggleState(); };
    };

    setupLock (lockStyle,    processorRef.lockStyle,    "Lock Style from being rerolled");
    setupLock (lockKeyScale, processorRef.lockKeyScale, "Lock Key & Scale from being rerolled");
    setupLock (lockMotif,    processorRef.lockMotif,    "Lock Motif type from being rerolled");
    setupLock (lockStrategy, processorRef.lockStrategy, "Lock Response Strategy from being rerolled");

    // Action Buttons
    btnNewPair.setTooltip ("Generate a new connected Call and Response pair");
    addAndMakeVisible (btnNewPair);
    btnNewPair.onClick = [this] {
        if (processorRef.nextPhraseRegen && processorRef.getIsPlaying()) {
            processorRef.isRegenQueued.store (true);
            btnNewPair.setButtonText ("QUEUED");
            lblStatus.setText ("Regeneration queued for next phrase boundary...", juce::dontSendNotification);
        } else {
            processorRef.generateNewPair();
            updateUiFromProcessor();
            lblStatus.setText ("Generated new Call & Response pair", juce::dontSendNotification);
        }
    };

    btnDice.setTooltip ("Reroll unlocked generation settings (Dice)");
    addAndMakeVisible (btnDice);
    btnDice.onClick = [this] {
        processorRef.rerollUnlocked();
        updateUiFromProcessor();
        lblStatus.setText ("Rerolled unlocked parameters", juce::dontSendNotification);
    };

    btnRegenTiming.setTooltip ("Regeneration timing: NOW or at NEXT PHRASE boundary");
    addAndMakeVisible (btnRegenTiming);
    btnRegenTiming.onClick = [this] {
        processorRef.nextPhraseRegen = !processorRef.nextPhraseRegen;
        btnRegenTiming.setButtonText (processorRef.nextPhraseRegen ? "NEXT PHRASE" : "NOW");
    };

    // Performance Rotaries
    auto setupKnob = [this](juce::Slider& sld, juce::Label& lbl, const juce::String& name, juce::Colour col) {
        sld.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        sld.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
        sld.setColour (juce::Slider::rotarySliderFillColourId, col);
        addAndMakeVisible (sld);

        lbl.setText (name, juce::dontSendNotification);
        lbl.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        lbl.setJustificationType (juce::Justification::centred);
        lbl.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText());
        addAndMakeVisible (lbl);
    };

    setupKnob (sldDensity,    lblDensity,    "DENSITY",     CustomLookAndFeel::getNeonCyan());
    setupKnob (sldGate,       lblGate,       "GATE",        CustomLookAndFeel::getNeonCyan());
    setupKnob (sldOctaveJump, lblOctaveJump, "OCT JUMP",    CustomLookAndFeel::getNeonOrange());
    setupKnob (sldResolve,    lblResolve,    "RESOLVE",     CustomLookAndFeel::getNeonEmerald());

    setupKnob (sldCutoff,     lblCutoff,     "CUTOFF",      CustomLookAndFeel::getNeonViolet());
    setupKnob (sldResonance,  lblRes,        "RESONANCE",   CustomLookAndFeel::getNeonViolet());
    setupKnob (sldDecay,      lblDecay,      "DECAY",       CustomLookAndFeel::getNeonViolet());

    // APVTS Attachments
    densityAttach    = std::make_unique<SliderAttachment> (processorRef.apvts, "density", sldDensity);
    gateAttach       = std::make_unique<SliderAttachment> (processorRef.apvts, "noteLength", sldGate);
    octaveJumpAttach = std::make_unique<SliderAttachment> (processorRef.apvts, "octaveJump", sldOctaveJump);
    resolveAttach    = std::make_unique<SliderAttachment> (processorRef.apvts, "resolve", sldResolve);
    cutoffAttach     = std::make_unique<SliderAttachment> (processorRef.apvts, "cutoff", sldCutoff);
    resAttach        = std::make_unique<SliderAttachment> (processorRef.apvts, "resonance", sldResonance);
    decayAttach      = std::make_unique<SliderAttachment> (processorRef.apvts, "decay", sldDecay);

    // View Mode Switcher
    auto setupViewBtn = [this](juce::TextButton& btn, EditorViewMode mode, const juce::String& tip) {
        btn.setClickingTogglesState (true);
        btn.setRadioGroupId (2001);
        btn.setTooltip (tip);
        addAndMakeVisible (btn);
        btn.onClick = [this, mode] { setViewMode (mode); };
    };

    setupViewBtn (btnViewStepGrid,  EditorViewMode::StepGrid,  "Switch to Step Sequencer Grid view");
    setupViewBtn (btnViewPianoRoll, EditorViewMode::PianoRoll, "Switch to Interactive Piano Roll Melody Editor");
    setupViewBtn (btnViewSplit,     EditorViewMode::SplitView, "Switch to Split View (Step Grid + Piano Roll)");
    btnViewStepGrid.setToggleState (true, juce::dontSendNotification);

    // Split View Sub-Lane Options
    auto setupSplitBtn = [this](juce::TextButton& btn, SplitLaneMode mode, const juce::String& tip) {
        btn.setClickingTogglesState (true);
        btn.setRadioGroupId (2002);
        btn.setTooltip (tip);
        addChildComponent (btn);
        btn.onClick = [this, mode] { setSplitLaneMode (mode); };
    };

    setupSplitBtn (btnSplitBoth,     SplitLaneMode::Both,     "Split View: Show Both Call & Response lanes");
    setupSplitBtn (btnSplitCall,     SplitLaneMode::Call,     "Split View: Show Call lane");
    setupSplitBtn (btnSplitResponse, SplitLaneMode::Response, "Split View: Show Response lane");
    btnSplitBoth.setToggleState (true, juce::dontSendNotification);

    // Dual Sequencer Lanes
    addAndMakeVisible (callGrid);
    addAndMakeVisible (responseGrid);

    callGrid.onPhraseChanged = [this] {
        processorRef.setCallPhrase (callGrid.getPhrase());
        auto currentConfig = processorRef.getConfig();
        pianoRollComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), currentConfig);
        midiDragComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), processorRef.getCurrentBpm());
        btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
        btnRedo.setEnabled (processorRef.getPresetManager().canRedo());
    };
    callGrid.onShiftRequested = [this](int delta) {
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::shift (phrase, delta);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onOctaveRequested = [this](int oct) {
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::transposeOctave (phrase, oct);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onInvertRequested = [this] {
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::invert (phrase, processorRef.getConfig());
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onReverseRequested = [this] {
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::reverse (phrase);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onChannelChanged = [this](int ch) {
        processorRef.setCallChannel (ch);
        lblStatus.setText ("Call MIDI Output routed to Channel " + juce::String(ch), juce::dontSendNotification);
    };

    responseGrid.onPhraseChanged = [this] {
        processorRef.setResponsePhrase (responseGrid.getPhrase());
        auto currentConfig = processorRef.getConfig();
        pianoRollComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), currentConfig);
        midiDragComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), processorRef.getCurrentBpm());
        btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
        btnRedo.setEnabled (processorRef.getPresetManager().canRedo());
    };
    responseGrid.onShiftRequested = [this](int delta) {
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::shift (phrase, delta);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onOctaveRequested = [this](int oct) {
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::transposeOctave (phrase, oct);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onInvertRequested = [this] {
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::invert (phrase, processorRef.getConfig());
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onReverseRequested = [this] {
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::reverse (phrase);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onChannelChanged = [this](int ch) {
        processorRef.setResponseChannel (ch);
        lblStatus.setText ("Response MIDI Output routed to Channel " + juce::String(ch), juce::dontSendNotification);
    };

    // Piano Roll Melody Editor
    addAndMakeVisible (pianoRollComp);
    pianoRollComp.onPhraseEdited = [this](const std::vector<Step>& phrase, bool isCall) {
        auto currentConfig = processorRef.getConfig();
        if (isCall) {
            processorRef.setCallPhrase (phrase);
            callGrid.setPhrase (phrase, currentConfig);
        } else {
            processorRef.setResponsePhrase (phrase);
            responseGrid.setPhrase (phrase, currentConfig);
        }
        midiDragComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), processorRef.getCurrentBpm());
        btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
        btnRedo.setEnabled (processorRef.getPresetManager().canRedo());
        lblStatus.setText ("Melody modified in Piano Roll", juce::dontSendNotification);
    };
    pianoRollComp.onAuditionNote = [this](int midiNote, bool isNoteOn) {
        processorRef.triggerAuditionNote (midiNote, isNoteOn);
    };
    pianoRollComp.onViewLaneChanged = [this](PianoRollComponent::ViewLane lane) {
        if (currentViewMode == EditorViewMode::SplitView) {
            if (lane == PianoRollComponent::ViewLane::Call && splitLaneMode != SplitLaneMode::Call) {
                setSplitLaneMode (SplitLaneMode::Call);
            } else if (lane == PianoRollComponent::ViewLane::Response && splitLaneMode != SplitLaneMode::Response) {
                setSplitLaneMode (SplitLaneMode::Response);
            } else if (lane == PianoRollComponent::ViewLane::Both && splitLaneMode != SplitLaneMode::Both) {
                setSplitLaneMode (SplitLaneMode::Both);
            }
        }
    };

    setViewMode (EditorViewMode::StepGrid);

    // Bottom Bar (Mutation & MIDI Drag & Drop)
    setupBox (cbMutateTarget, GeneratorEngine::getMutationTargets(), cfg.mutationTargetIndex);
    setupBox (cbMutateAmount, GeneratorEngine::getMutationAmounts(), cfg.mutationAmountIndex);
    addAndMakeVisible (btnMutate);
    btnMutate.onClick = [this] {
        processorRef.mutateSelected (cbMutateTarget.getText());
        updateUiFromProcessor();
        lblStatus.setText ("Mutated phrase (" + cbMutateTarget.getText() + ")", juce::dontSendNotification);
    };

    cbExportMode.addItemList ({"Combined (Call + Resp)", "Call Only", "Response Only", "Layered"}, 1);
    cbExportMode.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (cbExportMode);
    cbExportMode.onChange = [this] {
        auto id = cbExportMode.getSelectedId();
        midiDragComp.setExportMode (id == 2 ? MidiDragComponent::ExportMode::CallOnly
                                  : (id == 3 ? MidiDragComponent::ExportMode::ResponseOnly
                                  : (id == 4 ? MidiDragComponent::ExportMode::Layered
                                  : MidiDragComponent::ExportMode::Combined)));
    };

    toggleWholeBars.setToggleState (true, juce::dontSendNotification);
    addAndMakeVisible (toggleWholeBars);
    toggleWholeBars.onClick = [this] {
        midiDragComp.setWholeBars (toggleWholeBars.getToggleState());
    };

    addAndMakeVisible (midiDragComp);

    lblStatus.setText ("Ready. Place on track, generate Call & Response, and drag MIDI or run DAW transport.", juce::dontSendNotification);
    lblStatus.setFont (juce::FontOptions (11.5f));
    lblStatus.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText());
    addAndMakeVisible (lblStatus);

    // ComboBox Change Callbacks
    auto onConfigParamChanged = [this] {
        syncConfigToProcessor();
        updateUiFromProcessor();
        auto c = processorRef.getConfig();
        if (c.keyIndex >= 0 && c.scaleIndex >= 0) {
            lblStatus.setText ("Transposed phrase to " + GeneratorEngine::getKeys()[(size_t)c.keyIndex] + " " + GeneratorEngine::getScales()[(size_t)c.scaleIndex], juce::dontSendNotification);
        }
    };

    cbStyle.onChange = onConfigParamChanged;
    cbStrategy.onChange = onConfigParamChanged;
    cbKey.onChange = onConfigParamChanged;
    cbScale.onChange = onConfigParamChanged;
    cbRootOctave.onChange = onConfigParamChanged;
    cbMotifType.onChange = onConfigParamChanged;
    cbEnding.onChange = onConfigParamChanged;
    cbPhraseMode.onChange = onConfigParamChanged;
    cbMutateTarget.onChange = onConfigParamChanged;
    cbMutateAmount.onChange = onConfigParamChanged;
    cbSteps.onChange = [this] {
        int stepsMap[] = { 4, 8, 12, 16 };
        int idx = juce::jlimit(1, 4, cbSteps.getSelectedId()) - 1;
        auto c = processorRef.getConfig();
        c.steps = stepsMap[idx];
        processorRef.setConfig (c);
        processorRef.generateNewPair();
        updateUiFromProcessor();
    };

    updateUiFromProcessor();
    startTimerHz (30); // 30 FPS UI animation & transport update
}

CallResponseAudioProcessorEditor::~CallResponseAudioProcessorEditor() {
    // 1. Stop UI animation timer first
    stopTimer();

    // 2. Dismiss any active popups or context menus immediately
    juce::PopupMenu::dismissAllActiveMenus();

    // 3. Detach all APVTS attachments cleanly while sliders are still valid
    densityAttach.reset();
    gateAttach.reset();
    octaveJumpAttach.reset();
    resolveAttach.reset();
    cutoffAttach.reset();
    resAttach.reset();
    decayAttach.reset();

    // 4. Clear all callbacks from child components so destruction events cannot fire into this
    cbStyle.onChange = nullptr;
    cbStrategy.onChange = nullptr;
    cbKey.onChange = nullptr;
    cbScale.onChange = nullptr;
    cbRootOctave.onChange = nullptr;
    cbMotifType.onChange = nullptr;
    cbEnding.onChange = nullptr;
    cbPhraseMode.onChange = nullptr;
    cbSteps.onChange = nullptr;
    cbMutateTarget.onChange = nullptr;
    cbMutateAmount.onChange = nullptr;

    btnUndo.onClick = nullptr;
    btnRedo.onClick = nullptr;
    btnAudioMute.onClick = nullptr;
    btnPanic.onClick = nullptr;
    btnNewPair.onClick = nullptr;
    btnDice.onClick = nullptr;
    btnRegenTiming.onClick = nullptr;
    btnMutate.onClick = nullptr;
    btnViewStepGrid.onClick = nullptr;
    btnViewPianoRoll.onClick = nullptr;
    btnViewSplit.onClick = nullptr;
    btnSplitCall.onClick = nullptr;
    btnSplitResponse.onClick = nullptr;
    btnSplitBoth.onClick = nullptr;
    for (int i = 0; i < 8; ++i) {
        btnPresetSlots[i].onSave = nullptr;
        btnPresetSlots[i].onLoad = nullptr;
        btnPresetSlots[i].onSelect = nullptr;
    }

    callGrid.onPhraseChanged = nullptr;
    callGrid.onShiftRequested = nullptr;
    callGrid.onOctaveRequested = nullptr;
    callGrid.onInvertRequested = nullptr;
    callGrid.onReverseRequested = nullptr;
    callGrid.onChannelChanged = nullptr;

    responseGrid.onPhraseChanged = nullptr;
    responseGrid.onShiftRequested = nullptr;
    responseGrid.onOctaveRequested = nullptr;
    responseGrid.onInvertRequested = nullptr;
    responseGrid.onReverseRequested = nullptr;
    responseGrid.onChannelChanged = nullptr;

    pianoRollComp.onPhraseEdited = nullptr;
    pianoRollComp.onAuditionNote = nullptr;

    // 5. Safely detach look and feel
    setLookAndFeel (nullptr);
}

void CallResponseAudioProcessorEditor::syncConfigToProcessor() {
    auto c = processorRef.getConfig();
    c.styleIndex = cbStyle.getSelectedId() - 1;
    c.responseStrategyIndex = cbStrategy.getSelectedId() - 1;
    c.keyIndex = cbKey.getSelectedId() - 1;
    c.scaleIndex = cbScale.getSelectedId() - 1;
    c.rootOctaveIndex = cbRootOctave.getSelectedId() - 1;
    c.motifTypeIndex = cbMotifType.getSelectedId() - 1;
    c.endingBehaviourIndex = cbEnding.getSelectedId() - 1;
    c.phraseModeIndex = cbPhraseMode.getSelectedId() - 1;
    c.mutationTargetIndex = cbMutateTarget.getSelectedId() - 1;
    c.mutationAmountIndex = cbMutateAmount.getSelectedId() - 1;
    processorRef.setConfig (c);
}

void CallResponseAudioProcessorEditor::setViewMode (EditorViewMode mode) {
    currentViewMode = mode;
    btnViewStepGrid.setToggleState  (mode == EditorViewMode::StepGrid,  juce::dontSendNotification);
    btnViewPianoRoll.setToggleState (mode == EditorViewMode::PianoRoll, juce::dontSendNotification);
    btnViewSplit.setToggleState     (mode == EditorViewMode::SplitView, juce::dontSendNotification);

    bool isSplit = (mode == EditorViewMode::SplitView);
    btnSplitCall.setVisible (isSplit);
    btnSplitResponse.setVisible (isSplit);
    btnSplitBoth.setVisible (isSplit);

    if (isSplit) {
        setSplitLaneMode (splitLaneMode);
    } else {
        callGrid.setVisible      (mode == EditorViewMode::StepGrid);
        responseGrid.setVisible  (mode == EditorViewMode::StepGrid);
        pianoRollComp.setVisible (mode == EditorViewMode::PianoRoll);
    }

    resized();
    repaint();
}

void CallResponseAudioProcessorEditor::setSplitLaneMode (SplitLaneMode mode) {
    splitLaneMode = mode;
    btnSplitCall.setToggleState     (mode == SplitLaneMode::Call,     juce::dontSendNotification);
    btnSplitResponse.setToggleState (mode == SplitLaneMode::Response, juce::dontSendNotification);
    btnSplitBoth.setToggleState     (mode == SplitLaneMode::Both,     juce::dontSendNotification);

    if (currentViewMode == EditorViewMode::SplitView) {
        if (mode == SplitLaneMode::Call) {
            callGrid.setVisible (true);
            responseGrid.setVisible (false);
            pianoRollComp.setVisible (true);
            pianoRollComp.setViewLane (PianoRollComponent::ViewLane::Call);
        } else if (mode == SplitLaneMode::Response) {
            callGrid.setVisible (false);
            responseGrid.setVisible (true);
            pianoRollComp.setVisible (true);
            pianoRollComp.setViewLane (PianoRollComponent::ViewLane::Response);
        } else { // Both
            callGrid.setVisible (true);
            responseGrid.setVisible (true);
            pianoRollComp.setVisible (true);
            pianoRollComp.setViewLane (PianoRollComponent::ViewLane::Both);
        }
    }
    resized();
    repaint();
}

void CallResponseAudioProcessorEditor::updateUiFromProcessor() {
    auto cfg = processorRef.getConfig();
    cbStyle.setSelectedId (cfg.styleIndex + 1, juce::dontSendNotification);
    cbStrategy.setSelectedId (cfg.responseStrategyIndex + 1, juce::dontSendNotification);
    cbKey.setSelectedId (cfg.keyIndex + 1, juce::dontSendNotification);
    cbScale.setSelectedId (cfg.scaleIndex + 1, juce::dontSendNotification);
    cbRootOctave.setSelectedId (cfg.rootOctaveIndex + 1, juce::dontSendNotification);
    cbMotifType.setSelectedId (cfg.motifTypeIndex + 1, juce::dontSendNotification);
    cbEnding.setSelectedId (cfg.endingBehaviourIndex + 1, juce::dontSendNotification);
    cbPhraseMode.setSelectedId (cfg.phraseModeIndex + 1, juce::dontSendNotification);
    cbMutateTarget.setSelectedId (cfg.mutationTargetIndex + 1, juce::dontSendNotification);
    cbMutateAmount.setSelectedId (cfg.mutationAmountIndex + 1, juce::dontSendNotification);

    lockStyle.setToggleState (processorRef.lockStyle, juce::dontSendNotification);
    lockKeyScale.setToggleState (processorRef.lockKeyScale, juce::dontSendNotification);
    lockMotif.setToggleState (processorRef.lockMotif, juce::dontSendNotification);
    lockStrategy.setToggleState (processorRef.lockStrategy, juce::dontSendNotification);

    auto call = processorRef.getCallPhrase();
    auto resp = processorRef.getResponsePhrase();

    callGrid.setPhrase (call, cfg);
    responseGrid.setPhrase (resp, cfg);
    pianoRollComp.setPhrases (call, resp, cfg);

    callGrid.setChannel (cfg.callChannel);
    responseGrid.setChannel (cfg.responseChannel);

    midiDragComp.setPhrases (call, resp, processorRef.getCurrentBpm());

    btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
    btnRedo.setEnabled (processorRef.getPresetManager().canRedo());

    for (int i = 0; i < 8; ++i) {
        bool occupied = processorRef.getPresetManager().isSlotOccupied(i);
        btnPresetSlots[i].isOccupied = occupied;
        btnPresetSlots[i].repaint();
    }
}

void CallResponseAudioProcessorEditor::timerCallback() {
    int step = processorRef.getCurrentPlayingStep();
    int phase = processorRef.getCurrentPlayingPhase();

    if (step >= 0) {
        if (phase == 0) {
            callGrid.setCurrentStep (step);
            responseGrid.setCurrentStep (-1);
        } else {
            callGrid.setCurrentStep (-1);
            responseGrid.setCurrentStep (step);
        }
    } else {
        callGrid.setCurrentStep (-1);
        responseGrid.setCurrentStep (-1);
    }

    pianoRollComp.setCurrentStep (step, phase);

    if (processorRef.callMidiFlashed.exchange(false))
        callGrid.triggerMidiActivity();
    if (processorRef.respMidiFlashed.exchange(false))
        responseGrid.triggerMidiActivity();

    if (!processorRef.isRegenQueued.load() && btnNewPair.getButtonText() == "QUEUED") {
        btnNewPair.setButtonText ("NEW PAIR");
        updateUiFromProcessor();
    }
}

void CallResponseAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (CustomLookAndFeel::getDarkBg());

    // Top Header bar
    auto headerBounds = getLocalBounds().removeFromTop (52).toFloat();
    juce::ColourGradient headerGrad (juce::Colour(0xff161e2b), 0, 0,
                                     juce::Colour(0xff0f1520), 0, headerBounds.getHeight(), false);
    g.setGradientFill (headerGrad);
    g.fillRect (headerBounds);

    g.setColour (CustomLookAndFeel::getLineBorder());
    g.drawLine (0, headerBounds.getBottom(), (float)getWidth(), headerBounds.getBottom(), 1.0f);

    // Section cards background
    auto area = getLocalBounds().withTrimmedTop (56).withTrimmedBottom (40).reduced (10, 0);

    // Generation & Performance section container
    auto topArea = area.removeFromTop (160).toFloat();
    g.setColour (CustomLookAndFeel::getPanelBg());
    g.fillRoundedRectangle (topArea, 8.0f);
    g.setColour (CustomLookAndFeel::getLineBorder());
    g.drawRoundedRectangle (topArea, 8.0f, 1.0f);
}

void CallResponseAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();

    // 1. Top Header
    auto header = bounds.removeFromTop (52).reduced (10, 6);
    lblTitle.setBounds (header.removeFromLeft (150).reduced (0, 2));
    lblSubtitle.setBounds (header.removeFromLeft (145).reduced (0, 4));

    header.removeFromLeft (6);
    btnViewStepGrid.setBounds  (header.removeFromLeft (52).reduced (2, 4));
    btnViewPianoRoll.setBounds (header.removeFromLeft (74).reduced (2, 4));
    btnViewSplit.setBounds     (header.removeFromLeft (48).reduced (2, 4));

    if (currentViewMode == EditorViewMode::SplitView) {
        header.removeFromLeft (6);
        btnSplitBoth.setBounds     (header.removeFromLeft (44).reduced (2, 4));
        btnSplitCall.setBounds     (header.removeFromLeft (40).reduced (2, 4));
        btnSplitResponse.setBounds (header.removeFromLeft (42).reduced (2, 4));
    }

    btnAudioMute.setBounds (header.removeFromRight (95).reduced (2, 4));
    header.removeFromRight (4);
    btnPanic.setBounds (header.removeFromRight (48).reduced (2, 4));
    header.removeFromRight (6);

    btnRedo.setBounds (header.removeFromRight (40).reduced (2, 4));
    btnUndo.setBounds (header.removeFromRight (40).reduced (2, 4));
    header.removeFromRight (6);

    // Preset slots
    for (int i = 7; i >= 0; --i) {
        btnPresetSlots[i].setBounds (header.removeFromRight (24).reduced (1, 4));
    }

    bounds.removeFromTop (6);

    // 2. Generation Panel & Performance Knobs
    auto genPerfArea = bounds.removeFromTop (156).reduced (10, 0);
    auto perfArea = genPerfArea.removeFromRight (410).reduced (8, 6);
    auto genArea = genPerfArea.reduced (8, 6);

    // Generation Controls Grid
    auto row1 = genArea.removeFromTop (32);
    cbStyle.setBounds (row1.removeFromLeft (190).reduced (2));
    lockStyle.setBounds (row1.removeFromLeft (30).reduced (2));
    cbStrategy.setBounds (row1.removeFromLeft (240).reduced (2));
    lockStrategy.setBounds (row1.removeFromLeft (30).reduced (2));

    genArea.removeFromTop (6);

    auto row2 = genArea.removeFromTop (32);
    cbKey.setBounds (row2.removeFromLeft (64).reduced (2));
    cbScale.setBounds (row2.removeFromLeft (180).reduced (2));
    cbRootOctave.setBounds (row2.removeFromLeft (64).reduced (2));
    lockKeyScale.setBounds (row2.removeFromLeft (30).reduced (2));
    cbSteps.setBounds (row2.removeFromLeft (100).reduced (2));

    genArea.removeFromTop (6);

    auto row3 = genArea.removeFromTop (34);
    btnNewPair.setBounds (row3.removeFromLeft (110).reduced (2));
    btnDice.setBounds (row3.removeFromLeft (70).reduced (2));
    btnRegenTiming.setBounds (row3.removeFromLeft (95).reduced (2));
    cbMotifType.setBounds (row3.removeFromLeft (160).reduced (2));
    lockMotif.setBounds (row3.removeFromLeft (30).reduced (2));

    // Performance Knobs Grid
    int knobW = perfArea.getWidth() / 4;
    auto perfRow1 = perfArea.removeFromTop (74);

    auto kArea1 = perfRow1.removeFromLeft (knobW);
    lblDensity.setBounds (kArea1.removeFromBottom (14));
    sldDensity.setBounds (kArea1);

    auto kArea2 = perfRow1.removeFromLeft (knobW);
    lblGate.setBounds (kArea2.removeFromBottom (14));
    sldGate.setBounds (kArea2);

    auto kArea3 = perfRow1.removeFromLeft (knobW);
    lblOctaveJump.setBounds (kArea3.removeFromBottom (14));
    sldOctaveJump.setBounds (kArea3);

    auto kArea4 = perfRow1;
    lblResolve.setBounds (kArea4.removeFromBottom (14));
    sldResolve.setBounds (kArea4);

    // Synth mini controls in perfRow2
    auto perfRow2 = perfArea;
    int synthKnobW = perfRow2.getWidth() / 3;

    auto sArea1 = perfRow2.removeFromLeft (synthKnobW);
    lblCutoff.setBounds (sArea1.removeFromBottom (14));
    sldCutoff.setBounds (sArea1);

    auto sArea2 = perfRow2.removeFromLeft (synthKnobW);
    lblRes.setBounds (sArea2.removeFromBottom (14));
    sldResonance.setBounds (sArea2);

    auto sArea3 = perfRow2;
    lblDecay.setBounds (sArea3.removeFromBottom (14));
    sldDecay.setBounds (sArea3);

    bounds.removeFromTop (8);

    // 3. Bottom Action Bar & Status Footer
    auto footer = bounds.removeFromBottom (34).reduced (10, 2);
    lblStatus.setBounds (footer);

    auto bottomBar = bounds.removeFromBottom (42).reduced (10, 2);

    // Mutation
    cbMutateTarget.setBounds (bottomBar.removeFromLeft (160).reduced (2));
    cbMutateAmount.setBounds (bottomBar.removeFromLeft (90).reduced (2));
    btnMutate.setBounds (bottomBar.removeFromLeft (80).reduced (2));

    bottomBar.removeFromLeft (16);

    // Drag & Drop dock
    midiDragComp.setBounds (bottomBar.removeFromRight (230).reduced (2));
    toggleWholeBars.setBounds (bottomBar.removeFromRight (100).reduced (2));
    cbExportMode.setBounds (bottomBar.removeFromRight (170).reduced (2));

    bounds.removeFromBottom (6);

    // 4. Lanes / Piano Roll Area
    if (currentViewMode == EditorViewMode::StepGrid) {
        int laneGap = 8;
        int laneHeight = (bounds.getHeight() - laneGap) / 2;
        callGrid.setBounds (bounds.removeFromTop (laneHeight).reduced (10, 0));
        bounds.removeFromTop (laneGap);
        responseGrid.setBounds (bounds.removeFromTop (laneHeight).reduced (10, 0));
    } else if (currentViewMode == EditorViewMode::PianoRoll) {
        pianoRollComp.setBounds (bounds.reduced (10, 0));
    } else { // SplitView
        if (splitLaneMode == SplitLaneMode::Both) {
            int laneHeight = 84;
            callGrid.setBounds (bounds.removeFromTop (laneHeight).reduced (10, 0));
            bounds.removeFromTop (4);
            responseGrid.setBounds (bounds.removeFromTop (laneHeight).reduced (10, 0));
            bounds.removeFromTop (6);
            pianoRollComp.setBounds (bounds.reduced (10, 0));
        } else if (splitLaneMode == SplitLaneMode::Response) {
            int topLaneHeight = std::min(140, bounds.getHeight() * 36 / 100);
            responseGrid.setBounds (bounds.removeFromTop (topLaneHeight).reduced (10, 0));
            bounds.removeFromTop (6);
            pianoRollComp.setBounds (bounds.reduced (10, 0));
        } else { // Call
            int topLaneHeight = std::min(140, bounds.getHeight() * 36 / 100);
            callGrid.setBounds (bounds.removeFromTop (topLaneHeight).reduced (10, 0));
            bounds.removeFromTop (6);
            pianoRollComp.setBounds (bounds.reduced (10, 0));
        }
    }
}

} // namespace CR
