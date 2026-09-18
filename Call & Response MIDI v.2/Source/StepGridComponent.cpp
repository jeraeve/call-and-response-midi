#include "StepGridComponent.h"
#include "CustomLookAndFeel.h"

namespace CR {

class StepGridComponent::LaneLockButton : public juce::Button {
public:
    LaneLockButton() : juce::Button ("LaneLock") {
        setClickingTogglesState (true);
        setTooltip ("Lock phrase from being overwritten during generation");
    }

    void paintButton (juce::Graphics& g, bool isMouseOver, bool /*isButtonDown*/) override {
        auto bounds = getLocalBounds().toFloat();
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        bool locked = getToggleState();

        float bodyW = 9.0f;
        float bodyH = 7.5f;
        float bodyX = cx - bodyW * 0.5f;
        float bodyY = cy - 0.5f;

        juce::Colour accent = locked ? juce::Colour (0xfff59e0b) // Amber gold for phrase lock
                                     : (isMouseOver ? CustomLookAndFeel::getActiveText()
                                                    : CustomLookAndFeel::getMutedText().withAlpha (0.45f));

        if (locked) {
            g.setColour (accent.withAlpha (0.16f));
            g.fillRoundedRectangle (bounds.reduced (1.0f), 3.0f);
            g.setColour (accent.withAlpha (0.6f));
            g.drawRoundedRectangle (bounds.reduced (1.0f), 3.0f, 1.0f);
        } else if (isMouseOver) {
            g.setColour (juce::Colour (0xff252328));
            g.fillRoundedRectangle (bounds.reduced (1.0f), 3.0f);
        }

        // Shackle
        juce::Path shackle;
        float shackleR = 3.2f;
        if (locked) {
            shackle.addCentredArc (cx, bodyY + 1.0f, shackleR, shackleR, 0.0f,
                                   -juce::MathConstants<float>::pi * 0.5f,
                                   juce::MathConstants<float>::pi * 0.5f, true);
        } else {
            shackle.addCentredArc (cx - 1.5f, bodyY - 1.0f, shackleR, shackleR, 0.0f,
                                   -juce::MathConstants<float>::pi * 0.6f,
                                   juce::MathConstants<float>::pi * 0.4f, true);
        }

        g.setColour (accent);
        g.strokePath (shackle, juce::PathStrokeType (1.5f));

        // Lock Body
        if (locked) {
            g.setColour (accent);
            g.fillRoundedRectangle (bodyX, bodyY, bodyW, bodyH, 1.5f);
            g.setColour (juce::Colours::black);
            g.fillEllipse (cx - 1.0f, bodyY + 2.5f, 2.0f, 2.0f);
        } else {
            g.setColour (accent);
            g.drawRoundedRectangle (bodyX, bodyY, bodyW, bodyH, 1.5f, 1.2f);
            g.fillEllipse (cx - 0.75f, bodyY + 2.5f, 1.5f, 1.5f);
        }
    }
};

StepGridComponent::StepGridComponent (const juce::String& laneTitle, juce::Colour accentColor)
    : title (laneTitle), laneAccent (accentColor) {

    // Lane Lock button next to badge
    btnLockLane = std::make_unique<LaneLockButton>();
    btnLockLane->setTooltip ("Lock " + title + " phrase from being regenerated");
    addAndMakeVisible (*btnLockLane);
    btnLockLane->onClick = [this] {
        isLock = btnLockLane->getToggleState();
        if (onLockToggled) onLockToggled (isLock);
        repaint();
    };

    // 1. Mute button under lane badge
    btnMuteLane.setClickingTogglesState (true);
    btnMuteLane.setTooltip ("Mute / Bypass this lane");
    btnMuteLane.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff252326));
    btnMuteLane.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff7f1d1d)); // Red when muted
    btnMuteLane.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getMutedText());
    btnMuteLane.setColour (juce::TextButton::textColourOnId, juce::Colour (0xfffca5a5)); // Light red text
    addAndMakeVisible (btnMuteLane);
    btnMuteLane.onClick = [this] {
        isMute = btnMuteLane.getToggleState();
        if (onMuteToggled) onMuteToggled (isMute);
        repaint();
    };

    // 2. Dice button under lane badge to mutate this lane
    btnDiceLane.setTooltip ("Dice: Mutate this lane's content");
    btnDiceLane.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff252326));
    btnDiceLane.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getActiveText());
    addAndMakeVisible (btnDiceLane);
    btnDiceLane.onClick = [this] {
        if (onDiceRequested) onDiceRequested();
    };

    // 3. Mutate percentage button/selector
    btnMutatePct.setTooltip ("Click to select mutation percentage or change amount");
    btnMutatePct.setColour (juce::TextButton::buttonColourId, juce::Colour (0xff1f1d20));
    btnMutatePct.setColour (juce::TextButton::textColourOffId, CustomLookAndFeel::getMutedText());
    addAndMakeVisible (btnMutatePct);
    btnMutatePct.onClick = [this] {
        juce::PopupMenu menu;
        menu.addSectionHeader ("MUTATE AMOUNT");
        int presetValues[] = { 0, 10, 20, 30, 40, 50, 75, 100 };
        for (int v : presetValues) {
            menu.addItem (v + 1, juce::String (v) + "%", true, mutatePercent == v);
        }
        menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&btnMutatePct),
            [this](int result) {
                if (result > 0) {
                    setMutatePercent (result - 1);
                    if (onMutatePercentChanged) onMutatePercentChanged (mutatePercent);
                }
            });
    };

    // Compact utilities on top-right of lane
    auto setupBtn = [this](juce::TextButton& btn, const juce::String& tip) {
        btn.setTooltip (tip);
        addAndMakeVisible (btn);
    };

    setupBtn (btnCopy,       "Copy lane pattern & options");
    setupBtn (btnShiftLeft,  "Shift phrase 1 step left");
    setupBtn (btnShiftRight, "Shift phrase 1 step right");
    setupBtn (btnOctUp,      "+1 Octave");
    setupBtn (btnOctDown,    "-1 Octave");
    setupBtn (btnInvert,     "Invert phrase");
    setupBtn (btnReverse,    "Reverse phrase");

    btnCopy.onClick = [this] {
        juce::PopupMenu m;
        m.addItem (1, "Shift Left (-1 step)");
        m.addItem (2, "Shift Right (+1 step)");
        m.addSeparator();
        m.addItem (3, "Transpose Octave Up (+12)");
        m.addItem (4, "Transpose Octave Down (-12)");
        m.addSeparator();
        m.addItem (5, "Invert Melody");
        m.addItem (6, "Reverse Phrase");
        m.addSeparator();
        m.addItem (7, "Clear All Steps");

        m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&btnCopy),
            [this](int result) {
                if (result == 1 && onShiftRequested)  onShiftRequested (-1);
                if (result == 2 && onShiftRequested)  onShiftRequested (1);
                if (result == 3 && onOctaveRequested) onOctaveRequested (1);
                if (result == 4 && onOctaveRequested) onOctaveRequested (-1);
                if (result == 5 && onInvertRequested) onInvertRequested();
                if (result == 6 && onReverseRequested) onReverseRequested();
                if (result == 7) {
                    for (auto& s : phrase) s.active = false;
                    if (onPhraseChanged) onPhraseChanged();
                    repaint();
                }
            });
    };

    btnShiftLeft.onClick  = [this] { if (onShiftRequested)  onShiftRequested (-1); };
    btnShiftRight.onClick = [this] { if (onShiftRequested)  onShiftRequested (1); };
    btnOctUp.onClick      = [this] { if (onOctaveRequested) onOctaveRequested (1); };
    btnOctDown.onClick    = [this] { if (onOctaveRequested) onOctaveRequested (-1); };
    btnInvert.onClick     = [this] { if (onInvertRequested) onInvertRequested(); };
    btnReverse.onClick    = [this] { if (onReverseRequested) onReverseRequested(); };

    // Populate MIDI channel 1-16
    for (int ch = 1; ch <= 16; ++ch) {
        cbChannel.addItem ("MIDI: Ch " + juce::String (ch), ch);
    }
    cbChannel.setSelectedId (1, juce::dontSendNotification);
    cbChannel.setTooltip ("Select MIDI Output Channel for this lane");
    addAndMakeVisible (cbChannel);
    cbChannel.onChange = [this] {
        if (onChannelChanged) onChannelChanged (cbChannel.getSelectedId());
    };
}

