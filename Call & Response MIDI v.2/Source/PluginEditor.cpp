#include "PluginEditor.h"

namespace CR {

CallResponseAudioProcessorEditor::CallResponseAudioProcessorEditor (CallResponseAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      callGrid ("CALL", CustomLookAndFeel::getNeonGreen()),
      responseGrid ("RESPONSE", CustomLookAndFeel::getNeonViolet()),
      inspectorPanel (p)
{
    setLookAndFeel (&customLookAndFeel);

    // Make editor modern and fixed aspect ratio resizable
    setResizable (true, true);
    setResizeLimits (944, 600, 1888, 1200);
    if (auto* c = getConstrainer()) {
        c->setFixedAspectRatio (1180.0 / 750.0);
    }
    setSize (1180, 750);

    // --------------------------------------------------------------------------
    // 1. Top Ribbon Controls (matching Call & Response_UI_example_06.png)
    // --------------------------------------------------------------------------
    lblLogo.setText ("CALL & RESPONSE v.2", juce::dontSendNotification);
    lblLogo.setFont (juce::FontOptions (14.0f, juce::Font::bold));
    lblLogo.setColour (juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible (lblLogo);

    stepOctave.setItems ({"-2", "-1", "0", "1", "2", "3"});
    stepOctave.setSelectedIndex (processorRef.getConfig().rootOctaveIndex + 1, juce::dontSendNotification);
    addAndMakeVisible (stepOctave);

    stepKey.setItems (GeneratorEngine::getKeys());
    stepKey.setSelectedIndex (processorRef.getConfig().keyIndex, juce::dontSendNotification);
    addAndMakeVisible (stepKey);

    stepScale.setItems (GeneratorEngine::getScales());
    stepScale.setSelectedIndex (processorRef.getConfig().scaleIndex, juce::dontSendNotification);
    addAndMakeVisible (stepScale);

    btnEnharmonic.setClickingTogglesState (true);
    btnEnharmonic.setTooltip ("Toggle enharmonic flats (b) / sharps (#)");
    btnEnharmonic.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff252328));
    btnEnharmonic.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getMutedText());
    btnEnharmonic.setColour (juce::TextButton::textColourOnId, CustomLookAndFeel::getActiveText());
    addAndMakeVisible (btnEnharmonic);

    stepSteps.setItems ({"4", "8", "12", "16"});
    stepSteps.setSelectedIndex (processorRef.getConfig().steps == 16 ? 3 : (processorRef.getConfig().steps == 12 ? 2 : (processorRef.getConfig().steps == 8 ? 1 : 0)), juce::dontSendNotification);
    addAndMakeVisible (stepSteps);

    stepRate.setItems ({"Half speed", "0", "Double speed"});
    stepRate.setSelectedIndex (processorRef.getPlaybackRateIndex(), juce::dontSendNotification);
    addAndMakeVisible (stepRate);

    stepSwing.setContinuousRange (0.0, 100.0, (double)(processorRef.getSwingPercent() * 100.0f), 1.0, "%");
    stepSwing.setContinuousPresets ({
        { "0%  (Straight)", 0.0 },
        { "15% (Light)", 15.0 },
        { "25% (Medium)", 25.0 },
        { "33% (Groove)", 33.0 },
        { "50% (Triplets)", 50.0 },
        { "66% (Heavy)", 66.0 },
        { "75% (Dotted)", 75.0 },
        { "100% (Max)", 100.0 }
    });
    addAndMakeVisible (stepSwing);

    // 8 Preset Slot Buttons: [ A ] through [ H ]
    for (int i = 0; i < 8; ++i) {
        char slotChar = (char)('A' + i);
        presetSlotButtons[(size_t)i].setButtonText (juce::String::charToString (slotChar));
        presetSlotButtons[(size_t)i].slotIndex = i;
        presetSlotButtons[(size_t)i].setTooltip ("Preset Slot " + juce::String::charToString (slotChar) + " (Click to load, Shift+Click to save, Right-click for options)");
        presetSlotButtons[(size_t)i].onSelect = [this](int slot) {
            processorRef.recallPresetSlot (slot);
            updateUiFromProcessor();
            char sc = (char)('A' + slot);
            lblStatus.setText ("Loaded Preset Slot " + juce::String::charToString (sc), juce::dontSendNotification);
        };
        presetSlotButtons[(size_t)i].onLoad = [this](int slot) {
            processorRef.recallPresetSlot (slot);
            updateUiFromProcessor();
            char sc = (char)('A' + slot);
            lblStatus.setText ("Loaded Preset Slot " + juce::String::charToString (sc), juce::dontSendNotification);
        };
        presetSlotButtons[(size_t)i].onSave = [this](int slot) {
            processorRef.savePresetSlot (slot);
            updateUiFromProcessor();
            char sc = (char)('A' + slot);
            lblStatus.setText ("Saved State to Preset Slot " + juce::String::charToString (sc), juce::dontSendNotification);
        };
        addAndMakeVisible (presetSlotButtons[(size_t)i]);
    }

