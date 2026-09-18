#include "InspectorPanel.h"
#include "CustomLookAndFeel.h"

namespace CR {

InspectorPanel::InspectorPanel (CallResponseAudioProcessor& processor)
    : processorRef (processor) {

    // Tab buttons setup
    auto setupTabBtn = [this](juce::TextButton& btn, MainTab tab) {
        btn.setClickingTogglesState (true);
        btn.setRadioGroupId (1001);
        addAndMakeVisible (btn);
        btn.onClick = [this, tab] { setTab (tab); };
    };

    setupTabBtn (btnTabGenerate,     MainTab::Generate);
    setupTabBtn (btnTabShape,        MainTab::Shape);
    setupTabBtn (btnTabArticulation, MainTab::Articulation);
    setupTabBtn (btnTabOptions,      MainTab::Options);
    btnTabGenerate.setToggleState (true, juce::dontSendNotification);

    // Subtabs for Options setup
    auto setupSubTabBtn = [this](juce::TextButton& btn, OptionsSubTab subTab) {
        btn.setClickingTogglesState (true);
        btn.setRadioGroupId (1002);
        addChildComponent (btn);
        btn.onClick = [this, subTab] { setOptionsSubTab (subTab); };
    };

    setupSubTabBtn (btnSubRouting,     OptionsSubTab::Routing);
    setupSubTabBtn (btnSubTimingClips, OptionsSubTab::TimingClips);
    btnSubRouting.setToggleState (true, juce::dontSendNotification);

    // Legend
    lblLegend.setText ("A accent   S slide   T tie", juce::dontSendNotification);
    lblLegend.setFont (juce::FontOptions (10.0f));
    lblLegend.setJustificationType (juce::Justification::centredRight);
    lblLegend.setColour (juce::Label::textColourId, CustomLookAndFeel::getMutedText().withAlpha (0.7f));
    addAndMakeVisible (lblLegend);

    setupSteppers();
    setTab (MainTab::Generate);
}

InspectorPanel::~InspectorPanel() {
    btnTabGenerate.onClick = nullptr;
    btnTabShape.onClick = nullptr;
    btnTabArticulation.onClick = nullptr;
    btnTabOptions.onClick = nullptr;
    btnSubRouting.onClick = nullptr;
    btnSubTimingClips.onClick = nullptr;
    onConfigChanged = nullptr;
}

void InspectorPanel::setupSteppers() {
    auto setupStepp = [this](StepperComponent& s, const std::vector<juce::String>& items, bool lockable, std::function<void(int)> onParamChange) {
        s.setItems (items);
        s.setLockable (lockable);
        addChildComponent (s);
        s.onChange = [this, onParamChange](int idx) {
            if (isInternalUpdating) return;
            if (onParamChange) onParamChange (idx);
            if (onConfigChanged) onConfigChanged();
        };
    };

    auto updateCfg = [this](std::function<void(GeneratorConfig&)> apply) {
        auto cfg = processorRef.getConfig();
        apply (cfg);
        processorRef.setConfig (cfg);
    };

    // GENERATE
    setupStepp (stepStyle,  GeneratorEngine::getStyles(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.styleIndex = idx; });
    });
    setupStepp (stepMotif,  GeneratorEngine::getMotifTypes(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.motifTypeIndex = idx; });
    });
    setupStepp (stepAnswer, GeneratorEngine::getResponseStrategies(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.responseStrategyIndex = idx; });
    });
    setupStepp (stepMode,   GeneratorEngine::getPhraseModes(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.phraseModeIndex = idx; });
    });
    setupStepp (stepEnding, GeneratorEngine::getEndingBehaviours(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.endingBehaviourIndex = idx; });
    });

    stepStyle.onLockChanged  = [this](bool l) { processorRef.lockStyle = l; };
    stepMotif.onLockChanged  = [this](bool l) { processorRef.lockMotif = l; };
    stepAnswer.onLockChanged = [this](bool l) { processorRef.lockStrategy = l; };
    stepMode.onLockChanged   = [this](bool l) { processorRef.lockMode = l; };
    stepEnding.onLockChanged = [this](bool l) { processorRef.lockEnding = l; };

    // SHAPE
    setupStepp (stepRegister,   GeneratorEngine::getRegisterProfiles(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.registerProfileIndex = idx; });
    });
    setupStepp (stepContour,    GeneratorEngine::getContourPresets(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.contourPresetIndex = idx; });
    });
    setupStepp (stepRepetition, GeneratorEngine::getRepetitionPresets(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.repetitionPresetIndex = idx; });
    });
    setupStepp (stepNotePool,   GeneratorEngine::getNotePoolModes(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.notePoolModeIndex = idx; });
    });
    setupStepp (stepResolve,    {"0%", "25%", "50%", "75%", "100%"}, true, [this](int idx) {
        float vals[] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
        if (auto* param = processorRef.apvts.getParameter ("resolve"))
            param->setValueNotifyingHost (param->convertTo0to1 (vals[juce::jlimit (0, 4, idx)]));
    });

    // ARTICULATION
    setupStepp (stepArticProfile,  GeneratorEngine::getArticulationProfiles(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.articulationProfileIndex = idx; });
    });
    setupStepp (stepGateFeel,      GeneratorEngine::getGateFeels(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.gateFeelIndex = idx; });
    });
    setupStepp (stepJoinTies,      GeneratorEngine::getConnectionFeels(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.connectionFeelIndex = idx; });
    });
    setupStepp (stepSlideProfile,  GeneratorEngine::getSlideProfiles(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.slideProfileIndex = idx; });
    });
    setupStepp (stepAccentProfile, GeneratorEngine::getAccentProfiles(), true, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.accentProfileIndex = idx; });
    });

    // Continuous Velocity (1 to 127) with presets and arrow stepping
    addChildComponent (stepVelocity);
    stepVelocity.setContinuousRange (1.0, 127.0, (double)processorRef.getConfig().baseVelocity, 1.0, "");
    stepVelocity.setContinuousPresets ({
        { "32  (Soft)", 32.0 },
        { "48  (Med Soft)", 48.0 },
        { "64  (Normal)", 64.0 },
        { "80  (Medium)", 80.0 },
        { "96  (Strong)", 96.0 },
        { "112 (Hard)", 112.0 },
        { "127 (Max)", 127.0 }
    });
    stepVelocity.onValueChanged = [this, updateCfg](double val) {
        if (isInternalUpdating) return;
        int v = juce::jlimit (1, 127, (int)std::round (val));
        updateCfg ([v](GeneratorConfig& c) {
            c.baseVelocity = v;
            const int velTable[] = { 32, 48, 64, 80, 96, 112, 127 };
            int bestIdx = 4;
            int bestDiff = 999;
            for (int i = 0; i < 7; ++i) {
                int d = std::abs (velTable[i] - v);
                if (d < bestDiff) { bestDiff = d; bestIdx = i; }
            }
            c.baseVelocityIndex = bestIdx;
        });
        if (onConfigChanged) onConfigChanged();
    };

    setupStepp (stepAccentAmount,  {"0%", "25%", "50%", "65%", "80%", "100%"}, false, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.accentAmountIndex = idx; });
    });

    // OPTIONS (Routing)
    std::vector<juce::String> channels = { "Track (Default)" };
    for (int ch = 1; ch <= 16; ++ch) {
        channels.push_back ("Channel " + juce::String (ch));
    }
    setupStepp (stepCallOut,   channels, false, [this](int idx) {
        processorRef.setCallChannel (idx <= 0 ? 1 : idx);
    });
    setupStepp (stepRespOut,   channels, false, [this](int idx) {
        processorRef.setResponseChannel (idx <= 0 ? 2 : idx);
    });
    setupStepp (stepSync,      {"Restart", "Free-running", "Host sync"}, false, [this](int idx) {
        if (auto* param = processorRef.apvts.getParameter ("syncMode"))
            param->setValueNotifyingHost (param->convertTo0to1 ((float)idx));
    });
    setupStepp (stepVariation, {"Small", "Medium", "Large"}, false, [updateCfg](int idx) {
        updateCfg ([idx](GeneratorConfig& c) { c.variationIndex = idx; });
    });
    setupStepp (stepSynthMute, {"Preview: ON", "Preview: MUTED"}, false, [this](int idx) {
        processorRef.muteAudio = (idx == 1);
    });

    // OPTIONS (Timing / Clips)
    setupStepp (stepNewPairTiming, {"Now", "Next Phrase"}, false, [this](int idx) {
        processorRef.nextPhraseRegen = (idx == 1);
    });
    setupStepp (stepClipMode,      {"Combined", "Separate (Call/Resp)", "Layered"}, false, [this](int idx) {
        if (onClipModeChanged) onClipModeChanged (idx);
    });
    setupStepp (stepLoopLength,    {"Exact phrase", "Whole Bars"}, false, [this](int idx) {
        if (onLoopLengthChanged) onLoopLengthChanged (idx == 1);
    });
}

