# Call & Response MIDI (v2.0.0)

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**  
*Formats: VST3, Audio Unit (AU), Standalone | macOS Universal (Mojave Intel & Apple Silicon) & Windows 64-bit*

---

## Attributions & Acknowledgements

Call & Response MIDI stands on the shoulders of creative musical innovators:

- **Nordl3**: Deep gratitude and attribution to **Nordl3** for the original project concept and pioneering the algorithmic foundations of automated dual-phrase musical dialogue.
- **SideBrain**: Sincere appreciation and attribution to **SideBrain** for his renowned Max for Live implementation, creative workflow concepts, and dedication to sharing innovative composition tools with the music production community.

### Evolution by Viral Samples (v2.0.0)
Viral Samples evolved and expanded these foundational ideas into a native, high-performance C++/JUCE instrument for macOS and Windows:
- Full **Audio Unit (AU)**, **VST3**, and **Standalone** 64-bit binaries.
- **Universal macOS Binary**: Native compatibility with macOS 10.14 Mojave Intel (`x86_64`) through macOS Sonoma/Sequoia & Apple Silicon (`arm64`: M1, M2, M3, M4).
- **Native Windows 64-bit Binary**: Fully self-contained portable VST3 and Standalone executable with static runtime linking.
- **Brand New Ableton-Inspired Interface**: Beautiful dark theme with high-contrast elements, fixed aspect-ratio scaling (1180 × 750), and ergonomic workflow.
- **Interactive Top Ribbon Bar**: Direct value display boxes for `OCT`, `KEY`, `SCALE`, `b/#`, `STEPS`, `RATE`, and `SWING` with dual dropdown & stepper controls.
- **Tempo-Relative Playback RATE**: Choose between `Half speed`, `0` (Normal tempo sync), and `Double speed`.
- **Continuous SWING & VELOCITY Dragging**: Intuitive vertical click-and-drag for seamless groove swing (0% – 100%) and velocity (1 – 127).
- **Dual Phrase Lanes**:
  - Independent **MUTE** bypass buttons for Call and Response.
  - Dedicated per-lane **DICE** mutators with modifiable **MUTATE xx%** depth.
  - Per-lane **LOCK** padlocks protecting phrases from global regenerations.
  - Color-coded visual harmony (Orange for Call, Teal for Response).
- **Four-Tab Inspector Panel**:
  - **GENERATE**: Style (40 styles), Motif (24 types), Strategy (38 types), Mode, and Ending with individual vector lock padlocks.
  - **SHAPE**: Gate Length (0.25× – 2.0×), continuous click-and-drag Velocity (1 – 127), Resolve, and Drive.
  - **ARTICULATION**: Accent Boost, Slide Time, Filter Cutoff (60 Hz – 14 kHz), Resonance, and Decay Time.
  - **OPTIONS**: Routing (Channels 1–16, Combined / Split / Layered) and Timing (Sync Mode: Timeline, Restart, Free).
- **Enhanced Piano Roll**: Repeatable right-edge gate resizing up to 4.0× note length, step auditioning with automatic mouse release stop, pitch transposition, and note editing.
- **Integrated 303 Analog Synth**: Resonant low-pass filter, fast envelope decay, and saturation drive for immediate zero-latency previews.
- **8 Rapid Preset Slots (A–H)**: Instant `Cmd+Click` saving, `Double-Click` loading, and right-click context menus.
- **Multi-Level Undo/Redo & Panic**: 24-step non-destructive history and global All-Notes-Off MIDI Panic button.
- **Drag & Drop MIDI Export**: Export generated phrases directly into DAW tracks.

---

## Feature Summary

