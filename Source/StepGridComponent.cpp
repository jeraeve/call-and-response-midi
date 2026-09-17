#include "StepGridComponent.h"
#include "CustomLookAndFeel.h"

namespace CR {

StepGridComponent::StepGridComponent(const juce::String& laneTitle, juce::Colour accentColor)
    : title(laneTitle), laneAccent(accentColor) {

    auto setupBtn = [this](juce::TextButton& btn, const juce::String& tip) {
        btn.setTooltip(tip);
        addAndMakeVisible(btn);
    };

    // Populate MIDI channel 1-16
    for (int ch = 1; ch <= 16; ++ch) {
        cbChannel.addItem("MIDI Out: Ch " + juce::String(ch), ch);
    }
    cbChannel.setSelectedId(1, juce::dontSendNotification);
    cbChannel.setTooltip("Select MIDI Output Channel for this lane to route to another track or instrument");
    addAndMakeVisible(cbChannel);
    cbChannel.onChange = [this] {
        if (onChannelChanged) onChannelChanged(cbChannel.getSelectedId());
    };

    setupBtn(btnShiftLeft,  "Shift phrase 1 step left");
    setupBtn(btnShiftRight, "Shift phrase 1 step right");
    setupBtn(btnOctUp,      "Transpose phrase +1 octave");
    setupBtn(btnOctDown,    "Transpose phrase -1 octave");
    setupBtn(btnInvert,     "Invert pitch contour around center");
    setupBtn(btnReverse,    "Reverse phrase playback order");

    btnMute.setClickingTogglesState(true);
    btnMute.setTooltip("Mute MIDI output for this lane");
    addAndMakeVisible(btnMute);

    btnShiftLeft.onClick  = [this] { if (onShiftRequested)  onShiftRequested(-1); };
    btnShiftRight.onClick = [this] { if (onShiftRequested)  onShiftRequested(1); };
    btnOctUp.onClick      = [this] { if (onOctaveRequested) onOctaveRequested(1); };
    btnOctDown.onClick    = [this] { if (onOctaveRequested) onOctaveRequested(-1); };
    btnInvert.onClick     = [this] { if (onInvertRequested) onInvertRequested(); };
    btnReverse.onClick    = [this] { if (onReverseRequested) onReverseRequested(); };
    btnMute.onClick       = [this] {
        isMute = btnMute.getToggleState();
        repaint();
    };
}

StepGridComponent::~StepGridComponent() {
    cbChannel.onChange = nullptr;
    btnShiftLeft.onClick = nullptr;
    btnShiftRight.onClick = nullptr;
    btnOctUp.onClick = nullptr;
    btnOctDown.onClick = nullptr;
    btnInvert.onClick = nullptr;
    btnReverse.onClick = nullptr;
    btnMute.onClick = nullptr;
    onPhraseChanged = nullptr;
    onShiftRequested = nullptr;
    onInvertRequested = nullptr;
    onReverseRequested = nullptr;
    onOctaveRequested = nullptr;
    onChannelChanged = nullptr;
}

void StepGridComponent::setChannel(int ch) {
    cbChannel.setSelectedId(juce::jlimit(1, 16, ch), juce::dontSendNotification);
}

int StepGridComponent::getChannel() const {
    return cbChannel.getSelectedId();
}

void StepGridComponent::triggerMidiActivity() {
    midiActivityIntensity = 1.0f;
    repaint();
}

void StepGridComponent::setPhrase(const std::vector<Step>& steps, const GeneratorConfig& config) {
    phrase = steps;
    currentConfig = config;
    repaint();
}

void StepGridComponent::setCurrentStep(int stepIndex) {
    if (playingStep != stepIndex) {
        playingStep = stepIndex;
        repaint();
    }
}

void StepGridComponent::setMuted(bool mute) {
    isMute = mute;
    btnMute.setToggleState(mute, juce::dontSendNotification);
    repaint();
}

juce::Rectangle<int> StepGridComponent::getGridBounds() const {
    return getLocalBounds().withTrimmedTop(34).reduced(6, 4);
}

int StepGridComponent::getStepIndexAtPoint(juce::Point<int> pt) const {
    auto grid = getGridBounds();
    if (!grid.contains(pt) || phrase.empty()) return -1;

    int n = (int)phrase.size();
    float stepW = (float)grid.getWidth() / (float)n;
    int idx = (int)((float)(pt.x - grid.getX()) / stepW);
    return juce::jlimit(0, n - 1, idx);
}

void StepGridComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    // Lane container background
    g.setColour(juce::Colour(0xff121824));
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(isMute ? juce::Colour(0xff4a3535) : CustomLookAndFeel::getLineBorder());
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

    // Header Title
    g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
    g.setColour(isMute ? juce::Colour(0xff735555) : laneAccent);
    g.drawText(title + (isMute ? " (MUTED)" : ""), 12, 6, 140, 24, juce::Justification::centredLeft);