void InspectorPanel::setTab (MainTab tab) {
    currentTab = tab;
    btnTabGenerate.setToggleState     (tab == MainTab::Generate,     juce::dontSendNotification);
    btnTabShape.setToggleState        (tab == MainTab::Shape,        juce::dontSendNotification);
    btnTabArticulation.setToggleState (tab == MainTab::Articulation, juce::dontSendNotification);
    btnTabOptions.setToggleState      (tab == MainTab::Options,      juce::dontSendNotification);

    bool isGen   = (tab == MainTab::Generate);
    bool isShape = (tab == MainTab::Shape);
    bool isArtic = (tab == MainTab::Articulation);
    bool isOpt   = (tab == MainTab::Options);

    stepStyle.setVisible  (isGen);
    stepMotif.setVisible  (isGen);
    stepAnswer.setVisible (isGen);
    stepMode.setVisible   (isGen);
    stepEnding.setVisible (isGen);

    stepRegister.setVisible   (isShape);
    stepContour.setVisible    (isShape);
    stepRepetition.setVisible (isShape);
    stepNotePool.setVisible   (isShape);
    stepResolve.setVisible    (isShape);

    stepArticProfile.setVisible  (isArtic);
    stepGateFeel.setVisible      (isArtic);
    stepJoinTies.setVisible      (isArtic);
    stepSlideProfile.setVisible  (isArtic);
    stepAccentProfile.setVisible (isArtic);
    stepVelocity.setVisible      (isArtic);
    stepAccentAmount.setVisible  (isArtic);

    btnSubRouting.setVisible     (isOpt);
    btnSubTimingClips.setVisible (isOpt);

    if (isOpt) {
        setOptionsSubTab (currentSubTab);
    } else {
        stepCallOut.setVisible (false);
        stepRespOut.setVisible (false);
        stepSync.setVisible (false);
        stepVariation.setVisible (false);
        stepSynthMute.setVisible (false);
        stepNewPairTiming.setVisible (false);
        stepClipMode.setVisible (false);
        stepLoopLength.setVisible (false);
    }

    resized();
    repaint();
}

