#include "StepperComponent.h"
#include "CustomLookAndFeel.h"

namespace CR {

// ==============================================================================
// Sub-components: ArrowButton, ValueButton, PadlockButton
// ==============================================================================

class StepperComponent::ArrowButton : public juce::Button {
public:
    enum Direction { Left, Right };

    ArrowButton (Direction dir)
        : juce::Button ("Arrow"), direction (dir) {}

    void paintButton (juce::Graphics& g, bool isMouseOver, bool isButtonDown) override {
        auto bounds = getLocalBounds().toFloat();
        float w = bounds.getWidth();
        float h = bounds.getHeight();

        juce::Path p;
        float arrowSize = std::min (w, h) * 0.32f;
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();

        if (direction == Left) {
            p.addTriangle (cx + arrowSize * 0.45f, cy - arrowSize,
                           cx - arrowSize * 0.55f, cy,
                           cx + arrowSize * 0.45f, cy + arrowSize);
        } else {
            p.addTriangle (cx - arrowSize * 0.45f, cy - arrowSize,
                           cx + arrowSize * 0.55f, cy,
                           cx - arrowSize * 0.45f, cy + arrowSize);
        }

        juce::Colour col = isButtonDown ? juce::Colours::white
                         : (isMouseOver ? CustomLookAndFeel::getActiveText()
                                        : CustomLookAndFeel::getMutedText().withAlpha (0.7f));
        g.setColour (col);
        g.fillPath (p);
    }

private:
    Direction direction;
};

class StepperComponent::ValueButton : public juce::Button {
public:
    ValueButton (StepperComponent& o) : juce::Button ("Value"), owner (o) {
        setButtonText ("");
        setTooltip ("Click to open dropdown menu");
    }

    void mouseEnter (const juce::MouseEvent&) override {
        if (owner.continuousMode)
            setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
        else
            setMouseCursor (juce::MouseCursor::NormalCursor);
    }

