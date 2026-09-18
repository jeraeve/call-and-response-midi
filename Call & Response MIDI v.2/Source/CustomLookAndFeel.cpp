#include "CustomLookAndFeel.h"

namespace CR {

CustomLookAndFeel::CustomLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, getDarkBg());
    setColour(juce::ComboBox::backgroundColourId, getCardBg());
    setColour(juce::ComboBox::textColourId, getActiveText());
    setColour(juce::ComboBox::outlineColourId, getLineBorder());
    setColour(juce::PopupMenu::backgroundColourId, getPanelBg());
    setColour(juce::PopupMenu::textColourId, getActiveText());
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff2d2a2f));
    setColour(juce::PopupMenu::highlightedTextColourId, getActiveText());
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                        juce::Slider& slider) {
    auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(3.0f);
    auto radius = std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();

    float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float trackWidth = 3.5f;

    // Outer background track
    juce::Path bgArc;
    bgArc.addCentredArc(centre.x, centre.y, radius - trackWidth, radius - trackWidth,
                        0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff1f2a3a));
    g.strokePath(bgArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active illuminated arc
    juce::Path valArc;
    valArc.addCentredArc(centre.x, centre.y, radius - trackWidth, radius - trackWidth,
                         0.0f, rotaryStartAngle, angle, true);

    juce::Colour accentCol = slider.findColour(juce::Slider::rotarySliderFillColourId);
    if (accentCol.isTransparent()) accentCol = getNeonCyan();

    g.setColour(accentCol);
    g.strokePath(valArc, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Dial body
    float innerRadius = radius - trackWidth - 4.0f;
    if (innerRadius > 4.0f) {
        juce::ColourGradient grad(juce::Colour(0xff1f2b3e), centre.x, centre.y - innerRadius,
                                  juce::Colour(0xff141c28), centre.x, centre.y + innerRadius, false);
        g.setGradientFill(grad);
        g.fillEllipse(centre.x - innerRadius, centre.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        g.setColour(juce::Colour(0xff33445c));
        g.drawEllipse(centre.x - innerRadius, centre.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);

        // Indicator line / needle
        juce::Point<float> needleStart(centre.x + (innerRadius * 0.35f) * std::sin(angle),
                                       centre.y - (innerRadius * 0.35f) * std::cos(angle));
        juce::Point<float> needleEnd(centre.x + (innerRadius * 0.85f) * std::sin(angle),
                                     centre.y - (innerRadius * 0.85f) * std::cos(angle));

        g.setColour(accentCol);
        g.drawLine(needleStart.x, needleStart.y, needleEnd.x, needleEnd.y, 2.5f);
    }
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle style, juce::Slider& slider) {
    juce::ignoreUnused(minSliderPos, maxSliderPos, style);
    auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height);

    if (slider.isHorizontal()) {
        float trackY = bounds.getCentreY();
        float trackH = 4.0f;
        auto trackArea = juce::Rectangle<float>(bounds.getX() + 6.0f, trackY - trackH * 0.5f,
                                                bounds.getWidth() - 12.0f, trackH);

        g.setColour(juce::Colour(0xff1b2636));
        g.fillRoundedRectangle(trackArea, 2.0f);

        // Active bar
        auto activeArea = trackArea.withWidth(sliderPos - trackArea.getX());
        g.setColour(getNeonCyan());
        g.fillRoundedRectangle(activeArea, 2.0f);

        // Thumb handle
        float thumbRadius = 6.5f;
        g.setColour(juce::Colour(0xff0d1520));
        g.fillEllipse(sliderPos - thumbRadius, trackY - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
        g.setColour(getNeonCyan());
        g.drawEllipse(sliderPos - thumbRadius, trackY - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f, 1.8f);
    } else {
        // Vertical slider
        float trackX = bounds.getCentreX();
        float trackW = 4.0f;
        auto trackArea = juce::Rectangle<float>(trackX - trackW * 0.5f, bounds.getY() + 6.0f,
                                                trackW, bounds.getHeight() - 12.0f);

        g.setColour(juce::Colour(0xff1b2636));
        g.fillRoundedRectangle(trackArea, 2.0f);

        auto activeArea = trackArea.withTop(sliderPos);
        g.setColour(getNeonOrange());
        g.fillRoundedRectangle(activeArea, 2.0f);

        float thumbRadius = 6.5f;
        g.setColour(juce::Colour(0xff0d1520));
        g.fillEllipse(trackX - thumbRadius, sliderPos - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f);
        g.setColour(getNeonOrange());
        g.drawEllipse(trackX - thumbRadius, sliderPos - thumbRadius, thumbRadius * 2.0f, thumbRadius * 2.0f, 1.8f);
    }
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                             const juce::Colour& backgroundColour,
                                             bool isHighlighted, bool isDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    float corner = 6.0f;

    bool isToggleOn = button.getToggleState();
    juce::Colour bg = backgroundColour;

    bool hasCustomBg = button.isColourSpecified (juce::TextButton::buttonColourId);

    if (hasCustomBg) {
        bg = button.findColour (juce::TextButton::buttonColourId);
        if (isToggleOn) {
            bg = button.isColourSpecified (juce::TextButton::buttonOnColourId)
               ? button.findColour (juce::TextButton::buttonOnColourId)
               : bg.brighter (0.2f);
        } else if (isDown) {
            bg = bg.darker (0.2f);
        } else if (isHighlighted) {
            bg = bg.brighter (0.15f);
        }
    } else {
        if (isToggleOn) {
            bg = juce::Colour(0xff193a4d);
        } else if (isDown) {
            bg = juce::Colour(0xff121924);
        } else if (isHighlighted) {
            bg = juce::Colour(0xff222e40);
        }
    }

    g.setColour(bg);
    g.fillRoundedRectangle(bounds, corner);

    // Border
    juce::Colour borderCol = getLineBorder();
    if (hasCustomBg) {
        borderCol = bg.brighter (0.3f);
    } else if (isToggleOn) {
        borderCol = getNeonCyan();
    } else if (isHighlighted) {
        borderCol = juce::Colour(0xff4a6282);
    }

    g.setColour(borderCol);
    g.drawRoundedRectangle(bounds, corner, isToggleOn ? 1.5f : 1.0f);
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                       bool isHighlighted, bool isDown) {
    juce::ignoreUnused(isDown);
    auto text = button.getButtonText();
    if (text.isEmpty()) return;

    juce::Colour textCol;
    if (button.isColourSpecified (juce::TextButton::textColourOffId)) {
        textCol = button.getToggleState() && button.isColourSpecified (juce::TextButton::textColourOnId)
                ? button.findColour (juce::TextButton::textColourOnId)
                : button.findColour (juce::TextButton::textColourOffId);
    } else {
        textCol = button.getToggleState() ? getNeonCyan()
                : (isHighlighted ? juce::Colours::white : getActiveText());
    }

    g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    g.setColour(textCol);
    g.drawFittedText(text, button.getLocalBounds(), juce::Justification::centred, 1);
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                     int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) {
    juce::ignoreUnused(isButtonDown, buttonX, buttonY, buttonW, buttonH);
    auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height).reduced(1.0f);
    float corner = 6.0f;

    g.setColour(box.hasKeyboardFocus(false) ? juce::Colour(0xff1c283a) : getCardBg());
    g.fillRoundedRectangle(bounds, corner);

    g.setColour(box.hasKeyboardFocus(false) ? getNeonCyan() : getLineBorder());
    g.drawRoundedRectangle(bounds, corner, 1.0f);

    // Arrow icon
    float arrowX = (float)width - 16.0f;
    float arrowY = (float)height * 0.5f - 2.0f;
    juce::Path arrow;
    arrow.startNewSubPath(arrowX, arrowY);
    arrow.lineTo(arrowX + 4.0f, arrowY + 4.0f);
    arrow.lineTo(arrowX + 8.0f, arrowY);

    g.setColour(getMutedText());
    g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void CustomLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label) {
    label.setBounds(5, 0, box.getWidth() - 19, box.getHeight());
    label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centredLeft);
}

void CustomLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height) {
    auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height);
    g.setColour(getPanelBg());
    g.fillRoundedRectangle(bounds, 6.0f);

    g.setColour(getLineBorder());
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                                          const juce::String& text, const juce::String& shortcutKeyText,
                                          const juce::Drawable* icon, const juce::Colour* textColour) {
    juce::ignoreUnused(isTicked, hasSubMenu, shortcutKeyText, icon, textColour);

    if (isSeparator) {
        g.setColour(getLineBorder());
        g.fillRect(area.reduced(4, 0).withHeight(1));
        return;
    }

    if (isHighlighted && isActive) {
        g.setColour(juce::Colour(0xff223145));
        g.fillRoundedRectangle(area.toFloat().reduced(2.0f), 4.0f);
    }

    g.setFont(juce::FontOptions(13.0f));
    g.setColour(isHighlighted ? getNeonCyan() : (isActive ? getActiveText() : getMutedText()));
    g.drawFittedText(text, area.reduced(10, 0), juce::Justification::centredLeft, 1);
}

void LockIconButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto b = getLocalBounds().toFloat().reduced(2.0f);
    bool locked = getToggleState();

    juce::Colour bgCol = locked ? juce::Colour(0xff182d3f) : juce::Colour(0xff141c28);
    if (shouldDrawButtonAsHighlighted) bgCol = bgCol.brighter(0.12f);
    if (shouldDrawButtonAsDown) bgCol = bgCol.darker(0.15f);

    g.setColour(bgCol);
    g.fillRoundedRectangle(b, 5.0f);

    juce::Colour borderCol = locked ? CustomLookAndFeel::getNeonCyan().withAlpha(0.9f)
                                    : (shouldDrawButtonAsHighlighted ? CustomLookAndFeel::getNeonCyan().withAlpha(0.5f)
                                                                     : CustomLookAndFeel::getLineBorder());
    g.setColour(borderCol);
    g.drawRoundedRectangle(b, 5.0f, locked ? 1.4f : 1.0f);

    // Vector padlock
    float cx = b.getCentreX();
    float cy = b.getCentreY();
    float bodyW = 13.0f;
    float bodyH = 9.5f;
    float bodyX = cx - bodyW * 0.5f;
    float bodyY = cy - 1.0f;

    juce::Colour iconCol = locked ? CustomLookAndFeel::getNeonCyan()
                                  : (shouldDrawButtonAsHighlighted ? CustomLookAndFeel::getActiveText()
                                                                   : CustomLookAndFeel::getMutedText().withAlpha(0.7f));

    // Padlock body
    g.setColour(iconCol);
    g.fillRoundedRectangle(bodyX, bodyY, bodyW, bodyH, 2.0f);

    // Keyhole
    g.setColour(juce::Colour(0xff0d1117));
    g.fillEllipse(cx - 1.25f, bodyY + 2.2f, 2.5f, 2.5f);
    g.fillRect(cx - 0.75f, bodyY + 3.8f, 1.5f, 2.8f);

    // Padlock shackle
    g.setColour(iconCol);
    float shackleR = 4.2f;
    float shackleStroke = 1.9f;
    juce::Path shackle;

    if (locked) {
        // Closed shackle: loops from left body into right body
        shackle.startNewSubPath(cx - shackleR, bodyY + 1.0f);
        shackle.lineTo(cx - shackleR, bodyY - 4.5f);
        shackle.addCentredArc(cx, bodyY - 4.5f, shackleR, shackleR, 0.0f, -juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi, false);
        shackle.lineTo(cx + shackleR, bodyY + 1.0f);
    } else {
        // Open shackle: lifted up and swung open on the right
        shackle.startNewSubPath(cx - shackleR, bodyY - 0.5f);
        shackle.lineTo(cx - shackleR, bodyY - 6.5f);
        shackle.addCentredArc(cx, bodyY - 6.5f, shackleR, shackleR, 0.0f, -juce::MathConstants<float>::halfPi, juce::MathConstants<float>::halfPi, false);
        shackle.lineTo(cx + shackleR, bodyY - 4.0f); // open gap above the body
    }
    g.strokePath(shackle, juce::PathStrokeType(shackleStroke, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void PresetSlotButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto b = getLocalBounds().toFloat().reduced(1.5f);

    juce::Colour bgCol = isOccupied ? juce::Colour(0xff1c2b3d) : juce::Colour(0xff131a26);
    if (shouldDrawButtonAsHighlighted) bgCol = bgCol.brighter(0.15f);
    if (shouldDrawButtonAsDown) bgCol = bgCol.darker(0.2f);

    g.setColour(bgCol);
    g.fillRoundedRectangle(b, 4.0f);

    juce::Colour borderCol = isOccupied ? CustomLookAndFeel::getNeonCyan().withAlpha(shouldDrawButtonAsHighlighted ? 0.9f : 0.6f)
                                        : (shouldDrawButtonAsHighlighted ? CustomLookAndFeel::getLineBorder().brighter()
                                                                         : CustomLookAndFeel::getLineBorder().withAlpha(0.6f));
    g.setColour(borderCol);
    g.drawRoundedRectangle(b, 4.0f, isOccupied ? 1.2f : 1.0f);

    // Letter
    g.setFont(juce::FontOptions(12.0f, isOccupied ? juce::Font::bold : juce::Font::plain));
    g.setColour(isOccupied ? CustomLookAndFeel::getActiveText()
                           : (shouldDrawButtonAsHighlighted ? CustomLookAndFeel::getActiveText() : CustomLookAndFeel::getMutedText().withAlpha(0.65f)));
    g.drawText(getButtonText(), b.toNearestInt(), juce::Justification::centred);

    // Mini indicator dot for occupied preset
    if (isOccupied) {
        g.setColour(CustomLookAndFeel::getNeonCyan());
        g.fillEllipse(b.getRight() - 5.5f, b.getY() + 3.0f, 3.0f, 3.0f);
    }
}

} // namespace CR