void InspectorPanel::setOptionsSubTab (OptionsSubTab subTab) {
    currentSubTab = subTab;
    btnSubRouting.setToggleState     (subTab == OptionsSubTab::Routing,     juce::dontSendNotification);
    btnSubTimingClips.setToggleState (subTab == OptionsSubTab::TimingClips, juce::dontSendNotification);

    bool isRouting = (currentTab == MainTab::Options && subTab == OptionsSubTab::Routing);
    bool isTiming  = (currentTab == MainTab::Options && subTab == OptionsSubTab::TimingClips);

    stepCallOut.setVisible   (isRouting);
    stepRespOut.setVisible   (isRouting);
    stepSync.setVisible      (isRouting);
    stepVariation.setVisible (isRouting);
    stepSynthMute.setVisible (isRouting);

    stepNewPairTiming.setVisible (isTiming);
    stepClipMode.setVisible      (isTiming);
    stepLoopLength.setVisible    (isTiming);

    resized();
    repaint();
}

void InspectorPanel::updateFromConfig (const GeneratorConfig& cfg) {
    juce::ScopedValueSetter<bool> svs (isInternalUpdating, true);

    if (stepStyle.getSelectedIndex() != cfg.styleIndex)
        stepStyle.setSelectedIndex   (cfg.styleIndex, juce::dontSendNotification);
    if (stepMotif.getSelectedIndex() != cfg.motifTypeIndex)
        stepMotif.setSelectedIndex   (cfg.motifTypeIndex, juce::dontSendNotification);
    if (stepAnswer.getSelectedIndex() != cfg.responseStrategyIndex)
        stepAnswer.setSelectedIndex  (cfg.responseStrategyIndex, juce::dontSendNotification);
    if (stepMode.getSelectedIndex() != cfg.phraseModeIndex)
        stepMode.setSelectedIndex    (cfg.phraseModeIndex, juce::dontSendNotification);
    if (stepEnding.getSelectedIndex() != cfg.endingBehaviourIndex)
        stepEnding.setSelectedIndex  (cfg.endingBehaviourIndex, juce::dontSendNotification);

    if (stepRegister.getSelectedIndex() != cfg.registerProfileIndex)
        stepRegister.setSelectedIndex   (cfg.registerProfileIndex, juce::dontSendNotification);
    if (stepContour.getSelectedIndex() != cfg.contourPresetIndex)
        stepContour.setSelectedIndex    (cfg.contourPresetIndex, juce::dontSendNotification);
    if (stepRepetition.getSelectedIndex() != cfg.repetitionPresetIndex)
        stepRepetition.setSelectedIndex (cfg.repetitionPresetIndex, juce::dontSendNotification);
    if (stepNotePool.getSelectedIndex() != cfg.notePoolModeIndex)
        stepNotePool.setSelectedIndex   (cfg.notePoolModeIndex, juce::dontSendNotification);

    if (stepArticProfile.getSelectedIndex() != cfg.articulationProfileIndex)
        stepArticProfile.setSelectedIndex  (cfg.articulationProfileIndex, juce::dontSendNotification);
    if (stepGateFeel.getSelectedIndex() != cfg.gateFeelIndex)
        stepGateFeel.setSelectedIndex      (cfg.gateFeelIndex, juce::dontSendNotification);
    if (stepJoinTies.getSelectedIndex() != cfg.connectionFeelIndex)
        stepJoinTies.setSelectedIndex      (cfg.connectionFeelIndex, juce::dontSendNotification);
    if (stepSlideProfile.getSelectedIndex() != cfg.slideProfileIndex)
        stepSlideProfile.setSelectedIndex  (cfg.slideProfileIndex, juce::dontSendNotification);
    if (stepAccentProfile.getSelectedIndex() != cfg.accentProfileIndex)
        stepAccentProfile.setSelectedIndex (cfg.accentProfileIndex, juce::dontSendNotification);
    if (std::abs (stepVelocity.getCurrentValue() - (double)cfg.baseVelocity) > 0.5)
        stepVelocity.setCurrentValue ((double)cfg.baseVelocity, juce::dontSendNotification);
    if (stepAccentAmount.getSelectedIndex() != cfg.accentAmountIndex)
        stepAccentAmount.setSelectedIndex (cfg.accentAmountIndex, juce::dontSendNotification);
    if (stepVariation.getSelectedIndex() != cfg.variationIndex)
        stepVariation.setSelectedIndex (cfg.variationIndex, juce::dontSendNotification);

    if (auto* param = processorRef.apvts.getRawParameterValue ("resolve")) {
        float val = param->load();
        int resIdx = juce::jlimit (0, 4, juce::roundToInt (val * 4.0f));
        if (stepResolve.getSelectedIndex() != resIdx)
            stepResolve.setSelectedIndex (resIdx, juce::dontSendNotification);
    }
    if (auto* param = processorRef.apvts.getRawParameterValue ("syncMode")) {
        int sm = juce::jlimit (0, 2, juce::roundToInt (param->load()));
        if (stepSync.getSelectedIndex() != sm)
            stepSync.setSelectedIndex (sm, juce::dontSendNotification);
    }

    if (stepCallOut.getSelectedIndex() != cfg.callChannel)
        stepCallOut.setSelectedIndex (cfg.callChannel, juce::dontSendNotification);
    if (stepRespOut.getSelectedIndex() != cfg.responseChannel)
        stepRespOut.setSelectedIndex (cfg.responseChannel, juce::dontSendNotification);

    stepStyle.setLocked  (processorRef.lockStyle, juce::dontSendNotification);
    stepMotif.setLocked  (processorRef.lockMotif, juce::dontSendNotification);
    stepAnswer.setLocked (processorRef.lockStrategy, juce::dontSendNotification);
    stepMode.setLocked   (processorRef.lockMode, juce::dontSendNotification);
    stepEnding.setLocked (processorRef.lockEnding, juce::dontSendNotification);

    stepNewPairTiming.setSelectedIndex (processorRef.nextPhraseRegen ? 1 : 0, juce::dontSendNotification);
    stepSynthMute.setSelectedIndex     (processorRef.muteAudio ? 1 : 0, juce::dontSendNotification);
}