    void mouseDown (const juce::MouseEvent& e) override {
        if (owner.continuousMode) {
            dragStartY = e.getPosition().y;
            dragStartVal = owner.currentValue;
            hasDragged = false;
        }
        juce::Button::mouseDown (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override {
        if (owner.continuousMode) {
            float dy = (float)(dragStartY - e.getPosition().y);
            if (std::abs (dy) > 1.5f) {
                hasDragged = true;
                double speed = (owner.maxValue - owner.minValue) / 140.0;
                if (e.mods.isShiftDown()) speed *= 0.2; // fine tuning with Shift
                double newVal = dragStartVal + (double)dy * speed;
                owner.setCurrentValue (newVal, juce::sendNotificationAsync);
            }
        }
        juce::Button::mouseDrag (e);
    }

    void mouseUp (const juce::MouseEvent& e) override {
        juce::Button::mouseUp (e);
        if (owner.continuousMode && !hasDragged && contains (e.getPosition())) {
            owner.showPopupMenu();
        }
    }

    void paintButton (juce::Graphics& g, bool isMouseOver, bool isButtonDown) override {
        auto bounds = getLocalBounds().toFloat();

        if (isMouseOver || isButtonDown) {
            g.setColour (juce::Colour (0xff2a282c));
            g.fillRoundedRectangle (bounds, 3.0f);
        }

        g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
        juce::Colour textCol = (isMouseOver || isButtonDown) ? juce::Colours::white
                                                             : CustomLookAndFeel::getActiveText();
        g.setColour (textCol);
        g.drawFittedText (getButtonText(), getLocalBounds(),
                          juce::Justification::centred, 1, 0.60f);
    }

private:
    StepperComponent& owner;
    int dragStartY = 0;
    double dragStartVal = 0.0;
    bool hasDragged = false;
};

class StepperComponent::PadlockButton : public juce::Button {
public:
    PadlockButton() : juce::Button ("Lock") {
        setClickingTogglesState (true);
        setTooltip ("Lock parameter from being rerolled");
    }

    void paintButton (juce::Graphics& g, bool isMouseOver, bool /*isButtonDown*/) override {
        auto bounds = getLocalBounds().toFloat();
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        bool locked = getToggleState();

        float bodyW = 9.0f;
        float bodyH = 7.5f;
        float bodyX = cx - bodyW * 0.5f;
        float bodyY = cy - 1.0f;

        juce::Colour accent = locked ? CustomLookAndFeel::getNeonGreen()
                                     : (isMouseOver ? CustomLookAndFeel::getActiveText()
                                                    : CustomLookAndFeel::getMutedText().withAlpha (0.4f));

        // Shackle
        juce::Path shackle;
        float shackleR = 3.5f;
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

// ==============================================================================
// StepperComponent Implementation
// ==============================================================================

StepperComponent::StepperComponent (const juce::String& labelText, LayoutMode mode)
    : layoutMode (mode), label (labelText) {

    btnLeft = std::make_unique<ArrowButton> (ArrowButton::Left);
    btnRight = std::make_unique<ArrowButton> (ArrowButton::Right);
    btnValue = std::make_unique<ValueButton> (*this);
    btnLock = std::make_unique<PadlockButton>();

    addAndMakeVisible (*btnLeft);
    addAndMakeVisible (*btnRight);
    addAndMakeVisible (*btnValue);
    addAndMakeVisible (*btnLock);

    btnLeft->onClick = [this] { stepPrev(); };
    btnRight->onClick = [this] { stepNext(); };
    btnValue->onClick = [this] {
        if (!continuousMode) showPopupMenu();
    };
    btnLock->onClick = [this] {
        if (onLockChanged) onLockChanged (btnLock->getToggleState());
    };
}

StepperComponent::~StepperComponent() {
    btnLeft->onClick = nullptr;
    btnRight->onClick = nullptr;
    btnValue->onClick = nullptr;
    btnLock->onClick = nullptr;
    onChange = nullptr;
    onValueChanged = nullptr;
    onLockChanged = nullptr;
}

void StepperComponent::setLayoutMode (LayoutMode mode) {
    layoutMode = mode;
    resized();
    repaint();
}

void StepperComponent::setLabelText (const juce::String& text) {
    label = text;
    repaint();
}

void StepperComponent::setItems (const std::vector<juce::String>& itemsList) {
    items = itemsList;
    continuousMode = false;
    if (selectedIndex >= (int)items.size())
        selectedIndex = std::max (0, (int)items.size() - 1);
    btnValue->setButtonText (getSelectedText());
    repaint();
}

void StepperComponent::setSelectedIndex (int index, juce::NotificationType notification) {
    if (items.empty()) return;
    int clamped = juce::jlimit (0, (int)items.size() - 1, index);
    if (clamped != selectedIndex || btnValue->getButtonText() != getSelectedText()) {
        selectedIndex = clamped;
        btnValue->setButtonText (getSelectedText());
        repaint();
        if (notification != juce::dontSendNotification && onChange) {
            onChange (selectedIndex);
        }
    }
}

juce::String StepperComponent::getSelectedText() const {
    if (selectedIndex >= 0 && selectedIndex < (int)items.size())
        return items[(size_t)selectedIndex];
    return {};
}

void StepperComponent::setContinuousRange (double minVal, double maxVal, double defaultVal, double step, const juce::String& suffix) {
    continuousMode = true;
    minValue = minVal;
    maxValue = maxVal;
    stepIncrement = step > 0.0 ? step : 1.0;
    valueSuffix = suffix;
    btnValue->setTooltip ("Click & drag up/down to adjust, or click for presets");
    setCurrentValue (defaultVal, juce::dontSendNotification);
}

void StepperComponent::setCurrentValue (double val, juce::NotificationType notification) {
    double clamped = juce::jlimit (minValue, maxValue, val);
    if (stepIncrement > 0.0) {
        clamped = minValue + std::round ((clamped - minValue) / stepIncrement) * stepIncrement;
        clamped = juce::jlimit (minValue, maxValue, clamped);
    }

    bool isFirstSet = (btnValue->getButtonText() == "Value" || btnValue->getButtonText().isEmpty() || currentValue < -90000.0);
    if (std::abs (clamped - currentValue) > 1e-5 || isFirstSet) {
        currentValue = clamped;
        juce::String text;
        if (std::floor (currentValue) == currentValue)
            text = juce::String ((int)currentValue) + valueSuffix;
        else
            text = juce::String (currentValue, 1) + valueSuffix;

        btnValue->setButtonText (text);
        repaint();

        if (notification != juce::dontSendNotification) {
            if (onValueChanged) onValueChanged (currentValue);
            if (onChange) onChange ((int)std::round (currentValue));
        }
    }
}

void StepperComponent::setContinuousPresets (const std::vector<std::pair<juce::String, double>>& presetsList) {
    presets = presetsList;
}

void StepperComponent::setLockable (bool canLock) {
    lockable = canLock;
    btnLock->setVisible (canLock);
    resized();
}

void StepperComponent::setLocked (bool isLocked, juce::NotificationType notification) {
    btnLock->setToggleState (isLocked, juce::dontSendNotification);
    btnLock->repaint();
    if (notification != juce::dontSendNotification && onLockChanged) {
        onLockChanged (isLocked);
    }
}

bool StepperComponent::isLocked() const {
    return btnLock->getToggleState();
}

void StepperComponent::stepPrev() {
    if (continuousMode) {
        setCurrentValue (currentValue - stepIncrement, juce::sendNotificationAsync);
        return;
    }
    if (items.empty()) return;
    int nextIdx = (selectedIndex - 1 + (int)items.size()) % (int)items.size();
    setSelectedIndex (nextIdx, juce::sendNotificationAsync);
}

void StepperComponent::stepNext() {
    if (continuousMode) {
        setCurrentValue (currentValue + stepIncrement, juce::sendNotificationAsync);
        return;
    }
    if (items.empty()) return;
    int nextIdx = (selectedIndex + 1) % (int)items.size();
    setSelectedIndex (nextIdx, juce::sendNotificationAsync);
}

void StepperComponent::mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) {
    if (wheel.deltaY > 0.05f) {
        if (continuousMode)
            setCurrentValue (currentValue + stepIncrement, juce::sendNotificationAsync);
        else
            stepPrev();
    } else if (wheel.deltaY < -0.05f) {
        if (continuousMode)
            setCurrentValue (currentValue - stepIncrement, juce::sendNotificationAsync);
        else
            stepNext();
    }
}

void StepperComponent::showPopupMenu() {
    if (continuousMode) {
        juce::PopupMenu menu;
        if (!presets.empty()) {
            for (size_t i = 0; i < presets.size(); ++i) {
                bool isTicked = std::abs (presets[i].second - currentValue) < (stepIncrement * 0.5);
                menu.addItem ((int)i + 1, presets[i].first, true, isTicked);
            }
        } else {
            int numPresets = 5;
            for (int i = 0; i < numPresets; ++i) {
                double val = minValue + (maxValue - minValue) * ((double)i / (double)(numPresets - 1));
                juce::String name = (std::floor (val) == val ? juce::String ((int)val) : juce::String (val, 1)) + valueSuffix;
                bool isTicked = std::abs (val - currentValue) < (stepIncrement * 0.5);
                menu.addItem (i + 1, name, true, isTicked);
            }
        }

        menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (this),
            [this](int result) {
                if (result > 0) {
                    if (!presets.empty() && result <= (int)presets.size()) {
                        setCurrentValue (presets[(size_t)(result - 1)].second, juce::sendNotificationAsync);
                    } else {
                        int idx = result - 1;
                        int numPresets = 5;
                        double val = minValue + (maxValue - minValue) * ((double)idx / (double)(numPresets - 1));
                        setCurrentValue (val, juce::sendNotificationAsync);
                    }
                }
            });
        return;
    }

    if (items.empty()) return;
    juce::PopupMenu menu;
    for (size_t i = 0; i < items.size(); ++i) {
        menu.addItem ((int)i + 1, items[i], true, (int)i == selectedIndex);
    }

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (this),
        [this](int result) {
            if (result > 0) {
                setSelectedIndex (result - 1, juce::sendNotificationAsync);
            }
        });
}

void StepperComponent::paint (juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();

    if (layoutMode == LayoutMode::InspectorRow) {
        // Label on left
        g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
        g.setColour (CustomLookAndFeel::getMutedText());
        int labelWidth = 84;
        g.drawFittedText (label, 4, 0, labelWidth, (int)bounds.getHeight(), juce::Justification::centredLeft, 1);
    } else if (layoutMode == LayoutMode::RibbonInline) {
        // Ableton-style parameter box container
        auto boxRect = bounds.reduced (1.0f);

        g.setColour (CustomLookAndFeel::getCardBg());
        g.fillRoundedRectangle (boxRect, 3.5f);

        juce::Colour borderCol = isHovered ? CustomLookAndFeel::getLineBorder().brighter (0.4f)
                                           : CustomLookAndFeel::getLineBorder();
        g.setColour (borderCol);
        g.drawRoundedRectangle (boxRect, 3.5f, 1.0f);

        // Label on the left inside the box
        if (label.isNotEmpty()) {
            g.setFont (juce::FontOptions (10.0f, juce::Font::bold));
            g.setColour (CustomLookAndFeel::getMutedText());
            juce::Font font (juce::FontOptions (10.0f, juce::Font::bold));
            float labelW = (float)font.getStringWidth (label) + 2.0f;
            g.drawText (label, 5, 0, (int)labelW, (int)bounds.getHeight(), juce::Justification::centredLeft);
        }
    }
}

void StepperComponent::resized() {
    auto bounds = getLocalBounds();

    if (layoutMode == LayoutMode::InspectorRow) {
        int labelWidth = 86;
        bounds.removeFromLeft (labelWidth);

        if (lockable) {
            btnLock->setBounds (bounds.removeFromRight (20).reduced (0, 3));
            bounds.removeFromRight (4);
        }

        int arrowW = 14;
        btnRight->setBounds (bounds.removeFromRight (arrowW).reduced (0, 3));
        btnLeft->setBounds (bounds.removeFromRight (arrowW).reduced (0, 3));
        btnValue->setBounds (bounds.reduced (1, 2));
    } else { // RibbonInline: [ LABEL  <  VALUE  > ]
        btnLock->setVisible (false);

        auto b = bounds.reduced (2, 2);

        if (label.isNotEmpty()) {
            juce::Font font (juce::FontOptions (10.0f, juce::Font::bold));
            int labelW = font.getStringWidth (label) + 5;
            b.removeFromLeft (labelW);
        }

        int arrowW = 9;
        btnLeft->setBounds (b.removeFromLeft (arrowW));
        btnRight->setBounds (b.removeFromRight (arrowW));
        btnValue->setBounds (b);
    }
}

} // namespace CR