StepGridComponent::~StepGridComponent() {
    btnMuteLane.onClick = nullptr;
    btnDiceLane.onClick = nullptr;
    btnCopy.onClick = nullptr;
    btnShiftLeft.onClick = nullptr;
    btnShiftRight.onClick = nullptr;
    btnOctUp.onClick = nullptr;
    btnOctDown.onClick = nullptr;
    btnInvert.onClick = nullptr;
    if (btnLockLane != nullptr) btnLockLane->onClick = nullptr;
    cbChannel.onChange = nullptr;
    btnMutatePct.onClick = nullptr;

    if (auditioningMidiNote >= 0 && onAuditionNote) {
        onAuditionNote (auditioningMidiNote, false);
        auditioningMidiNote = -1;
    }

    onPhraseChanged = nullptr;
    onEditGestureStarted = nullptr;
    onMuteToggled = nullptr;
    onLockToggled = nullptr;
    onDiceRequested = nullptr;
    onMutatePercentChanged = nullptr;
    onShiftRequested = nullptr;
    onInvertRequested = nullptr;
    onReverseRequested = nullptr;
    onOctaveRequested = nullptr;
    onChannelChanged = nullptr;
    onAuditionNote = nullptr;
}

void StepGridComponent::setLocked (bool locked) {
    isLock = locked;
    if (btnLockLane != nullptr)
        btnLockLane->setToggleState (locked, juce::dontSendNotification);
    repaint();
}

