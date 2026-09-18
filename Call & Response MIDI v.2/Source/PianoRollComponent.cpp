#include "PianoRollComponent.h"
#include "CustomLookAndFeel.h"

namespace CR {

PianoRollComponent::PianoRollComponent() {
    auto setupTab = [this](juce::TextButton& btn, ViewLane lane) {
        btn.setClickingTogglesState (true);
        btn.setRadioGroupId (1001);
        addAndMakeVisible (btn);
        btn.onClick = [this, lane] {
            setViewLane (lane);
        };
    };

    setupTab (btnCallTab,     ViewLane::Call);
    setupTab (btnResponseTab, ViewLane::Response);
    setupTab (btnBothTab,     ViewLane::Both);

    btnCallTab.setColour (juce::TextButton::textColourOnId, CustomLookAndFeel::getNeonGreen());
    btnCallTab.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff1b2d1c));

    btnResponseTab.setColour (juce::TextButton::textColourOnId, juce::Colour (0xffaa80f5));
    btnResponseTab.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff281b3d));

    btnBothTab.setColour (juce::TextButton::textColourOnId, CustomLookAndFeel::getNeonCyan());
    btnBothTab.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff193a4d));

    currentView = ViewLane::Both;
    btnBothTab.setToggleState (true, juce::dontSendNotification);
}

PianoRollComponent::~PianoRollComponent() {
    if (auditioningMidi >= 0 && onAuditionNote) {
        onAuditionNote (auditioningMidi, false);
        auditioningMidi = -1;
    }
    btnCallTab.onClick = nullptr;
    btnResponseTab.onClick = nullptr;
    btnBothTab.onClick = nullptr;
    onPhraseEdited = nullptr;
    onEditGestureStarted = nullptr;
    onAuditionNote = nullptr;
}

void PianoRollComponent::setPhrases(const std::vector<Step>& call, const std::vector<Step>& resp, const GeneratorConfig& config) {
    currentConfig = config;
    if (currentDragAction != DragAction::MoveNote && currentDragAction != DragAction::ResizeNoteGate) {
        callPhrase = call;
        responsePhrase = resp;
        updateAutoRange();
        repaint();
    }
}

void PianoRollComponent::setViewLane(ViewLane lane) {
    currentView = lane;
    btnCallTab.setToggleState     (lane == ViewLane::Call, juce::dontSendNotification);
    btnResponseTab.setToggleState (lane == ViewLane::Response, juce::dontSendNotification);
    btnBothTab.setToggleState     (lane == ViewLane::Both, juce::dontSendNotification);
    updateAutoRange();
    if (onViewLaneChanged) onViewLaneChanged(lane);
    repaint();
}

void PianoRollComponent::setCurrentStep(int stepIndex, int phase) {
    if (playingStep != stepIndex || playingPhase != phase) {
        playingStep = stepIndex;
        playingPhase = phase;
        repaint();
    }
}

void PianoRollComponent::updateAutoRange() {
    int lowest = 127, highest = 0;
    auto checkPhrase = [&](const std::vector<Step>& phrase) {
        for (const auto& s : phrase) {
            if (s.active) {
                int note = s.getEffectiveMidiNote();
                lowest = std::min(lowest, note);
                highest = std::max(highest, note);
            }
        }
    };

    if (currentView == ViewLane::Call || currentView == ViewLane::Both) checkPhrase(callPhrase);
    if (currentView == ViewLane::Response || currentView == ViewLane::Both) checkPhrase(responsePhrase);

    if (lowest <= highest) {
        minMidi = std::max(12, (lowest / 12) * 12 - 5);
        maxMidi = std::min(108, ((highest + 11) / 12) * 12 + 5);
    } else {
        minMidi = 36; // C2
        maxMidi = 72; // C5
    }
    minMidi = std::min(minMidi, maxMidi - 12);
}

juce::Rectangle<int> PianoRollComponent::getGridArea() const {
    return getLocalBounds().withTrimmedTop(headerHeight).withTrimmedLeft(pianoKeyWidth).reduced(4, 4);
}