    // MIDI Activity LED
    float ledX = 156.0f;
    float ledY = 14.0f;
    juce::Colour ledCol = isMute ? juce::Colour(0xff4a3535)
                         : (midiActivityIntensity > 0.05f ? laneAccent.interpolatedWith(juce::Colours::white, midiActivityIntensity)
                                                          : laneAccent.withAlpha(0.25f));
    g.setColour(ledCol);
    g.fillEllipse(ledX, ledY, 8.0f, 8.0f);
    if (midiActivityIntensity > 0.05f) {
        g.setColour(laneAccent.withAlpha(midiActivityIntensity * 0.45f));
        g.fillEllipse(ledX - 3.0f, ledY - 3.0f, 14.0f, 14.0f);
        midiActivityIntensity = std::max(0.0f, midiActivityIntensity - 0.15f);
    }

    // Draw Steps
    auto grid = getGridBounds();
    if (phrase.empty()) return;

    int n = (int)phrase.size();
    float gap = 3.0f;
    float totalW = (float)grid.getWidth();
    float stepW = (totalW - gap * (float)(n - 1)) / (float)n;
    float stepH = (float)grid.getHeight();

    for (int i = 0; i < n; ++i) {
        float x = (float)grid.getX() + (float)i * (stepW + gap);
        float y = (float)grid.getY();
        auto cellBounds = juce::Rectangle<float>(x, y, stepW, stepH);

        const auto& s = phrase[(size_t)i];
        bool isPlaying = (playingStep == i);

        // Cell background
        juce::Colour cellBg;
        if (isPlaying) {
            cellBg = s.active ? juce::Colour(0xff1f3d54) : juce::Colour(0xff1c2331);
        } else if (s.active) {
            cellBg = juce::Colour(0xff182233);
        } else {
            cellBg = juce::Colour(0xff0f141d);
        }

        g.setColour(cellBg);
        g.fillRoundedRectangle(cellBounds, 6.0f);

        // Border
        juce::Colour cellBorder;
        float borderWidth = 1.0f;
        if (isPlaying) {
            cellBorder = juce::Colours::white;
            borderWidth = 2.0f;
        } else if (s.active) {
            cellBorder = laneAccent.withAlpha(0.6f);
        } else {
            cellBorder = juce::Colour(0xff222d3d);
        }
        g.setColour(cellBorder);
        g.drawRoundedRectangle(cellBounds, 6.0f, borderWidth);

        // Step number
        g.setFont(juce::FontOptions(10.0f));
        g.setColour(CustomLookAndFeel::getMutedText());
        g.drawText(juce::String(i + 1), (int)x, (int)y + 3, (int)stepW, 12, juce::Justification::centred);

        // Pitch name
        if (s.active) {
            g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
            g.setColour(isPlaying ? juce::Colours::white : CustomLookAndFeel::getActiveText());
            g.drawText(GeneratorEngine::midiToName(s.getEffectiveMidiNote()),
                       (int)x, (int)y + (int)(stepH * 0.28f), (int)stepW, 16, juce::Justification::centred);

            // Tags: ACC / SLD
            int tagY = (int)y + (int)(stepH * 0.52f);
            if (s.accent) {
                g.setColour(CustomLookAndFeel::getNeonOrange());
                g.fillRoundedRectangle(x + 3.0f, (float)tagY, (stepW - 6.0f) * 0.45f, 9.0f, 2.0f);
                g.setFont(juce::FontOptions(7.5f, juce::Font::bold));
                g.setColour(juce::Colours::black);
                g.drawText("ACC", (int)(x + 3.0f), tagY, (int)((stepW - 6.0f) * 0.45f), 9, juce::Justification::centred);
            }
            if (s.slide) {
                float sldX = s.accent ? (x + stepW * 0.5f) : (x + 3.0f);
                g.setColour(CustomLookAndFeel::getNeonCyan());
                g.fillRoundedRectangle(sldX, (float)tagY, (stepW - 6.0f) * 0.45f, 9.0f, 2.0f);
                g.setFont(juce::FontOptions(7.5f, juce::Font::bold));
                g.setColour(juce::Colours::black);
                g.drawText("SLD", (int)sldX, tagY, (int)((stepW - 6.0f) * 0.45f), 9, juce::Justification::centred);
            }

            // Gate bar at bottom
            float gateBarW = juce::jlimit(2.0f, stepW - 6.0f, (stepW - 6.0f) * std::min(1.0f, s.gate));
            float barY = y + stepH - 8.0f;
            g.setColour(laneAccent);
            g.fillRoundedRectangle(x + 3.0f, barY, gateBarW, 3.5f, 1.5f);
        } else {
            g.setFont(juce::FontOptions(11.0f));
            g.setColour(juce::Colour(0xff3a4c66));
            g.drawText("-", (int)x, (int)y + (int)(stepH * 0.35f), (int)stepW, 16, juce::Justification::centred);
        }
    }
}

