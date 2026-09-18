# Call & Response MIDI (v2.0.0)

[![Release](https://img.shields.io/badge/release-v2.0.0-00d2ff.svg)](https://github.com/jeraeve/call-and-response-midi/releases/tag/v2.0.0)
[![macOS](https://img.shields.io/badge/macOS-Mojave_10.14+_%7C_Apple_Silicon_&_Intel-brightgreen.svg)]()
[![Windows](https://img.shields.io/badge/Windows-10_%2F_11_64--bit-blue.svg)]()
[![Plugin Formats](https://img.shields.io/badge/formats-VST3_%7C_AU_%7C_Standalone-orange.svg)]()
[![AU Validation](https://img.shields.io/badge/auval-100%25_PASSED-success.svg)]()

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**  
*Formats: VST3, Audio Unit (AU), Standalone | macOS Universal (Mojave Intel & Apple Silicon) & Windows 64-bit*

---

## 🎵 Overview & Architecture

**Call & Response MIDI v2.0.0** is an advanced dual-lane algorithmic melody generator, performance sequencer, and MIDI composition instrument inspired by classic Roland TB-303 phrasing and the universal musical conversation of *Call & Response*.

Instead of random disjointed notes, Call & Response MIDI generates pairs of musical phrases that converse with one another:
- **Call Lane**: The theme, question, or melodic hook.
- **Response Lane**: The counter-melody, answer, or harmonic resolution shaped by sophisticated modal and rhythmic response strategies.

A built-in analog-modelled 303 preview synth with resonant low-pass filter, fast envelope decay, and drive saturation allows instant auditioning without having to wire a virtual instrument first.

---

## 🙏 Attributions & Acknowledgements

Call & Response MIDI stands on the shoulders of creative musical innovators:

- **Nordl3**: Deep gratitude and attribution to **Nordl3** for conceptualizing and developing the original foundational algorithmic Call and Response music generation system.
- **SideBrain**: Sincere appreciation and attribution to **SideBrain** for his renowned Max for Live implementation, creative workflow concepts, and dedication to sharing innovative composition tools with the music production community.

### Evolution by Viral Samples (v2.0.0)
Viral Samples evolved and expanded these foundational ideas into a native, high-performance C++/JUCE instrument for macOS and Windows:
- **Universal macOS Binary**: Native support for **macOS Mojave 10.14+ Intel (`x86_64`)** through macOS Sonoma/Sequoia and **Apple Silicon (`arm64`: M1, M2, M3, M4)**.
- **Native Windows 64-bit Binary**: Self-contained VST3 and Standalone `.exe` with static runtime linking.
- **Modern Dark UI**: Complete visual redesign inspired by modern digital audio workstations with fixed aspect-ratio scaling ($1180 \times 750$).
- **Direct Ribbon Displays**: Value boxes for `OCT`, `KEY`, `SCALE`, `b/#`, `STEPS`, `RATE`, and `SWING` with dropdown menus and arrow steppers.
- **Tempo-Relative RATE**: Play back at `Half speed`, `0` (Normal 16th sync), or `Double speed`.
- **Continuous SWING & VELOCITY Dragging**: Smooth click-and-drag continuous modulation for groove swing ($0\%–100\%$) and note velocity ($1–127$).
- **Dual Phrase Lanes**:
  - Independent **MUTE** bypass toggles for Call and Response.
  - Dedicated per-lane **DICE** mutators with modifiable **MUTATE xx%** depth.
  - Per-lane **LOCK** padlocks protecting phrases from global regenerations.
  - Octave transpositions, phrase inversion, retrograde reversal, copy, and channel routing.
- **Four-Tab Inspector Panel**:
  - **GENERATE**: Style (40 styles), Motif (24 types), Strategy (38 types), Mode, and Ending with individual vector lock padlocks.
  - **SHAPE**: Gate Length ($0.25\times–2.0\times$), continuous Velocity ($1–127$), Resolve, and Drive.
  - **ARTICULATION**: Accent Boost, Slide Time, Filter Cutoff ($60\,\text{Hz}–14\,\text{kHz}$), Resonance, and Decay Time.
  - **OPTIONS**: Routing (Channels 1–16, Combined / Split / Layered) and Timing (Sync Mode: Timeline, Restart, Free).
- **Interactive Piano Roll**: Color-coded note blocks matching lane themes (Orange for Call, Teal for Response), repeatable right-edge gate resizing up to $4.0\times$, step auditioning with automatic mouse release stop, pitch transposition, and note editing.
- **8 Rapid Preset Slots (A–H)**: Instant `Cmd+Click` saving, `Double-Click` loading, and right-click context menus.
- **Multi-Level Undo/Redo & Panic**: Non-destructive history queue and global All-Notes-Off MIDI Panic reset.
- **Drag & Drop MIDI Export**: Export generated phrases directly into DAW arrangement tracks.

---

## 📦 Downloads & Releases

Grab the latest installer packages from the [GitHub Releases](https://github.com/jeraeve/call-and-response-midi/releases):

| Platform | Installer Package | Contents | Compatibility |
|---|---|---|---|
| **macOS** | [`Call & Response MIDI v2.0.0 Installer.pkg`](https://github.com/jeraeve/call-and-response-midi/releases/download/v2.0.0/Call%20&%20Response%20MIDI%20v2.0.0%20Installer.pkg) | VST3, Audio Unit (AU), Docs | macOS 10.14 Mojave through Sequoia (Apple Silicon & Intel) |
| **Windows** | [`Call & Response MIDI v2.0.0 Windows Setup.exe`](https://github.com/jeraeve/call-and-response-midi/releases/download/v2.0.0/Call%20&%20Response%20MIDI%20v2.0.0%20Windows%20Setup.exe) | 64-bit VST3, Standalone App, Docs | Windows 10 & 11 (64-bit x86-64) |
| **User Manual** | [`User Manual.pdf`](https://github.com/jeraeve/call-and-response-midi/releases/download/v2.0.0/User%20Manual.pdf) | Comprehensive PDF guide | All platforms |
| **Read Me** | [`Read Me.pdf`](https://github.com/jeraeve/call-and-response-midi/releases/download/v2.0.0/Read%20Me.pdf) | Summary & quick reference | All platforms |

---

## 🎹 Quick Key Commands

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

## 🛠️ Building From Source

### Prerequisites
- **CMake** 3.22 or higher
- **Ninja** or Xcode / Visual Studio
- **C++20** compatible compiler (Apple Clang 12+, GCC 12+, MSVC 2022)

### macOS Universal Build
```bash
cd "Call & Response MIDI v.2"
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja -C build
./build_installer.sh
```

### Windows Build (Native MSVC)
```cmd
cd "Call & Response MIDI v.2"
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
makensis windows/installer.nsi
```

### Windows Cross-Compilation (macOS with MinGW)
```bash
cd "Call & Response MIDI v.2"
cmake -B build-win -G "Ninja" -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="cmake/mingw-w64-x86_64.cmake"
ninja -C build-win
makensis windows/installer.nsi
```

---

## 📄 License & Attribution

- Built with **JUCE** framework.
- Algorithmic concept and dialogue design attributed to **Nordl3** & **SideBrain**.
- Enhanced implementation, 303 synthesis engine, custom UI, and installers &copy; 2026 **Viral Samples**. All rights reserved.
