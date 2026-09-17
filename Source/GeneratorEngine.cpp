#include "GeneratorEngine.h"
#include <algorithm>
#include <numeric>

namespace CR {

// Helper random functions
static bool chance(float prob, std::mt19937& rng) {
    if (prob <= 0.0f) return false;
    if (prob >= 1.0f) return true;
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < prob;
}

static int randChoice(const std::vector<int>& items, std::mt19937& rng) {
    if (items.empty()) return 0;
    std::uniform_int_distribution<size_t> dist(0, items.size() - 1);
    return items[dist(rng)];
}

static float randChoiceFloat(const std::vector<float>& items, std::mt19937& rng) {
    if (items.empty()) return 0.0f;
    std::uniform_int_distribution<size_t> dist(0, items.size() - 1);
    return items[dist(rng)];
}

[[maybe_unused]] static int randRange(int minVal, int maxVal, std::mt19937& rng) {
    if (minVal >= maxVal) return minVal;
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(rng);
}

static std::vector<int> pickN(const std::vector<int>& pool, size_t n, std::mt19937& rng) {
    std::vector<int> copy = pool;
    std::shuffle(copy.begin(), copy.end(), rng);
    if (copy.size() > n) copy.resize(n);
    return copy;
}

const std::vector<juce::String>& GeneratorEngine::getKeys() {
    static const std::vector<juce::String> keys = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    return keys;
}

const std::vector<juce::String>& GeneratorEngine::getOctaves() {
    static const std::vector<juce::String> octs = { "C1", "C2", "C3", "C4", "C5" };
    return octs;
}

static const std::vector<std::pair<juce::String, std::vector<int>>> ALL_SCALES = {
    {"Chromatic", {0,1,2,3,4,5,6,7,8,9,10,11}},
    {"Major / Ionian", {0,2,4,5,7,9,11}},
    {"Natural Minor / Aeolian", {0,2,3,5,7,8,10}},
    {"Harmonic Minor", {0,2,3,5,7,8,11}},
    {"Melodic Minor", {0,2,3,5,7,9,11}},
    {"Dorian", {0,2,3,5,7,9,10}},
    {"Phrygian", {0,1,3,5,7,8,10}},
    {"Mixolydian", {0,2,4,5,7,9,10}},
    {"Locrian", {0,1,3,5,6,8,10}},
    {"Lydian", {0,2,4,6,7,9,11}},
    {"Lydian Dominant", {0,2,4,6,7,9,10}},
    {"Aeolian b5", {0,2,3,5,6,8,10}},
    {"Dorian b2", {0,1,3,5,7,9,10}},
    {"Mixolydian b6", {0,2,4,5,7,8,10}},
    {"Major Pentatonic", {0,2,4,7,9}},
    {"Minor Pentatonic", {0,3,5,7,10}},
    {"Blues Minor", {0,3,5,6,7,10}},
    {"Blues Major", {0,2,3,4,7,9}},
    {"Phrygian Dominant", {0,1,4,5,7,8,10}},
    {"Double Harmonic", {0,1,4,5,7,8,11}},
    {"Hungarian Minor", {0,2,3,6,7,8,11}},
    {"Romanian Minor", {0,2,3,6,7,9,10}},
    {"Whole Tone", {0,2,4,6,8,10}},
    {"Diminished Half-Whole", {0,1,3,4,6,7,9,10}},
    {"Diminished Whole-Half", {0,2,3,5,6,8,9,11}},
    {"Persian", {0,1,4,5,6,8,11}},
    {"Arabic", {0,2,4,5,6,8,10}},
    {"Byzantine", {0,1,4,5,7,8,11}},
    {"Hirajoshi", {0,2,3,7,8}},
    {"In Sen", {0,1,5,7,10}},
    {"Japanese Pentatonic", {0,1,5,7,8}},
    {"Kumoi", {0,2,3,7,9}},
    {"Pelog-ish", {0,1,3,7,8}},
    {"Egyptian Pentatonic", {0,2,5,7,10}},
    {"Minor Triad only", {0,3,7}},
    {"Power Fifths", {0,7}},
    {"Root + b7", {0,10}},
    {"Root + 5th + b7", {0,7,10}},
    {"Root + 2nd + 5th", {0,2,7}},
    {"Minor 7 arpeggio pool", {0,3,7,10}},
    {"Dominant 7 arpeggio pool", {0,4,7,10}},
    {"Diminished 7 arpeggio pool", {0,3,6,9}}
};

const std::vector<juce::String>& GeneratorEngine::getScales() {
    static std::vector<juce::String> names;
    if (names.empty()) {
        for (const auto& s : ALL_SCALES)
            names.push_back(s.first);
    }
    return names;
}

const std::vector<int>& GeneratorEngine::getScaleIntervals(int scaleIndex) {
    if (scaleIndex >= 0 && scaleIndex < (int)ALL_SCALES.size())
        return ALL_SCALES[(size_t)scaleIndex].second;
    return ALL_SCALES[15].second; // Minor Pentatonic fallback
}

const std::vector<juce::String>& GeneratorEngine::getStyles() {
    static const std::vector<juce::String> styles = {
        "Classic Acid","Raw Acid","Squelchy Acid","Dark Acid","Hypno Acid","Sparse Acid","Busy Acid","Acid Stabs","Acid Runner","Acid Question/Answer",
        "Driving Techno","Hypnotic Techno","Minimal Techno","Raw Warehouse","Tool Techno","Dub Techno","Industrial Techno","Percussive Techno","Loop Techno","Broken Techno",
        "Jackin House","Deep House","Minimal House","Classic House","Tech House","Bass House","Shuffly House","Organ House-ish","House Stabline","House Bounce",
        "90s Rave","Dark Rave","Rave Stab","Electro Pulse","Breaky Electro","Detroit-ish","Proto-Trance","Hooverish Riff",
        "Motif Study","Rooted Groove","Repetitive Hook","Wide Answer","Tight Answer","Tension Builder","Resolver","Call Lead / Response Support"
    };
    return styles;
}

const std::vector<juce::String>& GeneratorEngine::getResponseStrategies() {
    static const std::vector<juce::String> strats = {
        "Same rhythm + resolve","Same rhythm + new contour","Motif + simplify","Motif + extend","Motif + truncate",
        "Echo","Answer upward","Answer downward","Higher answer","Lower answer","Invert contour","Mirror contour loosely",
        "Copy rhythm / change notes","Copy notes / change rhythm","Transpose motif up","Transpose motif down",
        "Response resolves to root","Response resolves to 5th","Response resolves lower","Response resolves harder",
        "Repeat then break","Tighter answer","Sparser answer","Busier answer","Accent the answer","Slide the answer",
        "Staccato answer","Legato answer","Delayed answer","Half-phrase answer","Fragment reply","Tail answer",
        "Opposite register","Same motif darker","Same motif brighter","Question then closure","Hook then fill","Call dominant / response tonic"
    };
    return strats;
}

const std::vector<juce::String>& GeneratorEngine::getMotifTypes() {
    static const std::vector<juce::String> motifs = {
        "Pulse","Offbeat","Syncopated","Push","Sparse","Dense","Rest-heavy","Four-on-floor companion","Stabbed","Rolling",
        "Repeated note","Stepwise rise","Stepwise fall","Rise then drop","Drop then rise","Leap then settle","Pivot note",
        "Two-note hook","Three-note hook","Arp fragment","Acid nibble","Techno cell","House bounce","Rave stab figure",
        "Drone anchor","Pedal point motif","Call tag","Answer tag"
    };
    return motifs;
}

const std::vector<juce::String>& GeneratorEngine::getPhraseModes() {
    static const std::vector<juce::String> modes = {
        "Call only","Response only","Call then response","Response then call","Alternate full bars","Alternate half bars",
        "Call, call, response","Call, response, response","A / B / A / C","Hook then answer","Question then answer",
        "Continuous phrase loop","Call with response fill","Response only on every 2nd pass","Mutating call, stable response"
    };
    return modes;
}

const std::vector<juce::String>& GeneratorEngine::getEndingBehaviours() {
    static const std::vector<juce::String> endings = {
        "Resolve to root","Resolve to 5th","Leave hanging","Rise into next phrase","Fall into next phrase","Hard stop",
        "Fade energy","Strong cadence","Weak cadence","Ambiguous ending","Response more resolved than call",
        "Response same tension as call","Response more tense than call"
    };
    return endings;
}

const std::vector<juce::String>& GeneratorEngine::getRegisterProfiles() {
    static const std::vector<juce::String> list = { "Narrow register", "Medium register", "Wide register" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getDensityPresets() {
    static const std::vector<juce::String> list = { "Very sparse", "Sparse", "Medium", "Busy", "Dense", "Very dense" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getContourPresets() {
    static const std::vector<juce::String> list = { "Flat", "Slight rise", "Slight fall", "Arch", "Inverted arch", "Wandering", "Stepwise", "Leapy" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getRepetitionPresets() {
    static const std::vector<juce::String> list = { "Very repetitive", "Repetitive", "Balanced", "Varied", "Highly varied" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getMutationAmounts() {
    static const std::vector<juce::String> list = { "Tiny", "Small", "Medium", "Strong", "Brutal" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getMutationTargets() {
    static const std::vector<juce::String> list = {
        "Rhythm only", "Pitch only", "Articulation only", "Ending only", "Motif only",
        "Whole phrase", "Response only", "Call only", "Call + response linked"
    };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getNotePoolModes() {
    static const std::vector<juce::String> list = {
        "Strict note pool", "Pool with spice notes", "Pool expands on response", "Pool contracts on response",
        "Root-heavy", "Root + 5th heavy", "Triad pool", "Seventh pool", "Scale-wide"
    };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getArticulationProfiles() {
    static const std::vector<juce::String> list = {
        "Clean", "Punchy", "Rolling", "Acidic", "Hypnotic", "Choppy", "Fluid", "Stabbed", "Tense", "Released"
    };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getGateFeels() {
    static const std::vector<juce::String> list = { "Tight", "Short", "Medium", "Long", "Mixed", "Random within bounds" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getConnectionFeels() {
    static const std::vector<juce::String> list = { "Detached", "Semi-detached", "Legato", "Tied bias", "Mixed ties", "Connected runs" };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getSlideProfiles() {
    static const std::vector<juce::String> list = {
        "None", "Rare", "Occasional", "Acid moderate", "Acid heavy", "Phrase-end slides", "Upward slides bias", "Downward slides bias"
    };
    return list;
}

const std::vector<juce::String>& GeneratorEngine::getAccentProfiles() {
    static const std::vector<juce::String> list = {
        "None", "Light", "Moderate", "Heavy", "Offbeat accents", "Downbeat accents", "Motif accents", "Random accents within motif"
    };
    return list;
}

// Root MIDI calculation
int GeneratorEngine::rootMidi(const GeneratorConfig& config) {
    static const int octaveOffsets[] = { 24, 36, 48, 60, 72 };
    int octIdx = juce::jlimit(0, 4, config.rootOctaveIndex);
    return octaveOffsets[octIdx] + juce::jlimit(0, 11, config.keyIndex);
}

int GeneratorEngine::degreeToMidi(int deg, const GeneratorConfig& config) {
    const auto& s = getScaleIntervals(config.scaleIndex);
    int len = std::max(1, (int)s.size());
    int oct = (deg >= 0) ? (deg / len) : ((deg - len + 1) / len);
    int idx = ((deg % len) + len) % len;
    return juce::jlimit(0, 127, rootMidi(config) + s[(size_t)idx] + oct * 12);
}

int GeneratorEngine::midiToDegree(int midi, const GeneratorConfig& config) {
    int root = rootMidi(config);
    const auto& s = getScaleIntervals(config.scaleIndex);
    int len = std::max(1, (int)s.size());
    int diff = midi - root;
    int oct = (diff >= 0) ? (diff / 12) : ((diff - 11) / 12);
    int pitchClass = ((diff % 12) + 12) % 12;
    int bestIdx = 0;
    int bestDist = 999;
    for (int i = 0; i < len; ++i) {
        int d = std::abs(s[(size_t)i] - pitchClass);
        if (d < bestDist) {
            bestDist = d;
            bestIdx = i;
        }
    }
    return oct * len + bestIdx;
}

juce::String GeneratorEngine::midiToName(int midiNote) {
    if (midiNote < 0 || midiNote > 127) return "---";
    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (midiNote / 12) - 1;
    return juce::String(noteNames[midiNote % 12]) + juce::String(octave);
}

void GeneratorEngine::updateMidiNotes(std::vector<Step>& phrase, const GeneratorConfig& config) {
    for (auto& s : phrase) {
        if (s.active) {
            s.midiNote = degreeToMidi(s.degree, config);
        } else {
            s.midiNote = 60;
        }
    }
}

GeneratorEngine::StyleWeight GeneratorEngine::getStyleProfile(const GeneratorConfig& config) {
    const auto& styles = getStyles();
    juce::String styleName = (config.styleIndex >= 0 && config.styleIndex < (int)styles.size())
                             ? styles[(size_t)config.styleIndex] : "Classic Acid";
    juce::String s = styleName.toLowerCase();

    StyleWeight sw;
    if (s.contains("acid")) {
        sw.cat = "acid"; sw.density = 0.55f; sw.leap = 0.22f; sw.repeat = 0.62f;
        sw.accent = 0.45f; sw.slide = 0.35f; sw.rootBias = 0.28f; sw.responseTightness = 0.66f;
        sw.syncBias = 0.26f; sw.sustainBias = 0.34f;
    } else if (s.contains("techno")) {
        sw.cat = "techno"; sw.density = 0.42f; sw.leap = 0.14f; sw.repeat = 0.68f;
        sw.accent = 0.20f; sw.slide = 0.08f; sw.rootBias = 0.35f; sw.restBias = 0.24f;
        sw.responseTightness = 0.70f; sw.choppyBias = 0.30f; sw.syncBias = 0.12f;
    } else if (s.contains("house")) {
        sw.cat = "house"; sw.density = 0.48f; sw.leap = 0.12f; sw.repeat = 0.58f;
        sw.accent = 0.22f; sw.slide = 0.10f; sw.rootBias = 0.32f; sw.responseTightness = 0.62f;
        sw.offbeatBias = 0.30f; sw.syncBias = 0.20f; sw.sustainBias = 0.12f;
    } else if (s.contains("rave") || s.contains("electro") || s.contains("hoover")) {
        sw.cat = "rave"; sw.density = 0.50f; sw.leap = 0.24f; sw.repeat = 0.35f;
        sw.accent = 0.30f; sw.slide = 0.06f; sw.octaveBias = 0.20f; sw.offbeatBias = 0.22f;
    } else {
        sw.cat = "other"; sw.density = 0.45f; sw.leap = 0.18f; sw.repeat = 0.45f;
        sw.stableEnd = 0.70f; sw.accent = 0.25f; sw.slide = 0.18f; sw.rootBias = 0.22f;
    }

    if (s.contains("raw"))     { sw.density += 0.08f; sw.accent += 0.08f; sw.motifVar += 0.08f; sw.choppyBias += 0.12f; }
    if (s.contains("hypno"))   { sw.repeat += 0.15f; sw.leap -= 0.08f; sw.stableEnd = 0.55f; sw.restBias = 0.10f; sw.sustainBias += 0.16f; }
    if (s.contains("minimal")) { sw.density -= 0.12f; sw.repeat += 0.12f; sw.restBias = 0.28f; }
    if (s.contains("busy"))    { sw.density += 0.18f; sw.restBias -= 0.06f; }
    if (s.contains("sparse"))  { sw.density -= 0.18f; sw.restBias += 0.12f; }
    return sw;
}

static float densityValue(int presetIdx) {
    static const float vals[] = { 0.24f, 0.36f, 0.52f, 0.66f, 0.80f, 0.92f };
    return vals[juce::jlimit(0, 5, presetIdx)];
}

static float repetitionValue(int presetIdx) {
    static const float vals[] = { 0.88f, 0.72f, 0.50f, 0.32f, 0.18f };
    return vals[juce::jlimit(0, 4, presetIdx)];
}

static int contourBias(int presetIdx) {
    static const int vals[] = { 0, 1, -1, 2, -2, 99, 100, 101 };
    return vals[juce::jlimit(0, 7, presetIdx)];
}

static float preferredStepActivation(const juce::String& cat, int idx) {
    int mod4 = idx % 4;
    if (cat == "acid") {
        static const float a[] = { 0.82f, 0.35f, 0.72f, 0.28f };
        return a[mod4];
    }
    if (cat == "techno") {
        static const float t[] = { 0.86f, 0.18f, 0.58f, 0.22f };
        return t[mod4];
    }
    if (cat == "house") {
        static const float h[] = { 0.72f, 0.48f, 0.62f, 0.34f };
        return h[mod4];
    }
    if (cat == "rave") {
        static const float r[] = { 0.78f, 0.42f, 0.68f, 0.38f };
        return r[mod4];
    }
    static const float def[] = { 0.70f, 0.35f, 0.60f, 0.30f };
    return def[mod4];
}

std::vector<int> GeneratorEngine::stableDegreesForScale(const GeneratorConfig& config) {
    const auto& s = getScaleIntervals(config.scaleIndex);
    int len = (int)s.size();
    std::vector<int> stable;
    for (int deg : {0, 2, 4, 6}) {
        if (deg < len) stable.push_back(deg);
    }
    return stable.empty() ? std::vector<int>{0} : stable;
}

std::vector<int> GeneratorEngine::getNotePool(const GeneratorConfig& config, std::mt19937& rng) {
    const auto& s = getScaleIntervals(config.scaleIndex);
    int scaleLen = (int)s.size();
    std::vector<int> pool;
    for (int i = 0; i < scaleLen; ++i) pool.push_back(i);

    int mode = config.notePoolModeIndex;
    if (mode == 0) { // Strict note pool
        pool = pickN(pool, (size_t)std::min(scaleLen, 5), rng);
    } else if (mode == 1) { // Pool with spice notes
        pool = pickN(pool, (size_t)std::min(scaleLen, 4), rng);
        pool.push_back(std::min(scaleLen - 1, scaleLen / 2));
    } else if (mode == 2) { // Pool expands on response
        pool = {0, 1, 2, 4};
    } else if (mode == 3) { // Pool contracts on response
        // full scale
    } else if (mode == 4) { // Root-heavy
        pool = {0, 0, 0, 1, 2, 4};
    } else if (mode == 5) { // Root + 5th heavy
        pool = {0, 0, 4, 4, 1, 2};
    } else if (mode == 6) { // Triad pool
        pool = {0, 2, 4};
    } else if (mode == 7) { // Seventh pool
        pool = {0, 2, 4, 6};
    }

    std::vector<int> filtered;
    for (int d : pool) {
        if (d < scaleLen) filtered.push_back(d);
    }
    return filtered.empty() ? std::vector<int>{0} : filtered;
}

static void getMotifTemplate(const GeneratorConfig& config, int steps, int& outLen,
                             std::vector<bool>& outRhythm, std::vector<int>& outContour, std::mt19937& rng) {
    const auto& motifs = GeneratorEngine::getMotifTypes();
    juce::String t = (config.motifTypeIndex >= 0 && config.motifTypeIndex < (int)motifs.size())
                     ? motifs[(size_t)config.motifTypeIndex].toLowerCase() : "acid nibble";

    outLen = (steps <= 4) ? 2 : ((steps <= 8) ? randChoice({2, 3, 4}, rng) : randChoice({3, 4}, rng));
    outRhythm.assign((size_t)outLen, false);
    outContour.assign((size_t)outLen, 0);

    if (t.contains("pulse")) {
        std::fill(outRhythm.begin(), outRhythm.end(), true);
    } else if (t.contains("offbeat")) {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = (i % 2 == 1);
    } else if (t.contains("sync")) {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = chance(i == 0 ? 0.78f : 0.55f, rng);
    } else if (t.contains("sparse") || t.contains("rest")) {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = chance(0.45f, rng);
    } else if (t.contains("dense") || t.contains("rolling")) {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = chance(0.82f, rng);
    } else if (t.contains("stab")) {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = (i == 0 || chance(0.25f, rng));
    } else {
        for (int i = 0; i < outLen; ++i) outRhythm[(size_t)i] = chance(0.65f, rng);
    }

    bool anyActive = false;
    for (bool b : outRhythm) if (b) { anyActive = true; break; }
    if (!anyActive && outLen > 0) outRhythm[0] = true;

    if (t.contains("repeated")) {
        std::fill(outContour.begin(), outContour.end(), 0);
    } else if (t.contains("stepwise rise")) {
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = i;
    } else if (t.contains("stepwise fall")) {
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = -i;
    } else if (t.contains("rise then drop")) {
        std::vector<int> pat = {0, 1, 2, 0, -1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 4)];
    } else if (t.contains("drop then rise")) {
        std::vector<int> pat = {0, -1, -2, 0, 1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 4)];
    } else if (t.contains("leap")) {
        std::vector<int> pat = {0, randChoice({2, 3, 4}, rng), 1, 0};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 3)];
    } else if (t.contains("pivot")) {
        std::vector<int> pat = {0, 1, 0, -1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 3)];
    } else if (t.contains("two-note")) {
        std::vector<int> pat = {0, 1, 0, 1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 3)];
    } else if (t.contains("three-note")) {
        std::vector<int> pat = {0, 1, 2, 1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 3)];
    } else if (t.contains("arp")) {
        std::vector<int> pat = {0, 2, 4, 2};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 3)];
    } else if (t.contains("acid") || t.contains("techno") || t.contains("house")) {
        std::vector<int> pat = {0, 0, 1, 0, -1};
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = pat[(size_t)std::min(i, 4)];
    } else {
        for (int i = 0; i < outLen; ++i) outContour[(size_t)i] = randChoice({-1, 0, 1}, rng);
    }
}

static void articulationForStep(int idx, bool active, const GeneratorConfig& config,
                                float& outGate, bool& outAccent, bool& outSlide, bool& outTie,
                                std::mt19937& rng) {
    if (!active) {
        outGate = 0.82f; outAccent = false; outSlide = false; outTie = false;
        return;
    }

    outGate = 0.82f;
    outAccent = false;
    outSlide = false;
    outTie = false;

    // Gate feels
    switch (config.gateFeelIndex) {
        case 0: outGate = 0.45f; break; // Tight
        case 1: outGate = 0.60f; break; // Short
        case 2: outGate = 0.82f; break; // Medium
        case 3: outGate = 1.15f; break; // Long
        case 4: { // Mixed
            static const float mg[] = {0.55f, 0.70f, 0.85f, 1.0f, 1.15f};
            outGate = mg[idx % 5];
            break;
        }
        case 5: outGate = 0.45f + (std::uniform_real_distribution<float>(0.0f, 0.8f)(rng)); break; // Random
    }

    // Connection feels
    switch (config.connectionFeelIndex) {
        case 0: outGate = std::min(outGate, 0.62f); break; // Detached
        case 1: outGate = juce::jlimit(0.60f, 0.85f, outGate); break; // Semi-detached
        case 2: outGate = std::max(outGate, 1.05f); break; // Legato
        case 3: outGate = std::max(outGate, 1.05f); outTie = chance(0.25f, rng); break; // Tied bias
        case 4: outTie = chance(0.18f, rng); break; // Mixed ties
        case 5: outGate = std::max(outGate, 1.05f); outTie = chance(0.12f, rng); break; // Connected runs
    }

    // Articulation profiles
    switch (config.articulationProfileIndex) {
        case 0: break; // Clean
        case 1: outAccent = chance(0.25f, rng); outGate = std::min(outGate, 0.68f); break; // Punchy
        case 2: outGate = std::max(outGate, 0.95f); outTie = outTie || chance(0.12f, rng); break; // Rolling
        case 3: outAccent = chance(0.35f, rng); outSlide = chance(0.28f, rng); outGate = std::max(outGate, 0.88f); break; // Acidic
        case 4: outGate = std::min(outGate, 0.55f); outSlide = false; break; // Choppy
        case 5: outGate = std::max(outGate, 1.05f); outSlide = chance(0.18f, rng); break; // Fluid
        case 6: outGate = std::min(outGate, 0.50f); outAccent = chance(0.35f, rng); break; // Stabbed
        case 7: outAccent = chance(0.32f, rng); outSlide = chance(0.08f, rng); break; // Tense
        case 8: outGate = std::max(outGate, 0.95f); outAccent = chance(0.12f, rng); break; // Released
    }

    // Slide profiles
    switch (config.slideProfileIndex) {
        case 0: outSlide = false; break; // None
        case 1: outSlide = chance(0.06f, rng); break; // Rare
        case 2: outSlide = chance(0.16f, rng); break; // Occasional
        case 3: outSlide = chance(0.28f, rng); break; // Acid moderate
        case 4: outSlide = chance(0.42f, rng); break; // Acid heavy
        case 5: outSlide = chance(idx % 4 >= 2 ? 0.32f : 0.04f, rng); break; // Phrase-end
        case 6: outSlide = chance(0.18f, rng); break; // Upward bias
        case 7: outSlide = chance(0.18f, rng); break; // Downward bias
    }

    // Accent profiles
    switch (config.accentProfileIndex) {
        case 0: outAccent = false; break; // None
        case 1: outAccent = chance(0.14f, rng); break; // Light
        case 2: outAccent = chance(0.24f, rng); break; // Moderate
        case 3: outAccent = chance(0.38f, rng); break; // Heavy
        case 4: outAccent = (idx % 2 == 1) && chance(0.50f, rng); break; // Offbeat
        case 5: outAccent = (idx % 4 == 0) || (idx == 0); break; // Downbeat
        case 6: outAccent = (idx % 4 == 0) || chance(0.12f, rng); break; // Motif accents
        case 7: outAccent = chance(0.22f, rng); break; // Random
    }
}

void GeneratorEngine::postProcessArticulation(std::vector<Step>& phrase, const GeneratorConfig& config, std::mt19937& rng) {
    auto sp = getStyleProfile(config);
    if (sp.cat == "acid") {
        for (size_t i = 0; i < phrase.size(); ++i) {
            auto& s = phrase[i];
            if (!s.active) continue;
            if ((i % 4 == 0 || i == 0) && chance(0.55f, rng)) s.accent = true;
            if (s.degree == 0 && chance(0.45f, rng)) s.accent = true;
            if (i + 1 < phrase.size() && phrase[i + 1].active) {
                int diff = std::abs(phrase[i + 1].degree - s.degree);
                if (diff >= 1 && diff <= 3 && chance(0.28f, rng)) s.slide = true;
                if (diff <= 2 && chance(0.16f, rng)) {
                    s.tie = true;
                    s.gate = std::max(s.gate, 1.05f);
                }
            }
            if (!s.slide && !s.tie && chance(0.35f, rng))
                s.gate = std::min(s.gate, 0.82f);
        }

        // Limit slide density
        int slides = 0;
        for (const auto& s : phrase) if (s.slide) slides++;
        int maxSlides = (int)std::ceil((float)phrase.size() / 3.0f);
        if (slides > maxSlides) {
            for (int i = (int)phrase.size() - 1; i >= 0 && slides > maxSlides; --i) {
                if (phrase[(size_t)i].slide) {
                    phrase[(size_t)i].slide = false;
                    slides--;
                }
            }
        }
    } else if (sp.cat == "techno") {
        for (size_t i = 0; i < phrase.size(); ++i) {
            auto& s = phrase[i];
            if (s.active) {
                s.slide = false;
                if (i % 4 == 0 && chance(0.42f, rng)) s.accent = true;
                if (chance(0.30f, rng)) s.gate = std::min(s.gate, 0.62f);
            }
        }
    } else if (sp.cat == "house") {
        for (size_t i = 0; i < phrase.size(); ++i) {
            auto& s = phrase[i];
            if (s.active) {
                if (i % 2 == 1 && chance(0.36f, rng)) s.accent = true;
                if (i % 2 == 1 && chance(0.40f, rng)) s.gate = std::max(s.gate, 0.92f);
                if (chance(0.10f, rng)) s.tie = true;
            }
        }
    }
}

int GeneratorEngine::phraseContour(const std::vector<Step>& phrase) {
    std::vector<int> degs;
    for (const auto& s : phrase) {
        if (s.active) degs.push_back(s.degree);
    }
    if (degs.size() < 2) return 0;
    return degs.back() - degs.front();
}

void GeneratorEngine::applyEnding(std::vector<Step>& phrase, const GeneratorConfig& config,
                                  const juce::String& phase, std::mt19937& rng) {
    if (phrase.empty()) return;
    size_t lastIdx = phrase.size() - 1;
    if (!phrase[lastIdx].active) {
        phrase[lastIdx].active = true;
        phrase[lastIdx].degree = 0;
    }

    const auto& endings = getEndingBehaviours();
    juce::String end = (config.endingBehaviourIndex >= 0 && config.endingBehaviourIndex < (int)endings.size())
                       ? endings[(size_t)config.endingBehaviourIndex] : "Response more resolved than call";

    int scaleLen = (int)getScaleIntervals(config.scaleIndex).size();
    auto stable = stableDegreesForScale(config);

    if (end.contains("Resolve to root")) {
        phrase[lastIdx].degree = 0;
    } else if (end.contains("Resolve to 5th")) {
        phrase[lastIdx].degree = std::min(4, scaleLen - 1);
    } else if (end.contains("Rise into")) {
        int prev = (lastIdx > 0 && phrase[lastIdx - 1].active) ? phrase[lastIdx - 1].degree : 0;
        phrase[lastIdx].degree = juce::jlimit(0, 10, prev + 1);
    } else if (end.contains("Fall into")) {
        int prev = (lastIdx > 0 && phrase[lastIdx - 1].active) ? phrase[lastIdx - 1].degree : 1;
        phrase[lastIdx].degree = juce::jlimit(0, 10, prev - 1);
    } else if (end.contains("Hard stop")) {
        phrase[lastIdx].gate = 0.35f;
        phrase[lastIdx].tie = false;
    } else if (end.contains("Fade energy")) {
        phrase[lastIdx].accent = false;
        phrase[lastIdx].gate = 0.55f;
    } else if (end.contains("Strong cadence")) {
        phrase[lastIdx].degree = randChoice({0, 0, 0, 2, 4}, rng);
    } else if (end.contains("Weak cadence") || end.contains("Ambiguous")) {
        phrase[lastIdx].degree = randChoice({1, 3, 5}, rng);
    }

    if (phase == "response" && end.contains("more resolved")) {
        phrase[lastIdx].degree = randChoice({0, 0, 2, 4}, rng);
    } else if (phase == "response" && end.contains("more tense")) {
        phrase[lastIdx].degree = randChoice({1, 3, 5}, rng);
    }
}

std::vector<Step> GeneratorEngine::buildCall(const GeneratorConfig& config, std::mt19937& rng) {
    auto sp = getStyleProfile(config);
    int motifLen = 4;
    std::vector<bool> motifRhythm;
    std::vector<int> motifContour;
    getMotifTemplate(config, config.steps, motifLen, motifRhythm, motifContour, rng);

    auto pool = getNotePool(config, rng);
    int steps = juce::jlimit(2, 16, config.steps);
    float densTarget = densityValue(config.densityPresetIndex);
    float repTarget = repetitionValue(config.repetitionPresetIndex);
    int contourMode = contourBias(config.contourPresetIndex);

    int center = 0;
    if (config.registerProfileIndex == 1) center = randChoice({0, 1, 2}, rng);
    else if (config.registerProfileIndex == 2) center = randChoice({0, 1, 2, 3}, rng);

    std::vector<Step> motifSteps;
    int prevDeg = center + randChoice({0, 0, 1, -1}, rng);

    for (int m = 0; m < motifLen; ++m) {
        bool act = motifRhythm[(size_t)m];
        if (chance(sp.restBias + (1.0f - densTarget) * 0.25f, rng))
            act = act && chance(0.68f, rng);

        int deg = prevDeg;
        if (m == 0 && chance(sp.rootBias, rng)) {
            deg = randChoice({0, 0, 1, 2}, rng);
        } else {
            int contour = motifContour[(size_t)m];
            if (contourMode == 100 && m > 0) {
                deg = prevDeg + randChoice({-1, 0, 1}, rng);
            } else if (contourMode == 101 && m > 0 && chance(0.35f, rng)) {
                deg = prevDeg + randChoice({-3, -2, 2, 3}, rng);
            } else if (contourMode == 1) {
                deg = prevDeg + std::max(0, contour) + randChoice({-1, 0, 1}, rng);
            } else if (contourMode == -1) {
                deg = prevDeg + std::min(0, contour) + randChoice({-1, 0, 1}, rng);
            } else if (chance(0.65f, rng)) {
                deg = prevDeg + contour + randChoice({-1, 0, 0, 1}, rng);
            } else {
                deg = randChoice(pool, rng) + randChoice({0, 0, std::max(0, center / 2)}, rng);
            }
        }

        if (chance(sp.leap * (contourMode == 101 ? 1.45f : 1.0f), rng))
            deg += randChoice({-3, 3}, rng);
        if (sp.cat == "acid" && chance(sp.syncBias, rng))
            deg += randChoice({-1, 0, 1}, rng);
        if (sp.cat == "house" && chance(sp.offbeatBias, rng) && m % 2 == 1)
            deg += randChoice({0, 1}, rng);
        if (sp.cat == "techno" && chance(sp.rootBias * 0.4f, rng))
            deg = randChoice({0, 0, 2, 4}, rng);

        int maxDeg = (config.registerProfileIndex == 2) ? 11 : ((config.registerProfileIndex == 1) ? 8 : 6);
        deg = juce::jlimit(0, maxDeg, deg);
        prevDeg = deg;

        Step st;
        st.active = act;
        st.degree = act ? deg : 0;
        articulationForStep(m, act, config, st.gate, st.accent, st.slide, st.tie, rng);
        motifSteps.push_back(st);
    }

    bool hasActive = false;
    for (const auto& s : motifSteps) if (s.active) { hasActive = true; break; }
    if (!hasActive && !motifSteps.empty()) {
        motifSteps[0].active = true;
        motifSteps[0].degree = 0;
    }

    std::vector<Step> out;
    out.reserve((size_t)steps);
    for (int i = 0; i < steps; ++i) {
        int m = i % motifLen;
        int block = i / motifLen;
        Step step = motifSteps[(size_t)m];

        if (block > 0) {
            if (!chance(repTarget, rng)) {
                if (step.active) {
                    int spread = (config.repetitionPresetIndex == 4) ? randChoice({-3, -2, -1, 0, 1, 2, 3}, rng)
                                                                    : randChoice({-2, -1, 0, 1, 2}, rng);
                    int maxDeg = (config.registerProfileIndex == 2) ? 11 : 8;
                    step.degree = juce::jlimit(0, maxDeg, step.degree + spread);
                }
                if (chance(sp.motifVar + (1.0f - repTarget) * 0.25f, rng)) {
                    step.active = !step.active ? chance(0.38f, rng) : step.active;
                }
            }
            if (i >= steps * 3 / 4 && step.active && chance(sp.stableEnd, rng)) {
                step.degree = juce::jlimit(0, 10, step.degree + randChoice({-1, 0, 1}, rng));
            }
        }

        float styleAct = preferredStepActivation(sp.cat, i);
        float dens = juce::jlimit(0.05f, 0.98f, (densTarget * 0.6f) + (styleAct * 0.4f));
        if (chance(1.0f - dens, rng)) {
            step.active = step.active && chance(dens, rng);
        }

        if (step.active && chance(sp.rootBias * 0.4f, rng) && (i % std::max(2, motifLen) == 0)) {
            step.degree = randChoice({0, 0, 2, 4}, rng);
        }
        if (sp.cat == "house" && step.active && i % 2 == 1 && chance(sp.offbeatBias, rng)) {
            step.gate = std::max(step.gate, 0.95f);
        }
        if (sp.cat == "techno" && step.active && i % 4 == 0 && chance(0.40f, rng)) {
            step.accent = true;
        }

        out.push_back(step);
    }

    if (config.repetitionPresetIndex == 0 || config.repetitionPresetIndex == 1) {
        int cloneLen = std::min(motifLen, steps / 2);
        for (int i = cloneLen; i < steps; ++i) {
            float prob = (config.repetitionPresetIndex == 0) ? 0.90f : 0.72f;
            if (chance(prob, rng)) {
                Step src = out[(size_t)(i % cloneLen)];
                articulationForStep(i, src.active, config, src.gate, src.accent, src.slide, src.tie, rng);
                out[(size_t)i] = src;
            }
        }
    }

    applyEnding(out, config, "call", rng);
    postProcessArticulation(out, config, rng);
    updateMidiNotes(out, config);
    return out;
}

std::vector<Step> GeneratorEngine::buildResponse(const std::vector<Step>& call, const GeneratorConfig& config, std::mt19937& rng) {
    int steps = juce::jlimit(2, 16, config.steps);
    std::vector<Step> out = call;
    out.resize((size_t)steps);

    const auto& strats = getResponseStrategies();
    juce::String rt = (config.responseStrategyIndex >= 0 && config.responseStrategyIndex < (int)strats.size())
                      ? strats[(size_t)config.responseStrategyIndex] : "Same rhythm + resolve";

    auto sp = getStyleProfile(config);
    int motifLen = std::max(2, std::min(4, steps / 2));
    std::vector<Step> motif(call.begin(), call.begin() + (ptrdiff_t)std::min((size_t)motifLen, call.size()));
    while ((int)motif.size() < motifLen && !call.empty()) motif.push_back(call.front());

    std::vector<int> activeDegrees;
    for (const auto& s : call) if (s.active) activeDegrees.push_back(s.degree);
    float avgDeg = activeDegrees.empty() ? 0.0f : (float)std::accumulate(activeDegrees.begin(), activeDegrees.end(), 0) / (float)activeDegrees.size();

    auto respPool = getNotePool(config, rng);
    int callCont = phraseContour(call);
    auto stable = stableDegreesForScale(config);
    std::vector<bool> copyRhythm;
    for (const auto& s : call) copyRhythm.push_back(s.active);

    auto mapMotif = [&](auto fn) {
        for (int i = 0; i < steps; ++i) {
            const auto& base = motif[(size_t)(i % motifLen)];
            out[(size_t)i] = base;
            if (out[(size_t)i].active) {
                int d = fn(base.degree, i);
                if (chance(sp.responseTightness * 0.28f, rng) && !respPool.empty()) {
                    int scaleLen = std::max(1, (int)getScaleIntervals(config.scaleIndex).size());
                    d = randChoice(respPool, rng) + (d / scaleLen) * scaleLen;
                }
                out[(size_t)i].degree = juce::jlimit(0, 10, d);
            }
        }
    };

    if (rt == "Same rhythm + resolve") {
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].active = (i < (int)copyRhythm.size()) ? copyRhythm[(size_t)i] : false;
            if (out[(size_t)i].active)
                out[(size_t)i].degree = juce::jlimit(0, 10, call[(size_t)i].degree + randChoice({-1, 0, 0, 1}, rng));
        }
    } else if (rt == "Same rhythm + new contour") {
        int dir = (callCont >= 0) ? -1 : 1;
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].active = (i < (int)copyRhythm.size()) ? copyRhythm[(size_t)i] : false;
            if (out[(size_t)i].active)
                out[(size_t)i].degree = juce::jlimit(0, 10, call[(size_t)i].degree + dir * (i / std::max(2, steps / 4)) + randChoice({-1, 0, 1}, rng));
        }
    } else if (rt == "Motif + simplify") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].active = out[(size_t)i].active && chance(i % 2 == 0 ? 1.0f : 0.35f, rng);
        }
    } else if (rt == "Motif + extend") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 1; i < steps; ++i) {
            if (!out[(size_t)i].active && out[(size_t)(i - 1)].active && chance(0.42f, rng)) {
                out[(size_t)i].active = true;
                out[(size_t)i].degree = out[(size_t)(i - 1)].degree;
                out[(size_t)i].tie = true;
                out[(size_t)i].gate = std::max(out[(size_t)i].gate, 1.0f);
            }
        }
    } else if (rt == "Motif + truncate") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = steps * 65 / 100; i < steps; ++i) {
            if (chance(0.65f, rng)) out[(size_t)i].active = false;
        }
    } else if (rt == "Echo" || rt == "Delayed answer") {
        for (int i = 0; i < steps; ++i) {
            int srcIdx = (i - 2 + steps) % steps;
            const auto& src = call[(size_t)srcIdx];
            out[(size_t)i].active = src.active;
            if (src.active)
                out[(size_t)i].degree = juce::jlimit(0, 10, src.degree + randChoice({-1, 0, 1}, rng));
        }
        if (rt == "Delayed answer" && steps > 1) {
            for (int i = steps - 1; i > 0; --i) out[(size_t)i] = out[(size_t)(i - 1)];
            out[0].active = false;
        }
    } else if (rt == "Answer upward" || rt == "Higher answer") {
        mapMotif([&](int d, int) { return d + randChoice({1, 2, 3}, rng); });
    } else if (rt == "Answer downward" || rt == "Lower answer") {
        mapMotif([&](int d, int) { return d - randChoice({1, 1, 2}, rng); });
    } else if (rt == "Invert contour" || rt == "Mirror contour loosely") {
        mapMotif([&](int d, int) { return (int)std::round(avgDeg - ((float)d - avgDeg)) + randChoice({-1, 0, 1}, rng); });
    } else if (rt == "Copy rhythm / change notes") {
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].active = (i < (int)copyRhythm.size()) ? copyRhythm[(size_t)i] : false;
            if (out[(size_t)i].active)
                out[(size_t)i].degree = juce::jlimit(0, 10, randChoice(respPool, rng) + randChoice({0, 0, 2}, rng));
        }
    } else if (rt == "Copy notes / change rhythm") {
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].degree = call[(size_t)i].degree;
            out[(size_t)i].active = chance(call[(size_t)i].active ? 0.68f : 0.22f, rng);
        }
    } else if (rt == "Transpose motif up") {
        mapMotif([&](int d, int) { return d + randChoice({2, 3, 4}, rng); });
    } else if (rt == "Transpose motif down") {
        mapMotif([&](int d, int) { return d - randChoice({2, 2, 3}, rng); });
    } else if (rt == "Response resolves to root") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = std::max(0, steps - 3); i < steps; ++i) {
            if (out[(size_t)i].active) out[(size_t)i].degree = 0;
        }
    } else if (rt == "Response resolves to 5th") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = std::max(0, steps - 3); i < steps; ++i) {
            if (out[(size_t)i].active) out[(size_t)i].degree = std::min(4, (int)getScaleIntervals(config.scaleIndex).size() - 1);
        }
    } else if (rt == "Response resolves lower") {
        mapMotif([&](int d, int) { return d - randChoice({1, 2}, rng); });
        for (int i = std::max(0, steps - 2); i < steps; ++i) {
            if (out[(size_t)i].active) out[(size_t)i].degree = juce::jlimit(0, 10, out[(size_t)i].degree - 1);
        }
    } else if (rt == "Response resolves harder") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = std::max(0, steps - 4); i < steps; ++i) {
            if (out[(size_t)i].active) {
                out[(size_t)i].degree = randChoice(stable, rng);
                out[(size_t)i].accent = true;
            }
        }
    } else if (rt == "Repeat then break") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps - 2; ++i) {
            if (chance(0.72f, rng)) out[(size_t)i] = call[(size_t)i];
        }
        if (steps >= 2 && out[(size_t)(steps - 2)].active)
            out[(size_t)(steps - 2)].degree = juce::jlimit(0, 10, out[(size_t)(steps - 2)].degree + randChoice({2, 3, -2}, rng));
        if (steps >= 1 && out[(size_t)(steps - 1)].active)
            out[(size_t)(steps - 1)].degree = randChoice(stable, rng);
    } else if (rt == "Tighter answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            if (out[(size_t)i].active) {
                out[(size_t)i].gate = 0.48f + (i % 2 == 0 ? 0.04f : 0.0f);
                out[(size_t)i].accent = chance(0.30f, rng);
            }
        }
    } else if (rt == "Sparser answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].active = out[(size_t)i].active && chance(i % 2 == 0 ? 0.85f : 0.22f, rng);
        }
    } else if (rt == "Busier answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            if (!out[(size_t)i].active && chance(0.30f, rng)) {
                out[(size_t)i].active = true;
                out[(size_t)i].degree = (i > 0) ? out[(size_t)(i - 1)].degree : randChoice(respPool, rng);
            }
        }
    } else if (rt == "Accent the answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].accent = out[(size_t)i].active && (i % 2 == 0 || chance(0.22f, rng));
        }
    } else if (rt == "Slide the answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].slide = out[(size_t)i].active && chance(0.34f, rng);
            if (out[(size_t)i].slide) out[(size_t)i].gate = std::max(out[(size_t)i].gate, 1.05f);
        }
    } else if (rt == "Staccato answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].gate = 0.42f;
            out[(size_t)i].tie = false;
        }
    } else if (rt == "Legato answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps; ++i) {
            out[(size_t)i].gate = 1.08f;
            out[(size_t)i].tie = out[(size_t)i].active && chance(0.18f, rng);
        }
    } else if (rt == "Half-phrase answer") {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
        for (int i = 0; i < steps / 2; ++i) out[(size_t)i].active = false;
    } else if (rt == "Fragment reply" || rt == "Tail answer") {
        int start = (rt == "Tail answer") ? steps / 2 : steps / 3;
        for (int i = 0; i < steps; ++i) {
            const auto& src = call[(size_t)((start + i) % steps)];
            out[(size_t)i].active = src.active && chance(0.88f, rng);
            if (out[(size_t)i].active)
                out[(size_t)i].degree = juce::jlimit(0, 10, src.degree + randChoice({-1, 0, 1}, rng));
        }
    } else if (rt == "Opposite register") {
        mapMotif([&](int d, int) { return d + (config.registerProfileIndex == 2 ? -3 : 3); });
    } else if (rt == "Same motif darker") {
        mapMotif([&](int d, int) { return d - 1; });
    } else if (rt == "Same motif brighter") {
        mapMotif([&](int d, int) { return d + 1; });
    } else if (rt == "Hook then fill") {
        for (int i = 0; i < steps; ++i) {
            if (i < steps / 2) {
                out[(size_t)i] = call[(size_t)i];
            } else {
                const auto& src = motif[(size_t)(i % motifLen)];
                out[(size_t)i] = src;
                if (out[(size_t)i].active)
                    out[(size_t)i].degree = juce::jlimit(0, 10, src.degree + randChoice({-2, -1, 1, 2}, rng));
            }
        }
    } else {
        mapMotif([&](int d, int) { return d + randChoice({-1, 0, 1}, rng); });
    }

    for (int i = 0; i < steps; ++i) {
        if (out[(size_t)i].active && i < (int)call.size() && call[(size_t)i].active && chance(sp.responseTightness, rng)) {
            out[(size_t)i].degree = juce::jlimit(0, 10, (int)std::round((float)(out[(size_t)i].degree + call[(size_t)i].degree) / 2.0f));
        }
    }

    applyEnding(out, config, "response", rng);
    postProcessArticulation(out, config, rng);
    updateMidiNotes(out, config);
    return out;
}

