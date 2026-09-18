#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include <string>
#include <map>
#include <random>

namespace CR {

struct Step {
    bool active = false;
    int degree = 0;
    int midiNote = 60;
    float gate = 0.82f;
    bool accent = false;
    bool slide = false;
    bool tie = false;
    int octaveOffset = 0; // applied dynamically by Octave Jump control

    int getEffectiveMidiNote() const {
        return juce::jlimit(0, 127, midiNote + octaveOffset);
    }

    bool operator== (const Step& o) const {
        return active == o.active && degree == o.degree && midiNote == o.midiNote
            && std::abs(gate - o.gate) < 0.001f && accent == o.accent
            && slide == o.slide && tie == o.tie && octaveOffset == o.octaveOffset;
    }

    bool operator!= (const Step& o) const {
        return !(*this == o);
    }
};

struct GeneratorConfig {
    int keyIndex = 9;                 // "A" (0=C, 1=C#, ..., 9=A, 11=B)
    int scaleIndex = 15;              // "Minor Pentatonic"
    int rootOctaveIndex = 2;          // 0=C1, 1=C2, 2=C3, 3=C4, 4=C5
    int steps = 16;                   // 2 to 16
    int styleIndex = 0;               // "Classic Acid"
    int responseStrategyIndex = 0;    // "Same rhythm + resolve"
    int motifTypeIndex = 20;          // "Acid nibble"
    int phraseModeIndex = 2;          // "Call then response"
    int endingBehaviourIndex = 10;    // "Response more resolved than call"
    int registerProfileIndex = 1;     // "Medium register"
    int densityPresetIndex = 2;       // "Medium"
    int contourPresetIndex = 6;       // "Stepwise"
    int repetitionPresetIndex = 1;    // "Repetitive"
    int mutationAmountIndex = 2;      // "Medium"
    int mutationTargetIndex = 8;      // "Call + response linked"
    int notePoolModeIndex = 1;        // "Pool with spice notes"
    int articulationProfileIndex = 3; // "Acidic"
    int gateFeelIndex = 4;            // "Mixed"
    int connectionFeelIndex = 2;      // "Legato"
    int slideProfileIndex = 2;        // "Occasional"
    int accentProfileIndex = 6;       // "Motif accents"
    int baseVelocityIndex = 4;        // 0..6: 32, 48, 64, 80, 96, 112, 127 (default 96)
    int baseVelocity = 96;            // Continuous 1 to 127
    int accentAmountIndex = 3;        // 0..5: 0%, 25%, 50%, 65%, 80%, 100% (default 65%)
    int variationIndex = 1;           // 0..2: Small, Medium, Large (default Medium)
    int callChannel = 1;              // 1 to 16
    int responseChannel = 2;          // 1 to 16
};

struct PerformanceParams {
    float density = 0.5f;     // 0.0 to 1.0 (adds/removes notes)
    float noteLength = 1.0f;  // 0.25 to 2.0 (scales gate)
    float octaveJump = 0.0f;  // 0.0 to 1.0 (chance of +/-12/24 st)
    float resolve = 0.5f;     // 0.0 to 1.0 (cadence resolution)
};

class GeneratorEngine {
public:
    GeneratorEngine();

    // Data lists
    static const std::vector<juce::String>& getKeys();
    static const std::vector<juce::String>& getScales();
    static const std::vector<int>& getScaleIntervals(int scaleIndex);
    static const std::vector<juce::String>& getStyles();
    static const std::vector<juce::String>& getResponseStrategies();
    static const std::vector<juce::String>& getMotifTypes();
    static const std::vector<juce::String>& getPhraseModes();
    static const std::vector<juce::String>& getEndingBehaviours();
    static const std::vector<juce::String>& getRegisterProfiles();
    static const std::vector<juce::String>& getDensityPresets();
    static const std::vector<juce::String>& getContourPresets();
    static const std::vector<juce::String>& getRepetitionPresets();
    static const std::vector<juce::String>& getMutationAmounts();
    static const std::vector<juce::String>& getMutationTargets();
    static const std::vector<juce::String>& getNotePoolModes();
    static const std::vector<juce::String>& getArticulationProfiles();
    static const std::vector<juce::String>& getGateFeels();
    static const std::vector<juce::String>& getConnectionFeels();
    static const std::vector<juce::String>& getSlideProfiles();
    static const std::vector<juce::String>& getAccentProfiles();
    static const std::vector<juce::String>& getOctaves();

    // Core Generation
    static void generatePair(const GeneratorConfig& config,
                             std::vector<Step>& outCall,
                             std::vector<Step>& outResponse,
                             uint32_t seed = 0);

    static std::vector<Step> buildCall(const GeneratorConfig& config, std::mt19937& rng);
    static std::vector<Step> buildResponse(const std::vector<Step>& call, const GeneratorConfig& config, std::mt19937& rng);

    // Performance Application
    static void applyPerformance(std::vector<Step>& phrase,
                                 const GeneratorConfig& config,
                                 const PerformanceParams& perf,
                                 uint32_t seed = 0);

    // Edit Actions
    static void mutatePhrase(std::vector<Step>& phrase,
                             const GeneratorConfig& config,
                             const juce::String& target,
                             int amountIndex,
                             std::mt19937& rng);

    static void shift(std::vector<Step>& phrase, int delta);
    static void invert(std::vector<Step>& phrase, const GeneratorConfig& config);
    static void reverse(std::vector<Step>& phrase);
    static void transposeOctave(std::vector<Step>& phrase, int octaves);
    static void transposePhraseToScale(std::vector<Step>& phrase,
                                       const GeneratorConfig& oldConfig,
                                       const GeneratorConfig& newConfig);

    // Pitch conversion helpers
    static int rootMidi(const GeneratorConfig& config);
    static int degreeToMidi(int deg, const GeneratorConfig& config);
    static int midiToDegree(int midi, const GeneratorConfig& config);
    static juce::String midiToName(int midiNote);

private:
    struct StyleWeight {
        juce::String cat;
        float density = 0.5f;
        float repeat = 0.5f;
        float slide = 0.2f;
        float accent = 0.25f;
        float rootBias = 0.25f;
        float syncBias = 0.2f;
        float choppyBias = 0.15f;
        float sustainBias = 0.2f;
        float restBias = 0.2f;
        float motifVar = 0.2f;
        float responseTightness = 0.6f;
        float leap = 0.2f;
        float stableEnd = 0.7f;
        float offbeatBias = 0.2f;
        float octaveBias = 0.15f;
    };

    static StyleWeight getStyleProfile(const GeneratorConfig& config);
    static std::vector<int> getNotePool(const GeneratorConfig& config, std::mt19937& rng);
    static std::vector<int> stableDegreesForScale(const GeneratorConfig& config);
    static int phraseContour(const std::vector<Step>& phrase);
    static void applyEnding(std::vector<Step>& phrase, const GeneratorConfig& config, const juce::String& phase, std::mt19937& rng);
    static void postProcessArticulation(std::vector<Step>& phrase, const GeneratorConfig& config, std::mt19937& rng);
    static void updateMidiNotes(std::vector<Step>& phrase, const GeneratorConfig& config);
};

} // namespace CR