| Category | Highlights |
|---|---|
| **Musical Engine** | 35 Scales (Diatonic, Pentatonic, Blues, Exotic, Arp Pools), 40 Musical Styles, 38 Response Strategies, 24 Opening Motif types. |
| **Transposition** | Real-time automatic note conforming on Key/Scale/Octave change. Preserves modal intervals and harmonic integrity. |
| **Top Ribbon** | Direct value displays for `OCT`, `KEY`, `SCALE`, `b/#`, `STEPS`, `RATE`, `SWING` with dropdown menus and stepper arrows. |
| **Performance** | Continuous SWING (0% – 100%) and VELOCITY (1 – 127) click-and-drag, tempo-relative RATE (Half, 0, Double), Density, Octave Jump. |
| **Phrase Lanes** | Independent Mute, Dice, modifiable Mutate %, Lock padlocks, Octave transpose, Inversion, Reversal, Copy, and Channel routing. |
| **Piano Roll** | Color-coded note blocks matching lane themes, repeatable right-edge gate resizing up to 4.0×, step auditioning auto-stop. |
| **Inspector** | 4 tabs (GENERATE, SHAPE, ARTICULATION, OPTIONS) with individual parameter locks for targeted procedural composition. |
| **303 Synth** | Built-in analog-modelled sawtooth/square oscillator, resonant 24dB low-pass filter, and snappy decay envelope. |
| **Presets & History**| 8 quick-recall slots (A–H) with save/load indicators, 24-step undo/redo queue, global MIDI Panic button. |
| **MIDI Export** | Drag & Drop directly into DAW arrangements with Combined, Call Only, Response Only, or Layered multi-channel routing. |
| **Compatibility** | macOS Universal (Mojave 10.14+ Intel & Apple Silicon M1–M4); Windows 10/11 (64-bit VST3 & Standalone). |

---

## Installation & Setup

### macOS Universal Installation (Recommended)
Double-click **`Call & Response MIDI v2.0.0 Installer.pkg`** to install:
- **VST3 Plugin**: `/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3`
- **Audio Unit (AU)**: `/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component`
- **User Documentation**: `/Library/Audio/Documentation/Viral Samples/Call & Response MIDI/`

The installer automatically de-quarantines the files, applies code signatures, sets POSIX permissions, and flushes `AudioComponentRegistrar` so DAWs (Logic Pro, Ableton Live, FL Studio, Studio One, Cubase, Reaper) recognize the plugin immediately.

### Windows Installation
Run **`Call & Response MIDI v2.0.0 Windows Setup.exe`**:
- **VST3 Plugin (64-bit)**: `C:\Program Files\Common Files\VST3\Call & Response MIDI.vst3`
- **Standalone App**: `C:\Program Files\Viral Samples\Call & Response MIDI\Call & Response MIDI.exe`
- **Documentation**: `C:\Program Files\Viral Samples\Call & Response MIDI\Documentation\`

---

## Quick Reference / Key Commands

| Action | Gesture / Shortcut |
|---|---|
| **Save Preset to Slot A–H** | `Cmd+Click` (or `Shift+Click` / `Alt+Click` / `Ctrl+Click`) on slot button |
| **Load Preset from Slot A–H** | `Double-Click` on slot button |
| **Preset Context Menu** | `Right-Click` on slot button |
| **Continuous Swing Drag** | Click and drag vertically on `SWING` display box |
| **Continuous Velocity Drag**| Click and drag vertically on `VELOCITY` display in Inspector |
| **Add / Delete Note in Piano Roll** | `Double-Click` on grid cell or existing note |
| **Move Note (Pitch & Step)** | Click note center and drag horizontally or vertically |
| **Resize Note Gate Duration** | Hover right edge of note (`↔` cursor) and drag horizontally |
| **Audition Step** | Click note (audition stops automatically when mouse is released) |
| **Audition Pitch** | Click piano keys along the left edge of the Piano Roll |
| **Toggle Accent / Slide** | Right-Click note for Accent (`▲`); Alt-Click note for Slide (`~`) |
| **Mute Lane** | Click `MUTE` on CALL or RESPONSE lane |
| **Lock Lane** | Click padlock icon on CALL or RESPONSE lane header |
| **Parameter Lock / Unlock** | Click padlock icon next to Style, Strategy, Motif, Mode, or Ending in Inspector |
| **Offset Notes** | Click rectangular `[ < ]` or `[ > ]` buttons under Left Panel rotaries |

---

*Call & Response MIDI v2.0.0 &bull; Copyright &copy; 2026 Viral Samples. All rights reserved.*