void GeneratorEngine::generatePair(const GeneratorConfig& config,
                                   std::vector<Step>& outCall,
                                   std::vector<Step>& outResponse,
                                   uint32_t seed) {
    std::mt19937 rng(seed != 0 ? seed : (uint32_t)juce::Random::getSystemRandom().nextInt());
    outCall = buildCall(config, rng);
    outResponse = buildResponse(outCall, config, rng);
}

void GeneratorEngine::applyPerformance(std::vector<Step>& phrase,
                                       const GeneratorConfig& config,
                                       const PerformanceParams& perf,
                                       uint32_t seed) {
    if (phrase.empty()) return;
    std::mt19937 rng(seed != 0 ? seed : 12345);

    // 1. Density modifier: if perf.density != 0.5f
    float densDiff = perf.density - 0.5f; // -0.5 to +0.5
    for (size_t i = 0; i < phrase.size(); ++i) {
        auto& s = phrase[i];
        if (densDiff < 0.0f) {
            // Remove notes (preserve downbeats)
            if (s.active && (i % 4 != 0)) {
                if (chance(-densDiff * 1.5f, rng)) s.active = false;
            }
        } else if (densDiff > 0.0f) {
            // Add notes in empty slots
            if (!s.active) {
                if (chance(densDiff * 1.4f, rng)) {
                    s.active = true;
                    s.degree = (i > 0) ? phrase[i - 1].degree : 0;
                    s.gate = 0.75f;
                }
            }
        }
    }

    // 2. Note length scaling (gate scaling)
    for (auto& s : phrase) {
        s.gate = juce::jlimit(0.1f, 2.5f, s.gate * perf.noteLength);
    }

    // 3. Octave Jump
    for (size_t i = 0; i < phrase.size(); ++i) {
        auto& s = phrase[i];
        if (s.active && perf.octaveJump > 0.0f) {
            if (chance(perf.octaveJump * 0.65f, rng)) {
                s.octaveOffset = randChoice({-12, 12, 24}, rng);
            } else {
                s.octaveOffset = 0;
            }
        } else {
            s.octaveOffset = 0;
        }
    }

    // 4. Resolve control
    if (perf.resolve > 0.7f && !phrase.empty()) {
        phrase.back().active = true;
        phrase.back().degree = 0;
        phrase.back().accent = true;
        phrase.back().octaveOffset = 0;
    } else if (perf.resolve < 0.3f && !phrase.empty()) {
        phrase.back().degree = std::min(4, (int)getScaleIntervals(config.scaleIndex).size() - 1);
    }

    updateMidiNotes(phrase, config);
}