juce::Rectangle<int> PianoRollComponent::getKeyboardArea() const {
    auto grid = getGridArea();
    return juce::Rectangle<int>(4, grid.getY(), pianoKeyWidth, grid.getHeight());
}

float PianoRollComponent::getKeyHeight() const {
    int totalNotes = std::max(1, maxMidi - minMidi + 1);
    return (float)getGridArea().getHeight() / (float)totalNotes;
}

float PianoRollComponent::getStepWidth(int totalSteps) const {
    return (float)getGridArea().getWidth() / (float)std::max(1, totalSteps);
}

int PianoRollComponent::midiNoteAtY(int y) const {
    auto grid = getGridArea();
    if (y < grid.getY() || y > grid.getBottom()) return -1;
    float rowH = getKeyHeight();
    int row = (int)((float)(y - grid.getY()) / rowH);
    int midi = maxMidi - row;
    return juce::jlimit(minMidi, maxMidi, midi);
}

float PianoRollComponent::yForMidiNote(int midi) const {
    auto grid = getGridArea();
    float rowH = getKeyHeight();
    int row = maxMidi - midi;
    return (float)grid.getY() + (float)row * rowH;
}

int PianoRollComponent::stepAtX(int x, int totalSteps) const {
    auto grid = getGridArea();
    if (x < grid.getX() || x > grid.getRight()) return -1;
    float stepW = getStepWidth(totalSteps);
    int step = (int)((float)(x - grid.getX()) / stepW);
    return juce::jlimit(0, totalSteps - 1, step);
}

bool PianoRollComponent::isBlackKey(int midi) const {
    int note = (midi % 12 + 12) % 12;
    return (note == 1 || note == 3 || note == 6 || note == 8 || note == 10);
}

bool PianoRollComponent::isInCurrentScale(int midi) const {
    const auto& intervals = GeneratorEngine::getScaleIntervals(currentConfig.scaleIndex);
    int root = currentConfig.keyIndex;
    int note = (midi % 12 + 12) % 12;
    int diff = (note - root + 12) % 12;
    return std::find(intervals.begin(), intervals.end(), diff) != intervals.end();
}

void PianoRollComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    // Background container
    g.setColour(juce::Colour(0xff0f141d));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(CustomLookAndFeel::getLineBorder());
    g.drawRoundedRectangle(bounds, 8.0f, 1.0f);

    // Header bar
    auto headerBounds = bounds.removeFromTop((float)headerHeight);
    g.setColour(CustomLookAndFeel::getPanelBg());
    g.fillRoundedRectangle(headerBounds.getX(), headerBounds.getY(), headerBounds.getWidth(), headerBounds.getHeight(), 6.0f);

    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.setColour(CustomLookAndFeel::getActiveText());
    g.drawText("PIANO ROLL MELODY EDITOR", 12, 4, 220, headerHeight - 8, juce::Justification::centredLeft);

    auto grid = getGridArea();
    auto kb = getKeyboardArea();
    float rowH = getKeyHeight();
    int totalNotes = maxMidi - minMidi + 1;

    int totalSteps = (currentView == ViewLane::Both) ? (int)(callPhrase.size() + responsePhrase.size())
                   : ((currentView == ViewLane::Response) ? (int)responsePhrase.size() : (int)callPhrase.size());
    totalSteps = std::max(1, totalSteps);
    float stepW = getStepWidth(totalSteps);

    // 1. Draw Piano Keyboard on the left
    for (int i = 0; i < totalNotes; ++i) {
        int midi = maxMidi - i;
        float y = (float)kb.getY() + (float)i * rowH;
        bool black = isBlackKey(midi);
        bool inScale = isInCurrentScale(midi);
        bool isAuditioned = (auditioningMidi == midi);

        juce::Rectangle<float> keyRect((float)kb.getX(), y, (float)kb.getWidth(), rowH);

        // Key color
        juce::Colour keyCol;
        if (isAuditioned) {
            keyCol = CustomLookAndFeel::getNeonCyan();
        } else if (black) {
            keyCol = juce::Colour(0xff161b24);
        } else {
            keyCol = inScale ? juce::Colour(0xff2d3c52) : juce::Colour(0xff212b3a);
        }

        g.setColour(keyCol);
        g.fillRect(keyRect);

        g.setColour(juce::Colour(0xff10151f));
        g.drawRect(keyRect, 0.5f);

        // Note labels on C notes
        if (midi % 12 == 0) {
            g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
            g.setColour(juce::Colours::white);
            g.drawText("C" + juce::String(midi / 12 - 1), keyRect.withTrimmedRight(4.0f), juce::Justification::centredRight);
        }
    }

    // 2. Draw Horizontal Grid Rows (pitch lanes)
    for (int i = 0; i < totalNotes; ++i) {
        int midi = maxMidi - i;
        float y = (float)grid.getY() + (float)i * rowH;
        bool black = isBlackKey(midi);
        bool inScale = isInCurrentScale(midi);

        juce::Colour rowBg = inScale ? (black ? juce::Colour(0xff131b26) : juce::Colour(0xff16202d))
                                     : (black ? juce::Colour(0xff0d1219) : juce::Colour(0xff101620));

        g.setColour(rowBg);
        g.fillRect((float)grid.getX(), y, (float)grid.getWidth(), rowH);

        // Subtle divider line
        g.setColour(juce::Colour(0xff1e2938));
        g.drawHorizontalLine((int)y, (float)grid.getX(), (float)grid.getRight());
    }

    // 3. Draw Vertical Step Columns & Beat Dividers
    for (int s = 0; s <= totalSteps; ++s) {
        float x = (float)grid.getX() + (float)s * stepW;
        bool isBar = (s % 16 == 0);
        bool isBeat = (s % 4 == 0);

        juce::Colour lineCol = isBar ? juce::Colour(0xff455a7a)
                             : (isBeat ? juce::Colour(0xff2e3e54) : juce::Colour(0xff1a2330));
        float lineWidth = isBar ? 1.5f : 1.0f;

        g.setColour(lineCol);
        g.drawLine(x, (float)grid.getY(), x, (float)grid.getBottom(), lineWidth);
    }

    // Helper to draw notes of a phrase
    auto drawPhraseNotes = [&](const std::vector<Step>& phrase, int startStepOffset, juce::Colour col, bool isCall) {
        juce::ignoreUnused(isCall);
        for (size_t i = 0; i < phrase.size(); ++i) {
            const auto& step = phrase[i];
            if (!step.active) continue;

            int stepIdx = (int)i + startStepOffset;
            int noteNum = step.getEffectiveMidiNote();
            if (noteNum < minMidi || noteNum > maxMidi) continue;

            float x = (float)grid.getX() + (float)stepIdx * stepW + 1.0f;
            float y = yForMidiNote(noteNum) + 1.0f;
            float noteW = std::max(6.0f, (stepW * step.gate) - 2.0f);
            float noteH = rowH - 2.0f;

            auto noteRect = juce::Rectangle<float>(x, y, noteW, noteH);

            // Note body
            juce::Colour bodyCol = step.accent ? col.brighter(0.3f) : col;
            g.setColour(bodyCol.withAlpha(0.9f));
            g.fillRoundedRectangle(noteRect, 3.5f);

            // Border
            g.setColour(step.accent ? juce::Colours::white : bodyCol.brighter(0.4f));
            g.drawRoundedRectangle(noteRect, 3.5f, step.accent ? 1.5f : 1.0f);

            // Accent indicator
            if (step.accent) {
                g.setColour(juce::Colours::white);
                g.fillEllipse(x + 3.0f, y + noteH * 0.5f - 2.5f, 5.0f, 5.0f);
            }

            // Slide indicator (diagonal tail matching mockups)
            if (step.slide) {
                g.setColour (CustomLookAndFeel::getNeonCyan());
                g.drawLine (x + noteW - 2.0f, y + noteH - 1.0f, x + noteW + 5.0f, y + noteH + 4.0f, 2.2f);
            }

            // Pitch degree text inside note if wide enough
            if (noteW > 16.0f) {
                g.setFont(juce::FontOptions(std::min(10.0f, rowH * 0.75f), juce::Font::bold));
                g.setColour(isCall ? juce::Colours::black : juce::Colours::white);
                g.drawText(GeneratorEngine::midiToName(noteNum), noteRect.reduced(3.0f, 0.0f), juce::Justification::centredLeft);
            }
        }
    };

    // 4. Render Notes (matching the exact colors of their corresponding lines)
    if (currentView == ViewLane::Call) {
        drawPhraseNotes(callPhrase, 0, CustomLookAndFeel::getNeonGreen(), true);
    } else if (currentView == ViewLane::Response) {
        drawPhraseNotes(responsePhrase, 0, CustomLookAndFeel::getNeonViolet(), false);
    } else { // Both split
        int callLen = (int)callPhrase.size();
        drawPhraseNotes(callPhrase, 0, CustomLookAndFeel::getNeonGreen(), true);
        drawPhraseNotes(responsePhrase, callLen, CustomLookAndFeel::getNeonViolet(), false);
    }

    // 5. Draw Animated Playhead
    if (playingStep >= 0) {
        int currentGlobalStep = (currentView == ViewLane::Both)
                              ? (playingPhase == 0 ? playingStep : ((int)callPhrase.size() + playingStep))
                              : playingStep;

        if (currentGlobalStep < totalSteps) {
            float playheadX = (float)grid.getX() + (float)currentGlobalStep * stepW;

            // Playhead bar with soft glow
            g.setColour(juce::Colours::white.withAlpha(0.2f));
            g.fillRect(playheadX, (float)grid.getY(), stepW, (float)grid.getHeight());

            g.setColour(juce::Colours::white);
            g.drawLine(playheadX, (float)grid.getY(), playheadX, (float)grid.getBottom(), 2.0f);

            // Playhead pointer triangle at top
            juce::Path triangle;
            triangle.addTriangle(playheadX - 5.0f, (float)grid.getY(),
                                 playheadX + 5.0f, (float)grid.getY(),
                                 playheadX, (float)grid.getY() + 8.0f);
            g.fillPath(triangle);
        }
    }
}