void StepGridComponent::setMutatePercent (int pct) {
    mutatePercent = juce::jlimit (0, 100, pct);
    btnMutatePct.setButtonText ("MUTATE " + juce::String (mutatePercent) + "%");
    repaint();
}

int StepGridComponent::getMutatePercent() const {
    return mutatePercent;
}

void StepGridComponent::setChannel (int ch) {
    cbChannel.setSelectedId (juce::jlimit (1, 16, ch), juce::dontSendNotification);
}

int StepGridComponent::getChannel() const {
    return cbChannel.getSelectedId();
}

void StepGridComponent::triggerMidiActivity() {
    midiActivityIntensity = 1.0f;
    repaint();
}

void StepGridComponent::setPhrase (const std::vector<Step>& steps, const GeneratorConfig& config) {
    phrase = steps;
    currentConfig = config;
    repaint();
}

void StepGridComponent::setCurrentStep (int stepIndex) {
    if (playingStep != stepIndex) {
        playingStep = stepIndex;
        repaint();
    }
}

void StepGridComponent::setMuted (bool mute) {
    isMute = mute;
    btnMuteLane.setToggleState (mute, juce::dontSendNotification);
    repaint();
}

juce::Rectangle<int> StepGridComponent::getGridBounds() const {
    return getLocalBounds().withTrimmedLeft (88).withTrimmedTop (20).withTrimmedBottom (16).reduced (4, 0);
}

int StepGridComponent::getStepIndexAtPoint (juce::Point<int> pt) const {
    auto grid = getGridBounds();
    if (!grid.contains (pt) || phrase.empty()) return -1;

    int n = (int)phrase.size();
    float stepW = (float)grid.getWidth() / (float)n;
    int idx = (int)((float)(pt.x - grid.getX()) / stepW);
    return juce::jlimit (0, n - 1, idx);
}