void GeneratorEngine::mutatePhrase(std::vector<Step>& phrase,
                                   const GeneratorConfig& config,
                                   const juce::String& target,
                                   int amountIndex,
                                   std::mt19937& rng) {
    if (phrase.empty()) return;
    static const int amts[] = { 1, 2, 3, 5, 8 };
    int amt = amts[juce::jlimit(0, 4, amountIndex)];
    int n = (int)phrase.size();

    if (target == "Ending only") {
        int startEnd = std::max(0, n - std::min(amt + 1, n));
        for (int i = startEnd; i < n; ++i) {
            auto& s = phrase[(size_t)i];
            if (chance(0.65f, rng)) {
                s.active = true;
                int cadenceDegree = randChoice({0, 4, 1, 0}, rng);
                s.degree = juce::jlimit(0, 11, cadenceDegree);
                s.gate = (i == n - 1) ? 1.0f : 0.75f;
                s.accent = (i == n - 1);
            }
        }
    } else if (target == "Motif only") {
        int endMotif = std::min(n, std::max(2, amt + 1));
        for (int i = 0; i < endMotif; ++i) {
            auto& s = phrase[(size_t)i];
            if (s.active) {
                s.degree = juce::jlimit(0, 11, s.degree + randChoice({-2, -1, 1, 2}, rng));
                if (chance(0.4f, rng)) s.accent = !s.accent;
                if (chance(0.3f, rng)) s.slide = !s.slide;
            } else if (chance(0.35f, rng)) {
                s.active = true;
                s.degree = randChoice({0, 2, 4}, rng);
                s.gate = 0.82f;
            }
        }
    } else if (target == "Rhythm only") {
        std::vector<int> idxs;
        for (int i = 0; i < n; ++i) idxs.push_back(i);
        idxs = pickN(idxs, (size_t)std::min(amt, n), rng);
        for (int i : idxs) {
            auto& s = phrase[(size_t)i];
            if (s.active) {
                if (chance(0.6f, rng)) {
                    s.active = false;
                } else if (i + 1 < n && !phrase[(size_t)(i + 1)].active) {
                    std::swap(s, phrase[(size_t)(i + 1)]);
                }
            } else {
                if (chance(0.6f, rng)) {
                    s.active = true;
                    s.gate = 0.82f;
                    s.accent = chance(0.25f, rng);
                    s.degree = (i > 0 && phrase[(size_t)(i - 1)].active) ? phrase[(size_t)(i - 1)].degree : 0;
                }
            }
        }
    } else if (target == "Pitch only") {
        std::vector<int> activeIdxs;
        for (int i = 0; i < n; ++i) {
            if (phrase[(size_t)i].active) activeIdxs.push_back(i);
        }
        if (activeIdxs.empty()) {
            phrase[0].active = true;
            phrase[0].degree = 0;
            activeIdxs.push_back(0);
        }
        activeIdxs = pickN(activeIdxs, (size_t)std::min(amt, (int)activeIdxs.size()), rng);
        for (int i : activeIdxs) {
            auto& s = phrase[(size_t)i];
            s.degree = juce::jlimit(0, 11, s.degree + randChoice({-3, -2, -1, 1, 2, 3}, rng));
        }
    } else if (target == "Articulation only") {
        std::vector<int> activeIdxs;
        for (int i = 0; i < n; ++i) {
            if (phrase[(size_t)i].active) activeIdxs.push_back(i);
        }
        if (!activeIdxs.empty()) {
            activeIdxs = pickN(activeIdxs, (size_t)std::min(amt, (int)activeIdxs.size()), rng);
            for (int i : activeIdxs) {
                auto& s = phrase[(size_t)i];
                s.gate = randChoiceFloat({0.35f, 0.65f, 0.85f, 1.25f}, rng);
                s.accent = chance(0.35f, rng);
                s.slide = chance(0.30f, rng);
            }
        }
    } else {
        // "Whole phrase", "Call only", "Response only", "Call + response linked"
        std::vector<int> idxs;
        for (int i = 0; i < n; ++i) idxs.push_back(i);
        idxs = pickN(idxs, (size_t)std::min(amt, n), rng);
        for (int i : idxs) {
            auto& s = phrase[(size_t)i];
            if (s.active) {
                if (chance(0.25f, rng)) {
                    s.active = false;
                } else {
                    s.degree = juce::jlimit(0, 11, s.degree + randChoice({-2, -1, 1, 2}, rng));
                    s.gate = randChoiceFloat({0.4f, 0.75f, 1.0f}, rng);
                    if (chance(0.35f, rng)) s.accent = !s.accent;
                    if (chance(0.25f, rng)) s.slide = !s.slide;
                }
            } else {
                if (chance(0.45f, rng)) {
                    s.active = true;
                    s.degree = randChoice({0, 2, 4, 7}, rng);
                    s.gate = 0.82f;
                    s.accent = chance(0.3f, rng);
                }
            }
        }
    }

    updateMidiNotes(phrase, config);
}