void PianoRollComponent::resized() {
    auto header = getLocalBounds().removeFromTop(headerHeight).reduced(4, 4);

    // Tabs on the right side of header
    auto tabArea = header.removeFromRight(260);
    btnCallTab.setBounds(tabArea.removeFromLeft(75).reduced(2));
    btnResponseTab.setBounds(tabArea.removeFromLeft(95).reduced(2));
    btnBothTab.setBounds(tabArea.removeFromLeft(90).reduced(2));
}

PianoRollComponent::NoteHitResult PianoRollComponent::hitTestNote(int mouseX, int mouseY) const {
    NoteHitResult result;
    auto grid = getGridArea();
    if (!grid.contains(mouseX, mouseY)) return result;

    float rowH = getKeyHeight();
    int totalSteps = (currentView == ViewLane::Both) ? (int)(callPhrase.size() + responsePhrase.size())
                   : ((currentView == ViewLane::Response) ? (int)responsePhrase.size() : (int)callPhrase.size());
    totalSteps = std::max(1, totalSteps);
    float stepW = getStepWidth(totalSteps);

    auto testPhrase = [&](const std::vector<Step>& phrase, int startStepOffset, bool isCall) -> bool {
        for (size_t i = 0; i < phrase.size(); ++i) {
            const auto& step = phrase[i];
            if (!step.active) continue;

            int stepIdx = (int)i + startStepOffset;
            int noteNum = step.getEffectiveMidiNote();
            if (noteNum < minMidi || noteNum > maxMidi) continue;

            float x = (float)grid.getX() + (float)stepIdx * stepW + 1.0f;
            float y = yForMidiNote(noteNum) + 1.0f;
            float noteW = std::max(6.0f, (stepW * step.gate) - 2.0f);
            float noteH = rowH - 2.0f;

            juce::Rectangle<float> rect(x, y, noteW, noteH);

            if ((float)mouseY >= y - 1.0f && (float)mouseY <= y + noteH + 1.0f) {
                float rightEdge = x + noteW;
                // Handle within 7px of right edge
                if (std::abs((float)mouseX - rightEdge) <= 7.0f) {
                    result.hit = true;
                    result.isResizeHandle = true;
                    result.isCall = isCall;
                    result.stepIndex = (int)i;
                    result.midiNote = noteNum;
                    result.noteRect = rect;
                    return true;
                }
                // Note body
                if ((float)mouseX >= x && (float)mouseX < rightEdge) {
                    result.hit = true;
                    result.isResizeHandle = false;
                    result.isCall = isCall;
                    result.stepIndex = (int)i;
                    result.midiNote = noteNum;
                    result.noteRect = rect;
                    return true;
                }
            }
        }
        return false;
    };

    if (currentView == ViewLane::Call) {
        testPhrase(callPhrase, 0, true);
    } else if (currentView == ViewLane::Response) {
        testPhrase(responsePhrase, 0, false);
    } else {
        if (!testPhrase(callPhrase, 0, true)) {
            testPhrase(responsePhrase, (int)callPhrase.size(), false);
        }
    }

    return result;
}

