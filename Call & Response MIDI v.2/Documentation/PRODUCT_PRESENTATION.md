# Call & Response MIDI (v2.0.0)
### Intelligent Algorithmic Phrase Generator & 303-Style Sequencer
**By Viral Samples**

---

## Overview

**Call & Response MIDI** is a generative phrase synthesizer, performance sequencer, and MIDI composition utility designed to bring structured musical conversation to electronic music production.

Unlike conventional randomizers or arpeggiators that produce disconnected note sequences, Call & Response MIDI operates on the foundational principle of musical dialogue: an opening phrase (**Call**) establishes the motif, and a complementary counter-phrase (**Response**) delivers the harmonic, modal, or rhythmic resolution.

The result is focused, coherent musical hooks, acid basslines, leads, and ostinatos—generated in seconds and shaped with complete procedural control.

---

## Key Features

### 1. Dual-Phrase Dialogue Architecture
- **Independent Call & Response Lanes**: Dedicated lanes with individual mute switches, octave transpositions, phrase inversion, and retrograde reversal.
- **Selective Mutation & Dice**: Dedicated per-lane DICE mutators with adjustable mutation depth percentage (`MUTATE %`), allowing subtle phrasing variations without destroying the original hook.
- **Phrase Locking**: Individual padlock controls freeze either lane to protect favorite melodies during global rolls.

### 2. Modal Engine & Procedural Composition
- **35 Musical Scales**: Full support for Diatonic, Pentatonic, Blues, Exotic, and Arpeggio pools with real-time automatic conforming on key or scale changes.
- **Four-Tab Inspector Panel**:
  - **Generate**: 40 musical styles, 24 motif lead-in hooks, 38 response strategies, mode routing, and cadence endings with individual vector locks.
  - **Shape**: Gate length scaling (0.25× to 2.0×), continuous velocity control (1 – 127), harmonic resolve, and overdrive.
  - **Articulation**: TB-303-style accent boost, portamento slide time, filter cutoff (60 Hz – 14 kHz), resonance, and envelope decay.
  - **Options**: Flexible channel routing (Combined, Split, or Layered) and DAW timeline synchronization modes.

### 3. Tactile Performance & Modern Interface
- **Fixed Aspect-Ratio Interface**: Scalable, high-contrast dark theme calibrated for studio monitors and laptops without distortion or clipped controls.
- **Direct-Access Top Ribbon**: Live value readouts for Octave, Key, Scale, Accidental (b/#), Steps, Rate, and Swing, controllable via dropdown menus or stepper arrows.
- **Tempo-Relative Playback Rate**: Switch seamlessly between Half speed (0.5×), Normal (1.0× 16th-note sync), and Double speed (2.0×).
- **Continuous Modulation Dragging**: Intuitive vertical click-and-drag editing for Swing (0% – 100%) and Velocity (1 – 127).
- **Nudge & Step Shifting**: Dedicated `[ < ] OFFSET [ > ]` buttons to advance or retard note patterns by single steps.

### 4. Interactive Piano Roll & Built-in 303 Preview Engine
- **Visual Color-Coded Matrix**: Lane-matched note blocks (Orange for Call, Teal for Response) with double-click note editing and step auditioning that automatically silences upon mouse release.
- **Repeatable Gate Resizing**: Click and drag the right edge of any note up to 4.0× step duration.
- **Integrated Analog-Modelled Synth**: Built-in 303-inspired monophonic synthesizer featuring a resonant 24 dB low-pass filter, snappy envelope decay, and saturation—enabling zero-latency previews without wiring auxiliary virtual instruments.

### 5. Seamless DAW Integration & Presets
- **Direct MIDI Drag & Drop**: Export generated phrases directly onto DAW MIDI tracks.
- **8 Rapid-Access Preset Slots (A–H)**: Single-click recall, keyboard shortcut saving, and non-destructive 24-step undo/redo history.
- **Global Panic Button**: Instant All-Notes-Off reset across all 16 MIDI channels.

---

## Technical Specifications & System Requirements

| Specification | macOS | Windows |
|---|---|---|
| **Operating System** | macOS 10.14 (Mojave) or later (Catalina, Big Sur, Monterey, Ventura, Sonoma, Sequoia) | Windows 10 or Windows 11 (64-bit) |
| **Architectures** | Universal Binary: Native Apple Silicon (`arm64`: M1, M2, M3, M4) & Intel (`x86_64`) | 64-bit Intel / AMD x86-64 |
| **Plugin Formats** | VST3, Audio Unit (AU v2), Standalone Application | VST3, Standalone Application (`.exe`) |
| **Validation** | 100% Passed Apple `auval` (Audio Unit Validation Tool) | Standard VST3 Validator Compliant |
| **DAW Compatibility** | Ableton Live, Logic Pro, FL Studio, Studio One, Cubase, Reaper, Bitwig Studio | Ableton Live, FL Studio, Studio One, Cubase, Reaper, Bitwig Studio, Cakewalk |
| **Installation** | Dedicated `.pkg` installer with automated permissions & AU cache registration | Portable NSIS installer (`Setup.exe`) with static C++ runtime linking |

---

## Attributions

Call & Response MIDI stands on the shoulders of creative musical innovators:
- **Nordl3**: Original concept and pioneering algorithmic dual-phrase musical dialogue.
- **SideBrain**: Renowned Max for Live implementation and workflow concepts.
- **Viral Samples**: Native cross-platform C++/JUCE architecture, modern UI, analog synth engine, and expanded feature set.

---
*Copyright © 2026 Viral Samples. All rights reserved.*
