#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace CR {

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override = default;

    // Rotary Sliders
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    // Linear Sliders
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    // Buttons
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // ComboBox
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    // Popup Menu Background
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    // Exact colors sampled from UI reference examples:
    static juce::Colour getDarkBg()        { return juce::Colour (0xff1f1e20); } // Warm dark charcoal background
    static juce::Colour getTopBarBg()      { return juce::Colour (0xff222022); } // Top bar header
    static juce::Colour getPanelBg()       { return juce::Colour (0xff181719); } // Container panels
    static juce::Colour getCardBg()        { return juce::Colour (0xff262426); } // Box/card background
    static juce::Colour getLineBorder()    { return juce::Colour (0xff38353b); } // Line borders
    static juce::Colour getNeonGreen()     { return juce::Colour (0xff5fe743); } // NEW PAIR & Call lane pill #5fe743
    static juce::Colour getNeonCyan()      { return juce::Colour (0xff41d9eb); } // Piano roll Call note #41d9eb
    static juce::Colour getNeonOrange()    { return juce::Colour (0xffd9952d); } // Piano roll Resp note #d9952d
    static juce::Colour getNeonEmerald()   { return juce::Colour (0xff5bdd40); } // Lime/emerald accent
    static juce::Colour getNeonViolet()    { return juce::Colour (0xff5c349f); } // Response pill #5c349f
    static juce::Colour getPanicViolet()   { return juce::Colour (0xff512d86); } // Top PANIC box #512d86
    static juce::Colour getMutedText()     { return juce::Colour (0xff9e9ba3); }
    static juce::Colour getActiveText()    { return juce::Colour (0xfff2eff5); }
};

class LockIconButton : public juce::Button {
public:
    explicit LockIconButton(const juce::String& name = "Lock")
        : juce::Button(name) {
        setClickingTogglesState(true);
    }
    ~LockIconButton() override = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class PresetSlotButton : public juce::Button {
public:
    explicit PresetSlotButton(const juce::String& name = juce::String())
        : juce::Button(name) {}
    ~PresetSlotButton() override = default;

    int slotIndex = 0;
    bool isOccupied = false;

    std::function<void(int)> onSave;
    std::function<void(int)> onLoad;
    std::function<void(int)> onSelect;

    void mouseDown(const juce::MouseEvent& e) override {
        juce::Button::mouseDown(e);
        if (e.mods.isRightButtonDown()) {
            juce::PopupMenu menu;
            char slotChar = (char)('A' + slotIndex);
            menu.addItem(1, "Save Current State to Slot " + juce::String::charToString(slotChar));
            if (isOccupied) {
                menu.addItem(2, "Load Preset Slot " + juce::String::charToString(slotChar));
            }
            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),
                [this](int result) {
                    if (result == 1 && onSave) onSave(slotIndex);
                    else if (result == 2 && onLoad) onLoad(slotIndex);
                });
            return;
        }
        if (e.mods.isCommandDown() || e.mods.isShiftDown() || e.mods.isAltDown() || e.mods.isCtrlDown()) {
            if (onSave) onSave(slotIndex);
        } else {
            if (onSelect) onSelect(slotIndex);
        }
    }

    void mouseDoubleClick(const juce::MouseEvent& /*e*/) override {
        if (onLoad) onLoad(slotIndex);
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

} // namespace CR