void PianoRollComponent::mouseDown(const juce::MouseEvent& e) {
    auto kb = getKeyboardArea();
    auto grid = getGridArea();

    // 1. Click on Piano Keyboard -> audition pitch
    if (kb.contains(e.getPosition())) {
        int midi = midiNoteAtY(e.y);
        if (midi >= 0) {
            auditioningMidi = midi;
            currentDragAction = DragAction::PianoKeyAudition;
            if (onAuditionNote) onAuditionNote(midi, true);
            repaint();
        }
        return;
    }

    // 2. Click in Grid Area
    if (!grid.contains(e.getPosition())) return;

    auto hit = hitTestNote(e.x, e.y);
    if (hit.hit) {
        auto& phrase = hit.isCall ? callPhrase : responsePhrase;
        auto& s = phrase[(size_t)hit.stepIndex];

        if (e.mods.isRightButtonDown()) {
            if (onEditGestureStarted) onEditGestureStarted();
            s.accent = !s.accent;
            if (onPhraseEdited) onPhraseEdited(phrase, hit.isCall);
            repaint();
            return;
        }
        if (e.mods.isAltDown()) {
            if (onEditGestureStarted) onEditGestureStarted();
            s.slide = !s.slide;
            if (onPhraseEdited) onPhraseEdited(phrase, hit.isCall);
            repaint();
            return;
        }
        if (e.mods.isShiftDown()) {
            if (onEditGestureStarted) onEditGestureStarted();
            s.active = false;
            if (onPhraseEdited) onPhraseEdited(phrase, hit.isCall);
            repaint();
            return;
        }

        if (hit.isResizeHandle) {
            // Resize gate
            if (onEditGestureStarted) onEditGestureStarted();
            currentDragAction = DragAction::ResizeNoteGate;
            draggedStep = hit.stepIndex;
            draggedIsCall = hit.isCall;
            dragStartGate = s.gate;
            return;
        } else {
            // 2D Note Move
            if (onEditGestureStarted) onEditGestureStarted();
            currentDragAction = DragAction::MoveNote;
            draggedStep = hit.stepIndex;
            dragStartStep = hit.stepIndex;
            draggedIsCall = hit.isCall;
            dragStartY = (float)e.y;
            dragStartMidi = s.getEffectiveMidiNote();
            dragInitialPhrase = phrase;
            auditioningMidi = hit.midiNote;
            if (onAuditionNote) onAuditionNote(hit.midiNote, true);
            return;
        }
    }

    // Single-click on empty cell -> audition row pitch without modifying note
    int midi = midiNoteAtY(e.y);
    if (midi >= 0) {
        auditioningMidi = midi;
        currentDragAction = DragAction::None;
        if (onAuditionNote) onAuditionNote(midi, true);
    }
}