void StepGridComponent::resized() {
    auto bounds = getLocalBounds();
    auto header = bounds.removeFromTop(32);

    // Left title and channel dropdown
    cbChannel.setBounds(172, 4, 120, 24);

    // Right buttons
    auto rightArea = header.withTrimmedLeft(300).withTrimmedRight(8);

    btnMute.setBounds(rightArea.removeFromRight(54).reduced(0, 3));
    rightArea.removeFromRight(6);

    btnShiftRight.setBounds(rightArea.removeFromRight(26).reduced(0, 3));
    btnShiftLeft.setBounds(rightArea.removeFromRight(26).reduced(0, 3));
    rightArea.removeFromRight(6);

    btnOctUp.setBounds(rightArea.removeFromRight(42).reduced(0, 3));
    btnOctDown.setBounds(rightArea.removeFromRight(42).reduced(0, 3));
    rightArea.removeFromRight(6);

    btnReverse.setBounds(rightArea.removeFromRight(38).reduced(0, 3));
    btnInvert.setBounds(rightArea.removeFromRight(38).reduced(0, 3));
}

void StepGridComponent::mouseDown(const juce::MouseEvent& e) {
    int idx = getStepIndexAtPoint(e.getPosition());
    if (idx < 0 || idx >= (int)phrase.size()) return;

    if (e.mods.isRightButtonDown()) {
        // Right click: toggle accent
        if (phrase[(size_t)idx].active) {
            phrase[(size_t)idx].accent = !phrase[(size_t)idx].accent;
            if (onPhraseChanged) onPhraseChanged();
            repaint();
        }
    } else if (e.mods.isAltDown()) {
        // Alt click: toggle slide
        if (phrase[(size_t)idx].active) {
            phrase[(size_t)idx].slide = !phrase[(size_t)idx].slide;
            if (onPhraseChanged) onPhraseChanged();
            repaint();
        }
    } else {
        draggedStepIndex = idx;
        dragStartY = (float)e.y;
        dragStartDegree = phrase[(size_t)idx].degree;
    }
}

void StepGridComponent::mouseDoubleClick(const juce::MouseEvent& e) {
    int idx = getStepIndexAtPoint(e.getPosition());
    if (idx < 0 || idx >= (int)phrase.size()) return;

    // Double-click: toggle active (add or delete note)
    phrase[(size_t)idx].active = !phrase[(size_t)idx].active;
    if (phrase[(size_t)idx].active) {
        if (phrase[(size_t)idx].gate <= 0.05f) phrase[(size_t)idx].gate = 0.82f;
        phrase[(size_t)idx].midiNote = GeneratorEngine::degreeToMidi(phrase[(size_t)idx].degree, currentConfig);
    }
    if (onPhraseChanged) onPhraseChanged();
    repaint();
}

void StepGridComponent::mouseDrag(const juce::MouseEvent& e) {
    if (draggedStepIndex >= 0 && draggedStepIndex < (int)phrase.size()) {
        // Horizontal drag -> move note between steps left and right
        int currentIdx = getStepIndexAtPoint(e.getPosition());
        if (currentIdx >= 0 && currentIdx < (int)phrase.size() && currentIdx != draggedStepIndex) {
            std::swap(phrase[(size_t)draggedStepIndex], phrase[(size_t)currentIdx]);
            draggedStepIndex = currentIdx;
            if (onPhraseChanged) onPhraseChanged();
            repaint();
        }

        // Vertical drag -> adjust pitch/degree up and down
        if (draggedStepIndex >= 0 && phrase[(size_t)draggedStepIndex].active) {
            float dy = dragStartY - (float)e.y;
            int degreeDelta = (int)(dy / 10.0f);
            int newDeg = juce::jlimit(0, 11, dragStartDegree + degreeDelta);
            if (phrase[(size_t)draggedStepIndex].degree != newDeg) {
                phrase[(size_t)draggedStepIndex].degree = newDeg;
                phrase[(size_t)draggedStepIndex].midiNote = GeneratorEngine::degreeToMidi(newDeg, currentConfig);
                if (onPhraseChanged) onPhraseChanged();
                repaint();
            }
        }
    }
}

void StepGridComponent::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) {
    int idx = getStepIndexAtPoint(e.getPosition());
    if (idx >= 0 && idx < (int)phrase.size() && phrase[(size_t)idx].active) {
        int delta = (wheel.deltaY > 0.0f) ? 1 : -1;
        int newDeg = juce::jlimit(0, 11, phrase[(size_t)idx].degree + delta);
        phrase[(size_t)idx].degree = newDeg;
        phrase[(size_t)idx].midiNote = GeneratorEngine::degreeToMidi(newDeg, currentConfig);
        if (onPhraseChanged) onPhraseChanged();
        repaint();
    }
}

} // namespace CR