void InspectorPanel::updateFromProcessor() {
    updateFromConfig (processorRef.getConfig());
}

void InspectorPanel::paint (juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    // Background panel
    g.setColour (CustomLookAndFeel::getPanelBg().withAlpha (0.9f));
    g.fillRoundedRectangle (bounds, 6.0f);

    g.setColour (CustomLookAndFeel::getLineBorder().withAlpha (0.5f));
    g.drawRoundedRectangle (bounds, 6.0f, 1.0f);

    // Tab underline indicator for active tab
    juce::Rectangle<int> activeTabBounds;
    switch (currentTab) {
        case MainTab::Generate:     activeTabBounds = btnTabGenerate.getBounds(); break;
        case MainTab::Shape:        activeTabBounds = btnTabShape.getBounds(); break;
        case MainTab::Articulation: activeTabBounds = btnTabArticulation.getBounds(); break;
        case MainTab::Options:      activeTabBounds = btnTabOptions.getBounds(); break;
    }

    if (!activeTabBounds.isEmpty()) {
        g.setColour (CustomLookAndFeel::getNeonEmerald());
        g.fillRect (activeTabBounds.getX() + 4, activeTabBounds.getBottom() - 2,
                    activeTabBounds.getWidth() - 8, 2);
    }
}

void InspectorPanel::resized() {
    auto bounds = getLocalBounds().reduced (6, 4);

    // 1. Tab Bar
    auto tabArea = bounds.removeFromTop (26);
    int tabW = tabArea.getWidth() / 4;
    btnTabGenerate.setBounds     (tabArea.removeFromLeft (tabW).reduced (1));
    btnTabShape.setBounds        (tabArea.removeFromLeft (tabW).reduced (1));
    btnTabArticulation.setBounds (tabArea.removeFromLeft (tabW).reduced (1));
    btnTabOptions.setBounds      (tabArea.reduced (1));

    bounds.removeFromTop (4);

    // 2. Legend at bottom
    lblLegend.setBounds (bounds.removeFromBottom (16));

    // 3. Subtabs if Options is active
    if (currentTab == MainTab::Options) {
        auto subArea = bounds.removeFromTop (22);
        int subW = subArea.getWidth() / 2;
        btnSubRouting.setBounds     (subArea.removeFromLeft (subW).reduced (4, 1));
        btnSubTimingClips.setBounds (subArea.reduced (4, 1));
        bounds.removeFromTop (4);
    }

    // 4. Rows
    int rowH = 22;
    int gap = 2;

    auto layoutRows = [&](const std::vector<StepperComponent*>& rows) {
        auto rArea = bounds;
        for (auto* row : rows) {
            if (row != nullptr && row->isVisible()) {
                row->setBounds (rArea.removeFromTop (rowH));
                rArea.removeFromTop (gap);
            }
        }
    };

    if (currentTab == MainTab::Generate) {
        layoutRows ({ &stepStyle, &stepMotif, &stepAnswer, &stepMode, &stepEnding });
    } else if (currentTab == MainTab::Shape) {
        layoutRows ({ &stepRegister, &stepContour, &stepRepetition, &stepNotePool, &stepResolve });
    } else if (currentTab == MainTab::Articulation) {
        layoutRows ({ &stepArticProfile, &stepGateFeel, &stepJoinTies, &stepSlideProfile, &stepAccentProfile, &stepVelocity, &stepAccentAmount });
    } else if (currentTab == MainTab::Options) {
        if (currentSubTab == OptionsSubTab::Routing) {
            layoutRows ({ &stepCallOut, &stepRespOut, &stepSync, &stepVariation, &stepSynthMute });
        } else {
            layoutRows ({ &stepNewPairTiming, &stepClipMode, &stepLoopLength });
        }
    }
}

} // namespace CR