void PianoRollComponent::mouseDoubleClick(const juce::MouseEvent& e) {
    auto grid = getGridArea();
    if (!grid.contains(e.getPosition())) return;

    auto hit = hitTestNote(e.x, e.y);
    if (hit.hit) {
        // Double-click on existing note -> delete note
        if (onEditGestureStarted) onEditGestureStarted();
        auto& phrase = hit.isCall ? callPhrase : responsePhrase;
        phrase[(size_t)hit.stepIndex].active = false;
        if (auditioningMidi >= 0 && onAuditionNote) {
            onAuditionNote(auditioningMidi, false);
            auditioningMidi = -1;
        }
        if (onPhraseEdited) onPhraseEdited(phrase, hit.isCall);
        updateAutoRange();
        repaint();
        return;
    }

    // Double click on empty space -> add note
    int totalSteps = (currentView == ViewLane::Both) ? (int)(callPhrase.size() + responsePhrase.size())
                   : ((currentView == ViewLane::Response) ? (int)responsePhrase.size() : (int)callPhrase.size());
    totalSteps = std::max(1, totalSteps);

    int stepIdx = stepAtX(e.x, totalSteps);
    int midi = midiNoteAtY(e.y);
    if (stepIdx < 0 || midi < 0) return;

    bool isCallTarget = true;
    int localStep = stepIdx;
    if (currentView == ViewLane::Response) {
        isCallTarget = false;
    } else if (currentView == ViewLane::Both) {
        if (stepIdx >= (int)callPhrase.size()) {
            isCallTarget = false;
            localStep = stepIdx - (int)callPhrase.size();
        }
    }

    auto& phrase = isCallTarget ? callPhrase : responsePhrase;
    if (localStep >= (int)phrase.size()) return;

    if (onEditGestureStarted) onEditGestureStarted();
    auto& s = phrase[(size_t)localStep];
    s.active = true;
    s.midiNote = midi;
    s.octaveOffset = 0;
    s.degree = GeneratorEngine::midiToDegree(midi, currentConfig);
    s.gate = 0.82f;
    s.accent = false;
    s.slide = false;

    auditioningMidi = midi;
    if (onAuditionNote) onAuditionNote(midi, true);
    if (onPhraseEdited) onPhraseEdited(phrase, isCallTarget);
    updateAutoRange();
    repaint();
}

