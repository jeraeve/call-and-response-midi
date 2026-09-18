#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

namespace CR {

class SynthEngine {
public:
    SynthEngine() = default;

    void prepare(double newSampleRate) {
        sampleRate = (newSampleRate > 0.0) ? newSampleRate : 44100.0;
        reset();
    }

    void reset() {
        phase = 0.0;
        currentFreq = 220.0;
        targetFreq = 220.0;
        slideSpeed = 0.0;
        envLevel = 0.0f;
        gateActive = false;
        isSliding = false;
        filterState[0] = filterState[1] = filterState[2] = filterState[3] = 0.0f;
    }

    void noteOn(int midiNote, float velocity, bool accent, bool slide) {
        float freq = 440.0f * std::pow(2.0f, (float)(midiNote - 69) / 12.0f);
        targetFreq = freq;
        isAccented = accent;

        if (gateActive && slide) {
            // Legato slide: keep envelope alive, glide frequency
            isSliding = true;
            slideSpeed = (targetFreq - currentFreq) / (float)(sampleRate * 0.06); // 60ms slide
        } else {
            // New note trigger
            currentFreq = freq;
            isSliding = false;
            envLevel = 1.0f;
            gateActive = true;
        }
        noteVelocity = velocity;
    }

    void noteOff(int midiNote) {
        juce::ignoreUnused(midiNote);
        gateActive = false;
    }

    void setParameters(float cutoffHz, float resonance, float decayTimeSec, float drive) {
        targetCutoff = juce::jlimit(40.0f, 16000.0f, cutoffHz);
        filterRes = juce::jlimit(0.0f, 0.96f, resonance);
        decayRate = (decayTimeSec > 0.01f) ? (1.0f / (float)(sampleRate * (double)decayTimeSec)) : 0.001f;
        distortion = juce::jlimit(1.0f, 5.0f, drive);
    }

    void setMuted(bool mute) { muted = mute; }
    bool isMuted() const { return muted; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
        if (muted) return;

        auto* left = buffer.getWritePointer(0);
        auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer(1) : nullptr;

        for (int i = 0; i < numSamples; ++i) {
            // Slide frequency update
            if (isSliding) {
                currentFreq += slideSpeed;
                if ((slideSpeed > 0.0 && currentFreq >= targetFreq) ||
                    (slideSpeed < 0.0 && currentFreq <= targetFreq)) {
                    currentFreq = targetFreq;
                    isSliding = false;
                }
            }

            // Envelope decay
            if (gateActive) {
                envLevel = std::max(0.15f, envLevel - decayRate);
            } else {
                envLevel = std::max(0.0f, envLevel - decayRate * 4.0f);
            }

            if (envLevel <= 0.0001f && !gateActive) {
                phase = 0.0;
                continue;
            }

            // Oscillator: Band-limited PolyBLEP Sawtooth
            double dt = currentFreq / sampleRate;
            phase += dt;
            if (phase >= 1.0) phase -= 1.0;

            float rawSaw = (float)(2.0 * phase - 1.0);
            float blep = 0.0f;
            if (phase < dt) {
                double t = phase / dt;
                blep = (float)(t + t - t * t - 1.0);
            } else if (phase > 1.0 - dt) {
                double t = (phase - 1.0) / dt;
                blep = (float)(t * t + t + t + 1.0);
            }
            float osc = rawSaw - blep;

            // 4-pole Ladder Filter with cutoff modulated by envelope and accent
            float cutoffMod = targetCutoff + (isAccented ? 3500.0f : 1800.0f) * envLevel;
            cutoffMod = juce::jlimit(60.0f, (float)(sampleRate * 0.45), cutoffMod);

            float f = 2.0f * (float)std::sin(juce::MathConstants<double>::pi * (double)(cutoffMod / (float)sampleRate));
            f = juce::jlimit(0.01f, 0.99f, f);
            float fb = filterRes * (1.0f + 0.5f * (1.0f - f));

            float in = osc - fb * filterState[3];
            filterState[0] += f * (std::tanh(in) - filterState[0]);
            filterState[1] += f * (filterState[0] - filterState[1]);
            filterState[2] += f * (filterState[1] - filterState[2]);
            filterState[3] += f * (filterState[2] - filterState[3]);
            float filtered = filterState[3];

            // Drive / saturation
            float driven = std::tanh(filtered * distortion);

            // Amplitude shaping
            float amp = driven * envLevel * noteVelocity * (isAccented ? 1.3f : 1.0f) * 0.35f;

            int sampleIdx = startSample + i;
            left[sampleIdx] += amp;
            if (right != nullptr) right[sampleIdx] += amp;
        }
    }

private:
    double sampleRate = 44100.0;
    double phase = 0.0;
    double currentFreq = 220.0;
    double targetFreq = 220.0;
    double slideSpeed = 0.0;
    bool isSliding = false;

    float envLevel = 0.0f;
    float decayRate = 0.001f;
    bool gateActive = false;
    bool isAccented = false;
    float noteVelocity = 0.8f;

    float targetCutoff = 800.0f;
    float filterRes = 0.70f;
    float distortion = 1.8f;
    float filterState[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    bool muted = false;
};

} // namespace CR
