#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "GeneratorEngine.h"

namespace CR {

class MidiDragComponent : public juce::Component,
                           public juce::TooltipClient {
public:
    enum class ExportMode {
        Combined,
        CallOnly,
        ResponseOnly,
        Layered
    };

    MidiDragComponent();
    ~MidiDragComponent() override = default;

    juce::String getTooltip() override {
        return "Click & drag this button directly into your DAW or desktop to export MIDI clip!";
    }

    void setPhrases(const std::vector<Step>& call, const std::vector<Step>& response, double tempoBpm);
    void setExportMode(ExportMode mode);
    ExportMode getExportMode() const { return currentMode; }

    void setWholeBars(bool roundToWholeBars) { wholeBars = roundToWholeBars; }
    bool isWholeBars() const { return wholeBars; }

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    juce::File createMidiFile();

    std::vector<Step> callPhrase;
    std::vector<Step> responsePhrase;
    double bpm = 140.0;
    ExportMode currentMode = ExportMode::Combined;
    bool wholeBars = true;

    bool isHovered = false;
    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&) override { isHovered = false; repaint(); }
};

} // namespace CR