    // Top Undo & Redo buttons
    auto setupTopPill = [](juce::TextButton& btn, const juce::String& tip) {
        btn.setTooltip (tip);
        btn.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff252328));
        btn.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getActiveText());
    };
    setupTopPill (btnUndo, "Undo last edit (Cmd+Z)");
    setupTopPill (btnRedo, "Redo last undone edit (Cmd+Shift+Z)");
    addAndMakeVisible (btnUndo);
    addAndMakeVisible (btnRedo);
    btnUndo.onClick = [this] {
        processorRef.undo();
        updateUiFromProcessor();
        lblStatus.setText ("Undo applied", juce::dontSendNotification);
    };
    btnRedo.onClick = [this] {
        processorRef.redo();
        updateUiFromProcessor();
        lblStatus.setText ("Redo applied", juce::dontSendNotification);
    };

    // PANIC box
    btnPanic.setColour (juce::TextButton::buttonColourId, CustomLookAndFeel::getPanicViolet());
    btnPanic.setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    btnPanic.setTooltip ("Panic: Send All Notes Off across all 16 MIDI channels");
    addAndMakeVisible (btnPanic);
    btnPanic.onClick = [this] {
        processorRef.triggerPanic();
        lblStatus.setText ("Panic: All notes off sent on all 16 MIDI channels", juce::dontSendNotification);
    };

    // PREVIEW Audio Toggle Button (matching 06.png cyan style)
    btnAudioMute.setClickingTogglesState (true);
    btnAudioMute.setToggleState (!processorRef.muteAudio, juce::dontSendNotification);
    btnAudioMute.setButtonText (processorRef.muteAudio ? "PREVIEW: OFF" : "PREVIEW: ON");
    btnAudioMute.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff12232c));
    btnAudioMute.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff143547));
    btnAudioMute.setColour (juce::TextButton::textColourOffId, juce::Colour (0xff64748b));
    btnAudioMute.setColour (juce::TextButton::textColourOnId, CustomLookAndFeel::getNeonCyan());
    btnAudioMute.setTooltip ("Toggle internal preview synth audio ON/OFF");
    addAndMakeVisible (btnAudioMute);
    btnAudioMute.onClick = [this] {
        processorRef.muteAudio = !btnAudioMute.getToggleState();
        btnAudioMute.setButtonText (processorRef.muteAudio ? "PREVIEW: OFF" : "PREVIEW: ON");
        lblStatus.setText (processorRef.muteAudio ? "Preview audio muted" : "Preview audio enabled", juce::dontSendNotification);
    };

    // Ribbon callbacks
    auto onRibbonConfigChanged = [this] {
        auto c = processorRef.getConfig();
        c.rootOctaveIndex = juce::jlimit (0, 4, stepOctave.getSelectedIndex() - 1);
        c.keyIndex = stepKey.getSelectedIndex();
        c.scaleIndex = stepScale.getSelectedIndex();
        int stepsMap[] = { 4, 8, 12, 16 };
        c.steps = stepsMap[juce::jlimit (0, 3, stepSteps.getSelectedIndex())];
        processorRef.setConfig (c);
        updateUiFromProcessor();
        lblStatus.setText ("Key & Scale set to " + GeneratorEngine::getKeys()[(size_t)c.keyIndex] + " " + GeneratorEngine::getScales()[(size_t)c.scaleIndex], juce::dontSendNotification);
    };

    stepOctave.onChange = [onRibbonConfigChanged](int) { onRibbonConfigChanged(); };
    stepKey.onChange    = [onRibbonConfigChanged](int) { onRibbonConfigChanged(); };
    stepScale.onChange  = [onRibbonConfigChanged](int) { onRibbonConfigChanged(); };
    stepSteps.onChange  = [this](int) {
        int stepsMap[] = { 4, 8, 12, 16 };
        auto c = processorRef.getConfig();
        c.steps = stepsMap[juce::jlimit (0, 3, stepSteps.getSelectedIndex())];
        processorRef.setConfig (c);
        processorRef.generateNewPair();
        updateUiFromProcessor();
    };

    stepRate.onChange = [this](int idx) {
        processorRef.setPlaybackRateIndex (idx);
        juce::String names[] = { "Half speed", "Normal (0)", "Double speed" };
        lblStatus.setText ("Playback rate: " + names[juce::jlimit (0, 2, idx)], juce::dontSendNotification);
    };

    stepSwing.onValueChanged = [this](double val) {
        float pct = (float)juce::jlimit (0.0, 100.0, val) / 100.0f;
        processorRef.setSwingPercent (pct);
        lblStatus.setText ("Swing set to " + juce::String ((int)std::round (val)) + "%", juce::dontSendNotification);
    };

    btnEnharmonic.onClick = [this] {
        enharmonicFlats = btnEnharmonic.getToggleState();
        if (enharmonicFlats) {
            stepKey.setItems ({"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"});
        } else {
            stepKey.setItems (GeneratorEngine::getKeys());
        }
        stepKey.setSelectedIndex (processorRef.getConfig().keyIndex, juce::dontSendNotification);
    };

    // --------------------------------------------------------------------------
    // 2. Upper Dashboard - Left Column (Quick Controls & Knobs)
    // --------------------------------------------------------------------------
    btnNewPair.setColour (juce::TextButton::buttonColourId, CustomLookAndFeel::getNeonGreen());
    btnNewPair.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    btnNewPair.setTooltip ("Generate a new connected Call & Response pair");
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

    // Swapped AUTO with DICE button (v.1 behavior)
    btnDice.setColour (juce::TextButton::buttonColourId, CustomLookAndFeel::getCardBg());
    btnDice.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getActiveText());
    btnDice.setTooltip ("Reroll unlocked generation settings (Dice)");
    addAndMakeVisible (btnDice);
    btnDice.onClick = [this] {
        processorRef.rerollUnlocked();
        updateUiFromProcessor();
        lblStatus.setText ("Rerolled unlocked parameters", juce::dontSendNotification);
    };

    auto setupKnob = [this](juce::Slider& sld, juce::Label& lbl, const juce::String& name, juce::Colour col) {
        sld.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        sld.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        sld.setColour (juce::Slider::rotarySliderFillColourId, col);
        addAndMakeVisible (sld);

        lbl.setText (name + "\n50%", juce::dontSendNotification);
        lbl.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        lbl.setJustificationType (juce::Justification::centred);
        lbl.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText());
        addAndMakeVisible (lbl);
    };

    setupKnob (sldDensity,    lblDensityVal, "DENSITY\n", CustomLookAndFeel::getNeonEmerald());
    setupKnob (sldMutate,     lblMutateVal,  "MUTATE\n",  CustomLookAndFeel::getNeonViolet());
    setupKnob (sldOctaveJump, lblOctJumpVal, "OCT JUMP\n", CustomLookAndFeel::getNeonOrange());

    sldMutate.setRange (0.0, 100.0, 1.0);
    sldMutate.setValue (0.0, juce::dontSendNotification);

    densityAttach    = std::make_unique<SliderAttachment> (processorRef.apvts, "density", sldDensity);
    octaveJumpAttach = std::make_unique<SliderAttachment> (processorRef.apvts, "octaveJump", sldOctaveJump);

    sldDensity.onValueChange = [this] {
        int pct = juce::roundToInt (sldDensity.getValue() * 100.0);
        lblDensityVal.setText ("DENSITY\n" + juce::String (pct) + "%", juce::dontSendNotification);
    };
    sldMutate.onValueChange = [this] {
        int pct = juce::roundToInt (sldMutate.getValue());
        lblMutateVal.setText ("MUTATE\n" + juce::String (pct) + "%", juce::dontSendNotification);
    };
    sldOctaveJump.onValueChange = [this] {
        int pct = juce::roundToInt (sldOctaveJump.getValue() * 100.0);
        lblOctJumpVal.setText ("OCT JUMP\n" + juce::String (pct) + "%", juce::dontSendNotification);
    };

    sldDensity.onValueChange();
    sldMutate.onValueChange();
    sldOctaveJump.onValueChange();

    // Rectangular shift buttons under knobs (< OFFSET >)
    auto setupShiftBtn = [](juce::TextButton& btn, const juce::String& tip) {
        btn.setTooltip (tip);
        btn.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff252328));
        btn.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getActiveText());
    };
    setupShiftBtn (btnShiftPhraseLeft, "Offset all Call & Response notes left (-1 step)");
    setupShiftBtn (btnShiftPhraseRight, "Offset all Call & Response notes right (+1 step)");
    addAndMakeVisible (btnShiftPhraseLeft);
    addAndMakeVisible (btnShiftPhraseRight);

    lblOffset.setText ("OFFSET", juce::dontSendNotification);
    lblOffset.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    lblOffset.setJustificationType (juce::Justification::centred);
    lblOffset.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText());
    addAndMakeVisible (lblOffset);

    btnShiftPhraseLeft.onClick = [this] {
        processorRef.pushUndo();
        auto call = processorRef.getCallPhrase();
        auto resp = processorRef.getResponsePhrase();
        GeneratorEngine::shift (call, -1);
        GeneratorEngine::shift (resp, -1);
        processorRef.setCallPhrase (call);
        processorRef.setResponsePhrase (resp);
        updateUiFromProcessor();
        lblStatus.setText ("Shifted Call & Response notes left (-1 step)", juce::dontSendNotification);
    };

    btnShiftPhraseRight.onClick = [this] {
        processorRef.pushUndo();
        auto call = processorRef.getCallPhrase();
        auto resp = processorRef.getResponsePhrase();
        GeneratorEngine::shift (call, 1);
        GeneratorEngine::shift (resp, 1);
        processorRef.setCallPhrase (call);
        processorRef.setResponsePhrase (resp);
        updateUiFromProcessor();
        lblStatus.setText ("Shifted Call & Response notes right (+1 step)", juce::dontSendNotification);
    };

    lblCredits.setText (juce::String (juce::CharPointer_UTF8 ("Viral Samples \xc2\xb7 Call & Response MIDI v.2")), juce::dontSendNotification);
    lblCredits.setFont (juce::FontOptions (9.5f));
    lblCredits.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText().withAlpha (0.7f));
    lblCredits.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (lblCredits);

    // --------------------------------------------------------------------------
    // 3. Upper Dashboard - Center Column (Dual Step Lanes)
    // --------------------------------------------------------------------------
    addAndMakeVisible (callGrid);
    addAndMakeVisible (responseGrid);

    callGrid.onEditGestureStarted = [this] {
        processorRef.pushUndo();
    };
    callGrid.onPhraseChanged = [this] {
        processorRef.setCallPhrase (callGrid.getPhrase());
        auto currentConfig = processorRef.getConfig();
        pianoRollComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), currentConfig);
        midiDragComp.setPhrases (processorRef.getCallPhrase(), processorRef.getResponsePhrase(), processorRef.getCurrentBpm());
        btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
        btnRedo.setEnabled (processorRef.getPresetManager().canRedo());
    };
    callGrid.onShiftRequested = [this](int delta) {
        processorRef.pushUndo();
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::shift (phrase, delta);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onOctaveRequested = [this](int oct) {
        processorRef.pushUndo();
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::transposeOctave (phrase, oct);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onInvertRequested = [this] {
        processorRef.pushUndo();
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::invert (phrase, processorRef.getConfig());
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onReverseRequested = [this] {
        processorRef.pushUndo();
        auto phrase = processorRef.getCallPhrase();
        GeneratorEngine::reverse (phrase);
        processorRef.setCallPhrase (phrase);
        updateUiFromProcessor();
    };
    callGrid.onChannelChanged = [this](int ch) {
        processorRef.setCallChannel (ch);
        lblStatus.setText ("Call MIDI Output routed to Channel " + juce::String (ch), juce::dontSendNotification);
    };
    callGrid.onAuditionNote = [this](int note, bool on) {
        processorRef.triggerAuditionNote (note, on);
    };
    callGrid.onMuteToggled = [this](bool isMuted) {
        processorRef.setCallMuted (isMuted);
        lblStatus.setText (isMuted ? "Call lane muted / bypassed" : "Call lane active", juce::dontSendNotification);
    };
    callGrid.onLockToggled = [this](bool isLocked) {
        processorRef.setCallLocked (isLocked);
        lblStatus.setText (isLocked ? "Call phrase locked" : "Call phrase unlocked", juce::dontSendNotification);
    };
    callGrid.onDiceRequested = [this] {
        processorRef.mutateSelected ("Call only", callGrid.getMutatePercent());
        updateUiFromProcessor();
        lblStatus.setText ("Mutated Call phrase (" + juce::String (callGrid.getMutatePercent()) + "%)", juce::dontSendNotification);
    };

    responseGrid.onEditGestureStarted = [this] {
        processorRef.pushUndo();
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
        processorRef.pushUndo();
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::shift (phrase, delta);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onOctaveRequested = [this](int oct) {
        processorRef.pushUndo();
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::transposeOctave (phrase, oct);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onInvertRequested = [this] {
        processorRef.pushUndo();
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::invert (phrase, processorRef.getConfig());
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onReverseRequested = [this] {
        processorRef.pushUndo();
        auto phrase = processorRef.getResponsePhrase();
        GeneratorEngine::reverse (phrase);
        processorRef.setResponsePhrase (phrase);
        updateUiFromProcessor();
    };
    responseGrid.onChannelChanged = [this](int ch) {
        processorRef.setResponseChannel (ch);
        lblStatus.setText ("Response MIDI Output routed to Channel " + juce::String (ch), juce::dontSendNotification);
    };
    responseGrid.onAuditionNote = [this](int note, bool on) {
        processorRef.triggerAuditionNote (note, on);
    };
    responseGrid.onMuteToggled = [this](bool isMuted) {
        processorRef.setResponseMuted (isMuted);
        lblStatus.setText (isMuted ? "Response lane muted / bypassed" : "Response lane active", juce::dontSendNotification);
    };
    responseGrid.onLockToggled = [this](bool isLocked) {
        processorRef.setResponseLocked (isLocked);
        lblStatus.setText (isLocked ? "Response phrase locked" : "Response phrase unlocked", juce::dontSendNotification);
    };
    responseGrid.onDiceRequested = [this] {
        processorRef.mutateSelected ("Response only", responseGrid.getMutatePercent());
        updateUiFromProcessor();
        lblStatus.setText ("Mutated Response phrase (" + juce::String (responseGrid.getMutatePercent()) + "%)", juce::dontSendNotification);
    };

    // --------------------------------------------------------------------------
    // 4. Upper Dashboard - Right Column (Tabbed Inspector Panel)
    // --------------------------------------------------------------------------
    addAndMakeVisible (inspectorPanel);
    inspectorPanel.onConfigChanged = [this] {
        updateUiFromProcessor();
    };
    inspectorPanel.onClipModeChanged = [this](int mode) {
        cbExportMode.setSelectedId (mode + 1, juce::sendNotification);
    };
    inspectorPanel.onLoopLengthChanged = [this](bool wholeBars) {
        toggleWholeBars.setToggleState (wholeBars, juce::sendNotification);
    };

    // --------------------------------------------------------------------------
    // 5. Middle Section (Piano Roll Melody Editor)
    // --------------------------------------------------------------------------
    addAndMakeVisible (pianoRollComp);
    pianoRollComp.onEditGestureStarted = [this] {
        processorRef.pushUndo();
    };
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
        lblStatus.setText ("Melody edited in Piano Roll", juce::dontSendNotification);
    };
    pianoRollComp.onAuditionNote = [this](int midiNote, bool isNoteOn) {
        processorRef.triggerAuditionNote (midiNote, isNoteOn);
    };

    // --------------------------------------------------------------------------
    // 6. Bottom Bar (Mutation & MIDI Drag & Drop)
    // --------------------------------------------------------------------------
    auto cfg = processorRef.getConfig();

    auto setupBox = [this](juce::ComboBox& box, const std::vector<juce::String>& items, int selectedId) {
        box.clear();
        for (size_t i = 0; i < items.size(); ++i) {
            box.addItem (items[i], (int)i + 1);
        }
        box.setSelectedId (selectedId + 1, juce::dontSendNotification);
        addAndMakeVisible (box);
    };

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

    lblStatus.setText ("Generated new Call & Response pair", juce::dontSendNotification);
    lblStatus.setFont (juce::FontOptions (11.0f));
    lblStatus.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText());
    addAndMakeVisible (lblStatus);

    updateUiFromProcessor();
    startTimerHz (30);
}

CallResponseAudioProcessorEditor::~CallResponseAudioProcessorEditor() {
    stopTimer();
    juce::PopupMenu::dismissAllActiveMenus();

    densityAttach.reset();
    octaveJumpAttach.reset();
    resolveAttach.reset();
    gateAttach.reset();
    cutoffAttach.reset();
    resAttach.reset();
    decayAttach.reset();

    stepOctave.onChange = nullptr;
    stepKey.onChange = nullptr;
    stepScale.onChange = nullptr;
    stepSteps.onChange = nullptr;
    stepRate.onChange = nullptr;
    stepSwing.onChange = nullptr;
    stepSwing.onValueChanged = nullptr;
    btnEnharmonic.onClick = nullptr;
    btnDice.onClick = nullptr;
    btnPanic.onClick = nullptr;
    btnAudioMute.onClick = nullptr;
    btnNewPair.onClick = nullptr;
    btnUndo.onClick = nullptr;
    btnRedo.onClick = nullptr;
    btnMutate.onClick = nullptr;

    for (auto& b : presetSlotButtons) {
        b.onSelect = nullptr;
        b.onLoad = nullptr;
        b.onSave = nullptr;
    }
    btnShiftPhraseLeft.onClick = nullptr;
    btnShiftPhraseRight.onClick = nullptr;

    callGrid.onEditGestureStarted = nullptr;
    callGrid.onPhraseChanged = nullptr;
    callGrid.onShiftRequested = nullptr;
    callGrid.onOctaveRequested = nullptr;
    callGrid.onInvertRequested = nullptr;
    callGrid.onReverseRequested = nullptr;
    callGrid.onChannelChanged = nullptr;
    callGrid.onAuditionNote = nullptr;
    callGrid.onMuteToggled = nullptr;
    callGrid.onLockToggled = nullptr;
    callGrid.onDiceRequested = nullptr;
    callGrid.onMutatePercentChanged = nullptr;

    responseGrid.onEditGestureStarted = nullptr;
    responseGrid.onPhraseChanged = nullptr;
    responseGrid.onShiftRequested = nullptr;
    responseGrid.onOctaveRequested = nullptr;
    responseGrid.onInvertRequested = nullptr;
    responseGrid.onReverseRequested = nullptr;
    responseGrid.onChannelChanged = nullptr;
    responseGrid.onAuditionNote = nullptr;
    responseGrid.onMuteToggled = nullptr;
    responseGrid.onLockToggled = nullptr;
    responseGrid.onDiceRequested = nullptr;
    responseGrid.onMutatePercentChanged = nullptr;

    pianoRollComp.onEditGestureStarted = nullptr;
    pianoRollComp.onPhraseEdited = nullptr;
    pianoRollComp.onAuditionNote = nullptr;
    inspectorPanel.onConfigChanged = nullptr;
    inspectorPanel.onClipModeChanged = nullptr;
    inspectorPanel.onLoopLengthChanged = nullptr;

    setLookAndFeel (nullptr);
}

void CallResponseAudioProcessorEditor::syncConfigToProcessor() {
    auto c = processorRef.getConfig();
    c.mutationTargetIndex = cbMutateTarget.getSelectedId() - 1;
    c.mutationAmountIndex = cbMutateAmount.getSelectedId() - 1;
    processorRef.setConfig (c);
}

void CallResponseAudioProcessorEditor::updateUiFromProcessor() {
    auto cfg = processorRef.getConfig();

    stepOctave.setSelectedIndex (cfg.rootOctaveIndex + 1, juce::dontSendNotification);
    stepKey.setSelectedIndex    (cfg.keyIndex, juce::dontSendNotification);
    stepScale.setSelectedIndex  (cfg.scaleIndex, juce::dontSendNotification);
    stepSteps.setSelectedIndex  (cfg.steps == 16 ? 3 : (cfg.steps == 12 ? 2 : (cfg.steps == 8 ? 1 : 0)), juce::dontSendNotification);
    stepRate.setSelectedIndex   (processorRef.getPlaybackRateIndex(), juce::dontSendNotification);
    if (std::abs (stepSwing.getCurrentValue() - (double)(processorRef.getSwingPercent() * 100.0f)) > 0.5)
        stepSwing.setCurrentValue ((double)(processorRef.getSwingPercent() * 100.0f), juce::dontSendNotification);

    cbMutateTarget.setSelectedId (cfg.mutationTargetIndex + 1, juce::dontSendNotification);
    cbMutateAmount.setSelectedId (cfg.mutationAmountIndex + 1, juce::dontSendNotification);

    auto call = processorRef.getCallPhrase();
    auto resp = processorRef.getResponsePhrase();

    callGrid.setPhrase (call, cfg);
    responseGrid.setPhrase (resp, cfg);
    pianoRollComp.setPhrases (call, resp, cfg);

    callGrid.setChannel (cfg.callChannel);
    responseGrid.setChannel (cfg.responseChannel);
    callGrid.setMuted (processorRef.muteCall);
    responseGrid.setMuted (processorRef.muteResponse);
    callGrid.setLocked (processorRef.isCallLocked());
    responseGrid.setLocked (processorRef.isResponseLocked());

    midiDragComp.setPhrases (call, resp, processorRef.getCurrentBpm());

    inspectorPanel.updateFromProcessor();

    for (int i = 0; i < 8; ++i) {
        presetSlotButtons[(size_t)i].isOccupied = processorRef.getPresetManager().isSlotOccupied (i);
        presetSlotButtons[(size_t)i].setToggleState (processorRef.getPresetManager().getCurrentSlot() == i, juce::dontSendNotification);
        presetSlotButtons[(size_t)i].repaint();
    }
    btnAudioMute.setToggleState (!processorRef.muteAudio, juce::dontSendNotification);
    btnAudioMute.setButtonText (processorRef.muteAudio ? "PREVIEW: OFF" : "PREVIEW: ON");

    btnUndo.setEnabled (processorRef.getPresetManager().canUndo());
    btnRedo.setEnabled (processorRef.getPresetManager().canRedo());
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

    if (processorRef.callMidiFlashed.exchange (false))
        callGrid.triggerMidiActivity();
    if (processorRef.respMidiFlashed.exchange (false))
        responseGrid.triggerMidiActivity();

    if (!processorRef.isRegenQueued.load() && btnNewPair.getButtonText() == "QUEUED") {
        btnNewPair.setButtonText ("NEW PAIR");
        updateUiFromProcessor();
    }
}

void CallResponseAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll (CustomLookAndFeel::getDarkBg());

    // 1. Top Ribbon Background
    auto headerBounds = getLocalBounds().removeFromTop (40).toFloat();
    g.setColour (CustomLookAndFeel::getTopBarBg());
    g.fillRect (headerBounds);

    g.setColour (CustomLookAndFeel::getLineBorder().withAlpha (0.4f));
    g.drawLine (0, headerBounds.getBottom(), (float)getWidth(), headerBounds.getBottom(), 1.0f);

    // 2. Dividers between columns in upper dashboard
    auto upperBounds = getLocalBounds().withTrimmedTop (44).withTrimmedBottom (getHeight() - 250);
    g.setColour (CustomLookAndFeel::getLineBorder().withAlpha (0.3f));
    g.drawRoundedRectangle (upperBounds.toFloat().reduced (8.0f, 0.0f), 6.0f, 1.0f);
}

void CallResponseAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();

    // --------------------------------------------------------------------------
    // 1. Top Ribbon (40px height) matching Call & Response_UI_example_06.png
    // --------------------------------------------------------------------------
    auto ribbon = bounds.removeFromTop (40).reduced (8, 4);

    lblLogo.setBounds (ribbon.removeFromLeft (145).reduced (2, 2));

    // Stepper boxes along ribbon (each displays value and is changeable by click dropdown or arrows)
    stepOctave.setBounds    (ribbon.removeFromLeft (64).reduced (2, 1));
    stepKey.setBounds       (ribbon.removeFromLeft (62).reduced (2, 1));
    stepScale.setBounds     (ribbon.removeFromLeft (160).reduced (2, 1));
    btnEnharmonic.setBounds (ribbon.removeFromLeft (26).reduced (1, 3));
    stepSteps.setBounds     (ribbon.removeFromLeft (74).reduced (2, 1));
    stepRate.setBounds      (ribbon.removeFromLeft (106).reduced (2, 1));
    stepSwing.setBounds     (ribbon.removeFromLeft (84).reduced (2, 1));

    // Right side of ribbon: Audio Preview, PANIC, Redo, Undo, 8 Preset slots (matching Call & Response_UI_example_06.png)
    btnAudioMute.setBounds  (ribbon.removeFromRight (92).reduced (2, 2));
    ribbon.removeFromRight (4);
    btnPanic.setBounds      (ribbon.removeFromRight (56).reduced (2, 2));
    ribbon.removeFromRight (4);
    btnRedo.setBounds       (ribbon.removeFromRight (40).reduced (2, 2));
    btnUndo.setBounds       (ribbon.removeFromRight (40).reduced (2, 2));
    ribbon.removeFromRight (6);

    auto presetGroup = ribbon.removeFromRight (8 * 20);
    for (int i = 0; i < 8; ++i) {
        presetSlotButtons[(size_t)i].setBounds (presetGroup.removeFromLeft (18).reduced (1, 2));
        presetGroup.removeFromLeft (2);
    }

    bounds.removeFromTop (4);

    // --------------------------------------------------------------------------
    // 2. Upper Dashboard (3 Columns, ~202px height)
    // --------------------------------------------------------------------------
    int upperH = 202;
    auto upperArea = bounds.removeFromTop (upperH).reduced (8, 0);

    // Left Column: Quick Controls & Knobs (~175px)
    auto leftCol = upperArea.removeFromLeft (175).reduced (4, 4);
    auto topPairRow = leftCol.removeFromTop (28);
    btnNewPair.setBounds (topPairRow.removeFromLeft (106).reduced (1, 1));
    btnDice.setBounds    (topPairRow.reduced (1, 1)); // Swapped AUTO with DICE

    leftCol.removeFromTop (4);

    // 3 Knobs row
    auto knobRow = leftCol.removeFromTop (78);
    int kw = knobRow.getWidth() / 3;

    auto k1 = knobRow.removeFromLeft (kw);
    lblDensityVal.setBounds (k1.removeFromBottom (26));
    sldDensity.setBounds (k1.reduced (2));

    auto k2 = knobRow.removeFromLeft (kw);
    lblMutateVal.setBounds (k2.removeFromBottom (26));
    sldMutate.setBounds (k2.reduced (2));

    auto k3 = knobRow;
    lblOctJumpVal.setBounds (k3.removeFromBottom (26));
    sldOctaveJump.setBounds (k3.reduced (2));

    leftCol.removeFromTop (4);

    // Rectangular shift buttons under knobs (< OFFSET >)
    auto shiftRow = leftCol.removeFromTop (24);
    btnShiftPhraseLeft.setBounds (shiftRow.removeFromLeft (36).reduced (1, 1));
    btnShiftPhraseRight.setBounds (shiftRow.removeFromRight (36).reduced (1, 1));
    lblOffset.setBounds (shiftRow.reduced (2, 1));

    lblCredits.setBounds (leftCol.removeFromBottom (16));

    upperArea.removeFromLeft (6);

    // Right Column: Tabbed Inspector Panel (~320px)
    auto rightCol = upperArea.removeFromRight (320).reduced (2, 2);
    inspectorPanel.setBounds (rightCol);

    upperArea.removeFromRight (6);

    // Center Column: Dual Step Lanes
    auto centerCol = upperArea.reduced (2, 2);
    int laneH = (centerCol.getHeight() - 6) / 2;
    callGrid.setBounds (centerCol.removeFromTop (laneH));
    centerCol.removeFromTop (6);
    responseGrid.setBounds (centerCol);

    bounds.removeFromTop (4);

    // --------------------------------------------------------------------------
    // 3. Bottom Bar & Status (~40px)
    // --------------------------------------------------------------------------
    auto footer = bounds.removeFromBottom (16).reduced (10, 0);
    lblStatus.setBounds (footer);

    auto bottomBar = bounds.removeFromBottom (34).reduced (8, 2);

    // Mutation
    cbMutateTarget.setBounds (bottomBar.removeFromLeft (160).reduced (2, 1));
    cbMutateAmount.setBounds (bottomBar.removeFromLeft (90).reduced (2, 1));
    btnMutate.setBounds (bottomBar.removeFromLeft (76).reduced (2, 1));

    // Right Drag & Drop dock
    midiDragComp.setBounds (bottomBar.removeFromRight (220).reduced (2, 1));
    toggleWholeBars.setBounds (bottomBar.removeFromRight (95).reduced (2, 1));
    cbExportMode.setBounds (bottomBar.removeFromRight (160).reduced (2, 1));

    bounds.removeFromBottom (4);

    // --------------------------------------------------------------------------
    // 4. Middle Section: Piano Roll Melody Editor (fills remaining space)
    // --------------------------------------------------------------------------
    pianoRollComp.setBounds (bounds.reduced (8, 2));
}

} // namespace CR