void PianoRollComponent::mouseDrag(const juce::MouseEvent& e) {
    if (currentDragAction == DragAction::PianoKeyAudition) {
        int midi = midiNoteAtY(e.y);
        if (midi >= 0 && midi != auditioningMidi) {
            if (auditioningMidi >= 0 && onAuditionNote) onAuditionNote(auditioningMidi, false);
            auditioningMidi = midi;
            if (onAuditionNote) onAuditionNote(midi, true);
            repaint();
        }
    } else if (currentDragAction == DragAction::MoveNote) {
        int totalSteps = (currentView == ViewLane::Both) ? (int)(callPhrase.size() + responsePhrase.size())
                       : ((currentView == ViewLane::Response) ? (int)responsePhrase.size() : (int)callPhrase.size());
        totalSteps = std::max(1, totalSteps);

        int newMidi = midiNoteAtY(e.y);
        int newStepIdx = stepAtX(e.x, totalSteps);

        if (newMidi >= 0 && newStepIdx >= 0 && dragStartStep >= 0 && !dragInitialPhrase.empty()) {
            int newLocalStep = newStepIdx;
            if (currentView == ViewLane::Both) {
                if (draggedIsCall) {
                    newLocalStep = juce::jlimit(0, (int)callPhrase.size() - 1, newStepIdx);
                } else {
                    newLocalStep = juce::jlimit(0, (int)responsePhrase.size() - 1, newStepIdx - (int)callPhrase.size());
                }
            } else {
                auto& ph = draggedIsCall ? callPhrase : responsePhrase;
                newLocalStep = juce::jlimit(0, (int)ph.size() - 1, newStepIdx);
            }

            auto& phrase = draggedIsCall ? callPhrase : responsePhrase;
            if (dragStartStep < (int)phrase.size() && newLocalStep < (int)phrase.size() &&
                dragStartStep < (int)dragInitialPhrase.size() && newLocalStep < (int)dragInitialPhrase.size()) {

                // Reset from snapshot so intermediate steps are never scrambled
                phrase = dragInitialPhrase;

                Step movingNote = dragInitialPhrase[(size_t)dragStartStep];
                movingNote.active = true;
                movingNote.midiNote = newMidi;
                movingNote.octaveOffset = 0;
                movingNote.degree = GeneratorEngine::midiToDegree(newMidi, currentConfig);

                if (newLocalStep == dragStartStep) {
                    // Vertical drag only (pitch transposition)
                    phrase[(size_t)dragStartStep] = movingNote;
                } else {
                    // Horizontal move (or 2D move): clear source position
                    phrase[(size_t)dragStartStep].active = false;
                    if (dragInitialPhrase[(size_t)newLocalStep].active) {
                        // Clean swap with destination note if one existed
                        phrase[(size_t)dragStartStep] = dragInitialPhrase[(size_t)newLocalStep];
                    }
                    phrase[(size_t)newLocalStep] = movingNote;
                }
                draggedStep = newLocalStep;

                if (newMidi != auditioningMidi) {
                    if (auditioningMidi >= 0 && onAuditionNote) onAuditionNote(auditioningMidi, false);
                    auditioningMidi = newMidi;
                    if (onAuditionNote) onAuditionNote(newMidi, true);
                }

                if (onPhraseEdited) onPhraseEdited(phrase, draggedIsCall);
                repaint();
            }
        }
    } else if (currentDragAction == DragAction::ResizeNoteGate) {
        if (draggedStep >= 0) {
            int totalSteps = (currentView == ViewLane::Both) ? (int)(callPhrase.size() + responsePhrase.size())
                           : ((currentView == ViewLane::Response) ? (int)responsePhrase.size() : (int)callPhrase.size());
            totalSteps = std::max(1, totalSteps);
            float stepW = getStepWidth(totalSteps);
            float deltaX = (float)e.getDistanceFromDragStartX();
            float newGate = juce::jlimit(0.15f, 4.0f, dragStartGate + (deltaX / stepW));

            auto& phrase = draggedIsCall ? callPhrase : responsePhrase;
            if (draggedStep < (int)phrase.size()) {
                phrase[(size_t)draggedStep].gate = newGate;
                if (onPhraseEdited) onPhraseEdited(phrase, draggedIsCall);
                repaint();
            }
        }
    }
}

void PianoRollComponent::mouseUp(const juce::MouseEvent&) {
    if (auditioningMidi >= 0) {
        if (onAuditionNote) onAuditionNote(auditioningMidi, false);
        auditioningMidi = -1;
    }
    bool wasMoving = (currentDragAction == DragAction::MoveNote);
    currentDragAction = DragAction::None;
    draggedStep = -1;
    dragStartStep = -1;
    dragInitialPhrase.clear();
    if (wasMoving) updateAutoRange();
    repaint();
}

void PianoRollComponent::mouseMove(const juce::MouseEvent& e) {
    auto hit = hitTestNote(e.x, e.y);
    if (hit.hit) {
        if (hit.isResizeHandle) {
            setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
            return;
        } else {
            setMouseCursor(juce::MouseCursor::UpDownLeftRightResizeCursor);
            return;
        }
    }
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

} // namespace CR