void StepGridComponent::paint (juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    // Subtle container background matching reference
    g.setColour (CustomLookAndFeel::getTopBarBg());
    g.fillRoundedRectangle (bounds, 5.0f);

    g.setColour (isMute ? juce::Colour (0xff5f2424) : CustomLookAndFeel::getLineBorder().withAlpha (0.5f));
    g.drawRoundedRectangle (bounds, 5.0f, 1.0f);

    // Left Lane Badge (fits nicely beside lock button)
    juce::Rectangle<float> badgeArea (6.0f, 6.0f, 56.0f, 18.0f);
    g.setColour (isMute ? juce::Colour (0xff2d1a1a) : laneAccent.withAlpha (0.15f));
    g.fillRoundedRectangle (badgeArea, 3.0f);
    g.setColour (isMute ? juce::Colour (0xffa04040) : laneAccent);
    g.drawRoundedRectangle (badgeArea, 3.0f, 1.0f);

    g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
    g.drawText (title, badgeArea.withTrimmedRight (7.0f).toNearestInt(), juce::Justification::centred);

    // Activity LED
    float ledX = 54.5f;
    float ledY = 12.0f;
    juce::Colour ledCol = isMute ? juce::Colour (0xff5a2a2a)
                         : (midiActivityIntensity > 0.05f ? laneAccent.interpolatedWith (juce::Colours::white, midiActivityIntensity)
                                                          : laneAccent.withAlpha (0.3f));
    g.setColour (ledCol);
    g.fillEllipse (ledX, ledY, 4.0f, 4.0f);
    if (midiActivityIntensity > 0.05f) {
        midiActivityIntensity = std::max (0.0f, midiActivityIntensity - 0.12f);
    }

    // Draw Steps
    auto grid = getGridBounds();
    if (phrase.empty()) return;

    int n = (int)phrase.size();
    float gap = 4.0f;
    float totalW = (float)grid.getWidth();
    float stepW = (totalW - gap * (float)(n - 1)) / (float)n;
    float stepH = (float)grid.getHeight();

    for (int i = 0; i < n; ++i) {
        float x = (float)grid.getX() + (float)i * (stepW + gap);
        float y = (float)grid.getY();
        auto cellBounds = juce::Rectangle<float> (x, y, stepW, stepH);

        const auto& s = phrase[(size_t)i];
        bool isPlaying = (playingStep == i);

        // Step number header above
        g.setFont (juce::FontOptions (9.0f));
        g.setColour (CustomLookAndFeel::getMutedText().withAlpha (0.6f));
        g.drawText (juce::String (i + 1), (int)x, (int)y - 12, (int)stepW, 10, juce::Justification::centred);

        // Slot background
        juce::Colour cellBg = isPlaying ? juce::Colour (0xff252f3d)
                            : (isMute ? juce::Colour (0xff171618) : juce::Colour (0xff1b1a1c));
        g.setColour (cellBg);
        g.fillRoundedRectangle (cellBounds, 3.5f);

        // Border
        juce::Colour borderCol = isPlaying ? juce::Colours::white : juce::Colour (0xff2b282d);
        g.setColour (borderCol);
        g.drawRoundedRectangle (cellBounds, 3.5f, isPlaying ? 1.5f : 1.0f);

        // Active Note Pill reflecting length, velocity, accent, slide
        if (s.active) {
            float availableW = cellBounds.getWidth() - 4.0f;
            float gateFactor = juce::jlimit (0.2f, 1.0f, s.gate);
            float pillW = std::max (6.0f, availableW * gateFactor);
            float pillH = cellBounds.getHeight() - 8.0f;
            float pillX = cellBounds.getX() + 2.0f;
            float pillY = cellBounds.getY() + 4.0f;

            // If staccato / short gate, center narrow pill in slot
            if (gateFactor < 0.45f) {
                pillX = cellBounds.getCentreX() - pillW * 0.5f;
            }

            auto pillBounds = juce::Rectangle<float> (pillX, pillY, pillW, pillH);

            // If tied to next, extend right edge through gap
            bool tiesToNext = (s.tie && (i + 1 < n) && phrase[(size_t)(i + 1)].active);
            if (tiesToNext) {
                juce::Colour tieCol = isMute ? laneAccent.withAlpha (0.25f) : laneAccent.withAlpha (0.85f);
                g.setColour (tieCol);
                float tieBarY = cellBounds.getCentreY() - 3.0f;
                g.fillRect (pillBounds.getRight() - 1.0f, tieBarY, (cellBounds.getRight() - pillBounds.getRight()) + gap + 4.0f, 6.0f);
            }

            // Note pill fill with velocity / accent brightness
            float velNorm = s.accent ? 1.0f : 0.82f;
            juce::Colour basePill = s.accent ? laneAccent.brighter (0.2f) : laneAccent;
            juce::Colour pillCol = isMute ? basePill.withAlpha (0.35f) : basePill.withAlpha (0.92f * velNorm);
            g.setColour (pillCol);
            g.fillRoundedRectangle (pillBounds, 3.5f);

            // Accent border & indicator
            if (s.accent) {
                g.setColour (isMute ? juce::Colours::grey : juce::Colours::white);
                g.drawRoundedRectangle (pillBounds, 3.5f, 1.5f);

                // Accent dot
                g.setColour (juce::Colours::white);
                g.fillEllipse (pillBounds.getX() + 3.0f, pillBounds.getCentreY() - 2.0f, 4.0f, 4.0f);
            } else {
                g.setColour (isMute ? pillCol.darker (0.2f) : pillCol.brighter (0.25f));
                g.drawRoundedRectangle (pillBounds, 3.5f, 1.0f);
            }

            // Slide indicator: diagonal tail in neon cyan
            if (s.slide) {
                g.setColour (CustomLookAndFeel::getNeonCyan());
                g.drawLine (pillBounds.getRight() - 2.0f, pillBounds.getBottom() - 1.0f,
                            pillBounds.getRight() + 4.0f, pillBounds.getBottom() + 3.0f, 2.2f);
            }

            // Note text (if wide enough)
            if (pillW >= 18.0f) {
                g.setFont (juce::FontOptions (10.5f, juce::Font::bold));
                bool isBrightLane = (laneAccent.getBrightness() > 0.6f);
                g.setColour (isMute ? juce::Colours::lightgrey : (isBrightLane ? juce::Colours::black : juce::Colours::white));
                auto textRect = pillBounds;
                if (s.accent) textRect.removeFromLeft (8.0f);
                g.drawText (GeneratorEngine::midiToName (s.getEffectiveMidiNote()),
                            textRect.toNearestInt(), juce::Justification::centred);
            }
        }

        // Sub-step Articulation marker below the step slot
        juce::String tag;
        if (s.accent && s.slide && s.tie)   tag = "AST";
        else if (s.accent && s.slide)        tag = "AS";
        else if (s.accent && s.tie)          tag = "AT";
        else if (s.slide && s.tie)           tag = "ST";
        else if (s.accent)                   tag = "A";
        else if (s.slide)                    tag = "S";
        else if (s.tie)                      tag = "T";

        if (tag.isNotEmpty()) {
            g.setFont (juce::FontOptions (9.5f, juce::Font::bold));
            juce::Colour tagCol = isMute ? CustomLookAndFeel::getMutedText().withAlpha (0.4f)
                                : (s.slide ? CustomLookAndFeel::getNeonCyan()
                                           : (s.accent ? CustomLookAndFeel::getNeonOrange()
                                                       : laneAccent));
            g.setColour (tagCol);
            g.drawText (tag, (int)x, (int)cellBounds.getBottom() + 1, (int)stepW, 12, juce::Justification::centred);
        }
    }
}