void GeneratorEngine::shift(std::vector<Step>& phrase, int delta) {
    if (phrase.empty() || delta == 0) return;
    int n = (int)phrase.size();
    delta = (delta % n + n) % n;
    std::rotate(phrase.begin(), phrase.begin() + delta, phrase.end());
}

void GeneratorEngine::invert(std::vector<Step>& phrase, const GeneratorConfig& config) {
    std::vector<int> degs;
    for (const auto& s : phrase) if (s.active) degs.push_back(s.degree);
    if (degs.empty()) return;
    float avg = (float)std::accumulate(degs.begin(), degs.end(), 0) / (float)degs.size();
    for (auto& s : phrase) {
        if (s.active) s.degree = juce::jlimit(0, 10, (int)std::round(avg - ((float)s.degree - avg)));
    }
    updateMidiNotes(phrase, config);
}

void GeneratorEngine::reverse(std::vector<Step>& phrase) {
    std::reverse(phrase.begin(), phrase.end());
}

void GeneratorEngine::transposeOctave(std::vector<Step>& phrase, int octaves) {
    int delta = octaves * 12;
    for (auto& s : phrase) {
        if (s.active) s.midiNote = juce::jlimit(0, 127, s.midiNote + delta);
    }
}

void GeneratorEngine::transposePhraseToScale(std::vector<Step>& phrase,
                                             const GeneratorConfig& oldConfig,
                                             const GeneratorConfig& newConfig) {
    if (phrase.empty()) return;

    bool scaleChanged = (oldConfig.scaleIndex != newConfig.scaleIndex);
    bool keyChanged   = (oldConfig.keyIndex != newConfig.keyIndex);
    bool octChanged   = (oldConfig.rootOctaveIndex != newConfig.rootOctaveIndex);

    if (!scaleChanged && !keyChanged && !octChanged) return;

    const auto& newIntervals = getScaleIntervals(newConfig.scaleIndex);
    int newRoot = rootMidi(newConfig);

    // Calculate key / octave shift
    int keyDelta = newConfig.keyIndex - oldConfig.keyIndex;
    if (keyDelta > 6) keyDelta -= 12;
    else if (keyDelta < -6) keyDelta += 12;
    int octDelta = (newConfig.rootOctaveIndex - oldConfig.rootOctaveIndex) * 12;
    int totalShift = keyDelta + octDelta;

    for (auto& s : phrase) {
        if (!s.active) continue;

        int shiftedMidi = juce::jlimit(0, 127, s.midiNote + totalShift);

        // Calculate pitch class relative to newRoot
        int diff = shiftedMidi - newRoot;
        int oct = (diff >= 0) ? (diff / 12) : ((diff - 11) / 12);
        int pitchClass = ((diff % 12) + 12) % 12;

        bool inScale = (std::find(newIntervals.begin(), newIntervals.end(), pitchClass) != newIntervals.end());
        int targetPc = pitchClass;

        if (!inScale) {
            auto hasInterval = [&](int pc) {
                return std::find(newIntervals.begin(), newIntervals.end(), pc) != newIntervals.end();
            };

            if (pitchClass == 3 && hasInterval(4)) targetPc = 4;
            else if (pitchClass == 4 && hasInterval(3)) targetPc = 3;
            else if (pitchClass == 8 && hasInterval(9)) targetPc = 9;
            else if (pitchClass == 9 && hasInterval(8)) targetPc = 8;
            else if (pitchClass == 10 && hasInterval(11)) targetPc = 11;
            else if (pitchClass == 11 && hasInterval(10)) targetPc = 10;
            else if (pitchClass == 1 && hasInterval(2)) targetPc = 2;
            else if (pitchClass == 2 && hasInterval(1)) targetPc = 1;
            else if (pitchClass == 6 && hasInterval(7)) targetPc = 7;
            else {
                int bestPc = newIntervals[0];
                int minDiff = 999;
                for (int pc : newIntervals) {
                    int d = std::abs(pc - pitchClass);
                    if (d < minDiff || (d == minDiff && pc > bestPc)) {
                        minDiff = d;
                        bestPc = pc;
                    }
                }
                targetPc = bestPc;
            }
        }

        s.midiNote = juce::jlimit(0, 127, newRoot + oct * 12 + targetPc);
        s.degree = midiToDegree(s.midiNote, newConfig);
    }
}

GeneratorEngine::GeneratorEngine() = default;

} // namespace CR
