#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <functional>

namespace CR {

class StepperComponent : public juce::Component {
public:
    enum class LayoutMode {
        RibbonInline, // Ableton-style box: [ LABEL  <  VALUE  > ]
        InspectorRow  // Inspector row: Label on left, <, value, >, [lock] on far right
    };

    explicit StepperComponent (const juce::String& labelText = {},
                               LayoutMode mode = LayoutMode::InspectorRow);
    ~StepperComponent() override;

    void setLayoutMode (LayoutMode mode);
    void setLabelText (const juce::String& text);
    juce::String getLabelText() const { return label; }

    void setItems (const std::vector<juce::String>& itemsList);
    const std::vector<juce::String>& getItems() const { return items; }

    void setSelectedIndex (int index, juce::NotificationType notification = juce::sendNotificationAsync);
    int getSelectedIndex() const { return selectedIndex; }
    juce::String getSelectedText() const;

    void setLockable (bool canLock);
    bool isLockable() const { return lockable; }
    void setLocked (bool isLocked, juce::NotificationType notification = juce::dontSendNotification);
    bool isLocked() const;

    void setContinuousRange (double minVal, double maxVal, double defaultVal, double step = 1.0, const juce::String& suffix = "");
    bool isContinuous() const { return continuousMode; }
    double getCurrentValue() const { return currentValue; }
    void setCurrentValue (double val, juce::NotificationType notification = juce::sendNotificationAsync);
    void setContinuousPresets (const std::vector<std::pair<juce::String, double>>& presetsList);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseEnter (const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit (const juce::MouseEvent&) override { isHovered = false; repaint(); }

    std::function<void(int index)> onChange;
    std::function<void(double value)> onValueChanged;
    std::function<void(bool locked)> onLockChanged;

private:
    void stepPrev();
    void stepNext();
    void showPopupMenu();

    LayoutMode layoutMode = LayoutMode::InspectorRow;
    juce::String label;
    std::vector<juce::String> items;
    int selectedIndex = 0;

    bool continuousMode = false;
    double minValue = 0.0;
    double maxValue = 100.0;
    double currentValue = -999999.0;
    double stepIncrement = 1.0;
    juce::String valueSuffix;
    std::vector<std::pair<juce::String, double>> presets;

    bool lockable = true;
    bool isHovered = false;

    class ArrowButton;
    class ValueButton;
    class PadlockButton;

    std::unique_ptr<ArrowButton> btnLeft;
    std::unique_ptr<ArrowButton> btnRight;
    std::unique_ptr<ValueButton> btnValue;
    std::unique_ptr<PadlockButton> btnLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StepperComponent)
};

} // namespace CR