void StepGridComponent::resized() {
    auto bounds = getLocalBounds();

    // 1. Left controls: Lock button next to badge, then MUTE and DICE buttons side-by-side, then MUTATE %
    if (btnLockLane != nullptr)
        btnLockLane->setBounds (65, 6, 18, 18);

    auto leftArea = bounds.removeFromLeft (86).reduced (4, 4);
    leftArea.removeFromTop (22); // Skip badge & lock row

    auto buttonRow = leftArea.removeFromTop (22);
    btnMuteLane.setBounds (buttonRow.removeFromLeft (44).reduced (1, 1));
    buttonRow.removeFromLeft (2);
    btnDiceLane.setBounds (buttonRow.reduced (1, 1));

    leftArea.removeFromTop (2);
    btnMutatePct.setBounds (leftArea.removeFromTop (18).reduced (1, 0));

    // 2. Right compact lane utilities in the top row
    auto headerRight = bounds.removeFromTop (18).reduced (4, 1);
    cbChannel.setBounds (headerRight.removeFromRight (92).reduced (1, 0));
    headerRight.removeFromRight (4);
    btnCopy.setBounds (headerRight.removeFromRight (18).reduced (1, 0));
    btnReverse.setBounds (headerRight.removeFromRight (28).reduced (1, 0));
    btnInvert.setBounds (headerRight.removeFromRight (28).reduced (1, 0));
    btnOctUp.setBounds (headerRight.removeFromRight (34).reduced (1, 0));
    btnOctDown.setBounds (headerRight.removeFromRight (34).reduced (1, 0));
    btnShiftRight.setBounds (headerRight.removeFromRight (18).reduced (1, 0));
    btnShiftLeft.setBounds (headerRight.removeFromRight (18).reduced (1, 0));
}

