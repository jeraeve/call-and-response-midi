#include "MidiDragComponent.h"

namespace CR {

MidiDragComponent::MidiDragComponent() {
}

void MidiDragComponent::setPhrases(const std::vector<Step>& call, const std::vector<Step>& resp, double tempoBpm) {
    callPhrase = call;
    responsePhrase = resp;
    bpm = (tempoBpm > 20.0 && tempoBpm < 999.0) ? tempoBpm : 140.0;
    repaint();
}

void MidiDragComponent::setExportMode(ExportMode mode) {
    currentMode = mode;
    repaint();
}

void MidiDragComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);

    // Pill background
    juce::Colour bgCol = isHovered ? juce::Colour(0xff24344d) : juce::Colour(0xff182232);
    juce::Colour borderCol = isHovered ? juce::Colour(0xff00e5ff) : juce::Colour(0xff335277);

    g.setColour(bgCol);
    g.fillRoundedRectangle(bounds, 8.0f);

    g.setColour(borderCol);
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);

    // Icon & text
    g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    g.setColour(isHovered ? juce::Colours::white : juce::Colour(0xffd0e2ff));

    juce::String labelText;
    switch (currentMode) {
        case ExportMode::Combined:     labelText = "DRAG MIDI: COMBINED"; break;
        case ExportMode::CallOnly:     labelText = "DRAG MIDI: CALL"; break;
        case ExportMode::ResponseOnly: labelText = "DRAG MIDI: RESPONSE"; break;
        case ExportMode::Layered:      labelText = "DRAG MIDI: LAYERED"; break;
    }

    // Small drag indicator dots on the left
    float dotX = bounds.getX() + 12.0f;
    float centerY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff00e5ff));
    for (int r = -1; r <= 1; ++r) {
        for (int c = 0; c < 2; ++c) {
            g.fillEllipse(dotX + (float)c * 5.0f, centerY + (float)r * 5.0f - 1.5f, 3.0f, 3.0f);
        }
    }

    auto textBounds = bounds.withTrimmedLeft(28.0f).withTrimmedRight(10.0f);
    g.drawFittedText(labelText, textBounds.toNearestInt(), juce::Justification::centred, 1);
}

void MidiDragComponent::resized() {}

juce::File MidiDragComponent::createMidiFile() {
    juce::MidiFile midiFile;
    const short ticksPerQuarter = 960;
    midiFile.setTicksPerQuarterNote(ticksPerQuarter);

    juce::MidiMessageSequence trackSequence;
    trackSequence.addEvent(juce::MidiMessage::tempoMetaEvent((int)(60000000.0 / bpm)), 0);
    trackSequence.addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);

    const int ticksPer16th = ticksPerQuarter / 4; // 240 ticks per 16th note

    auto appendPhrase = [&](const std::vector<Step>& phrase, int startTick, int midiChannel) {
        for (size_t i = 0; i < phrase.size(); ++i) {
            const auto& s = phrase[i];
            if (!s.active) continue;

            int noteNumber = s.getEffectiveMidiNote();
            float vel = s.accent ? 1.0f : 0.75f;
            int noteStart = startTick + (int)i * ticksPer16th;

            // Gate calculation
            int gateTicks = (int)((float)ticksPer16th * s.gate);
            if (s.slide) gateTicks += ticksPer16th / 4; // overlap for 303 slide
            gateTicks = std::max(20, gateTicks);

            int noteEnd = noteStart + gateTicks;

            trackSequence.addEvent(juce::MidiMessage::noteOn(midiChannel, noteNumber, vel), noteStart);
            trackSequence.addEvent(juce::MidiMessage::noteOff(midiChannel, noteNumber), noteEnd);
        }
    };

    int callLen = (int)callPhrase.size();
    int respLen = (int)responsePhrase.size();

    if (currentMode == ExportMode::CallOnly) {
        appendPhrase(callPhrase, 0, 1);
    } else if (currentMode == ExportMode::ResponseOnly) {
        appendPhrase(responsePhrase, 0, 2);
    } else if (currentMode == ExportMode::Layered) {
        appendPhrase(callPhrase, 0, 1);
        appendPhrase(responsePhrase, 0, 2);
    } else { // Combined: Call then Response
        appendPhrase(callPhrase, 0, 1);
        appendPhrase(responsePhrase, callLen * ticksPer16th, 2);
    }

    // Optional loop padding to whole bars
    int totalSteps = (currentMode == ExportMode::Combined) ? (callLen + respLen)
                   : std::max(callLen, respLen);
    if (wholeBars) {
        int stepsPerBar = 16;
        int numBars = std::max(1, (totalSteps + stepsPerBar - 1) / stepsPerBar);
        int endOfClipTicks = numBars * stepsPerBar * ticksPer16th;
        trackSequence.addEvent(juce::MidiMessage::endOfTrack(), endOfClipTicks);
    }

    trackSequence.updateMatchedPairs();
    midiFile.addTrack(trackSequence);

    // Write to temporary directory
    auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
    juce::String fileName;
    switch (currentMode) {
        case ExportMode::CallOnly:     fileName = "Call_Phrase.mid"; break;
        case ExportMode::ResponseOnly: fileName = "Response_Phrase.mid"; break;
        case ExportMode::Layered:      fileName = "Layered_Pair.mid"; break;
        case ExportMode::Combined:     fileName = "Call_Response_Combined.mid"; break;
    }

    juce::File outFile = tempDir.getChildFile(fileName);
    outFile.deleteFile();
    juce::FileOutputStream stream(outFile);
    if (stream.openedOk()) {
        midiFile.writeTo(stream);
    }

    return outFile;
}

void MidiDragComponent::mouseDrag(const juce::MouseEvent& e) {
    if (e.mouseWasDraggedSinceMouseDown()) {
        juce::File midiFile = createMidiFile();
        if (midiFile.existsAsFile()) {
            juce::StringArray files;
            files.add(midiFile.getFullPathName());
            juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this);
        }
    }
}

} // namespace CR