void StepGridComponent::mouseDown (const juce::MouseEvent& e) {
    int idx = getStepIndexAtPoint (e.getPosition());
    if (idx < 0 || idx >= (int)phrase.size()) return;

    if (onEditGestureStarted) onEditGestureStarted();

    if (e.mods.isRightButtonDown()) {
        juce::PopupMenu menu;
        auto& s = phrase[(size_t)idx];
        menu.addItem (1, "Active", true, s.active);
        menu.addItem (2, "Accent (A)", true, s.accent);
        menu.addItem (3, "Slide (S)", true, s.slide);
        menu.addItem (4, "Tie (T)", true, s.tie);
        menu.addSeparator();
        menu.addItem (5, "Octave Up (+12)");
        menu.addItem (6, "Octave Down (-12)");

        menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (this),
            [this, idx](int result) {
                if (result == 0) return;
                auto& step = phrase[(size_t)idx];
                if (result == 1) step.active = !step.active;
                else if (result == 2) step.accent = !step.accent;
                else if (result == 3) step.slide = !step.slide;
                else if (result == 4) step.tie = !step.tie;
                else if (result == 5) step.octaveOffset += 12;
                else if (result == 6) step.octaveOffset -= 12;

                if (onPhraseChanged) onPhraseChanged();
                repaint();
            });
        return;
    }

    draggedStepIndex = idx;
    dragStartY = (float)e.y;
    dragStartDegree = phrase[(size_t)idx].degree;

    if (e.mods.isCommandDown() || e.mods.isCtrlDown()) {
        phrase[(size_t)idx].accent = !phrase[(size_t)idx].accent;
    } else if (e.mods.isShiftDown()) {
        phrase[(size_t)idx].slide = !phrase[(size_t)idx].slide;
    } else if (e.mods.isAltDown()) {
        phrase[(size_t)idx].tie = !phrase[(size_t)idx].tie;
    } else {
        phrase[(size_t)idx].active = !phrase[(size_t)idx].active;
    }

    if (phrase[(size_t)idx].active && onAuditionNote) {
        auditioningMidiNote = phrase[(size_t)idx].getEffectiveMidiNote();
        onAuditionNote (auditioningMidiNote, true);
    }

    if (onPhraseChanged) onPhraseChanged();
    repaint();
}

void StepGridComponent::mouseDoubleClick (const juce::MouseEvent& e) {
    int idx = getStepIndexAtPoint (e.getPosition());
    if (idx >= 0 && idx < (int)phrase.size()) {
        if (onEditGestureStarted) onEditGestureStarted();
        phrase[(size_t)idx].accent = !phrase[(size_t)idx].accent;
        if (onPhraseChanged) onPhraseChanged();
        repaint();
    }
}

void StepGridComponent::mouseDrag (const juce::MouseEvent& e) {
    if (draggedStepIndex < 0 || draggedStepIndex >= (int)phrase.size()) return;

    float deltaY = dragStartY - (float)e.y;
    int degreeStep = (int)(deltaY / 12.0f);
    int newDegree = juce::jlimit (-14, 21, dragStartDegree + degreeStep);

    if (newDegree != phrase[(size_t)draggedStepIndex].degree) {
        phrase[(size_t)draggedStepIndex].degree = newDegree;
        phrase[(size_t)draggedStepIndex].midiNote = GeneratorEngine::degreeToMidi (newDegree, currentConfig);
        phrase[(size_t)draggedStepIndex].active = true;

        int newMidi = phrase[(size_t)draggedStepIndex].getEffectiveMidiNote();
        if (newMidi != auditioningMidiNote) {
            if (auditioningMidiNote >= 0 && onAuditionNote)
                onAuditionNote (auditioningMidiNote, false);
            auditioningMidiNote = newMidi;
            if (onAuditionNote)
                onAuditionNote (auditioningMidiNote, true);
        }

        if (onPhraseChanged) onPhraseChanged();
        repaint();
    }
}

void StepGridComponent::mouseUp (const juce::MouseEvent&) {
    if (auditioningMidiNote >= 0) {
        if (onAuditionNote)
            onAuditionNote (auditioningMidiNote, false);
        auditioningMidiNote = -1;
    }
    draggedStepIndex = -1;
}

void StepGridComponent::mouseExit (const juce::MouseEvent&) {
    if (auditioningMidiNote >= 0) {
        if (onAuditionNote)
            onAuditionNote (auditioningMidiNote, false);
        auditioningMidiNote = -1;
    }
}

void StepGridComponent::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) {
    int idx = getStepIndexAtPoint (e.getPosition());
    if (idx < 0 || idx >= (int)phrase.size()) return;

    if (wheel.deltaY > 0.05f) {
        phrase[(size_t)idx].degree = juce::jlimit (-14, 21, phrase[(size_t)idx].degree + 1);
        phrase[(size_t)idx].midiNote = GeneratorEngine::degreeToMidi (phrase[(size_t)idx].degree, currentConfig);
        if (onPhraseChanged) onPhraseChanged();
        repaint();
    } else if (wheel.deltaY < -0.05f) {
        phrase[(size_t)idx].degree = juce::jlimit (-14, 21, phrase[(size_t)idx].degree - 1);
        phrase[(size_t)idx].midiNote = GeneratorEngine::degreeToMidi (phrase[(size_t)idx].degree, currentConfig);
        if (onPhraseChanged) onPhraseChanged();
        repaint();
    }
}

} // namespace CR
