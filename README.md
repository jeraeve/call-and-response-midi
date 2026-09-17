# Call & Response MIDI (v1.0.5)

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**  
*Formats: VST3, Audio Unit (AU), Standalone | Universal Binary (Apple Silicon & Intel)*

---

## Attributions & Acknowledgements

Call & Response MIDI stands on the shoulders of creative musical innovators:

- **Nordl3**: Deep gratitude and attribution to **Nordl3** for the original project concept and pioneering the algorithmic foundations of automated dual-phrase musical dialogue.
- **SideBrain**: Sincere appreciation and attribution to **SideBrain** for his renowned Max for Live implementation, creative workflow concepts, and dedication to sharing innovative composition tools with the music production community.

### Evolution by Viral Samples
Viral Samples evolved and expanded these foundational ideas into a native, high-performance C++/JUCE instrument for macOS:
- Full **Audio Unit (AU)**, **VST3**, and **Standalone** universal 64-bit binaries.
- Integrated **analog-modelled 303 synthesizer** with squelchy resonant low-pass filter and envelope decay for instant previewing.
- **Interactive 2D Piano Roll** with full pitch transpose, step movement, gate handles, and double-click note editing.
- **Dynamic Scale Transposition** that automatically conforms existing phrases when modulating keys or scales while preserving modal intervals.
- **8 Rapid Preset Slots (A–H)** with instant **Cmd+Click** saving and **Double-Click** loading.
- **Multi-Level Undo/Redo** history (24 states) and global **MIDI Panic** control.
- **Vector Lock Icons** to freeze chosen parameters during Dice randomization.
- **3-Way Split Views** (`BOTH`, `CALL`, `RESP`) offering seamless dual-lane and focused editing.
- **9 Surgical Mutation Engines** and **Drag & Drop MIDI Export** with 4 routing configurations.

---

## Feature Summary

| Category | Highlights |
|---|---|
| **Musical Engine** | 35 Scales (Diatonic, Pentatonic, Blues, Exotic, Arp Pools), 40 Musical Styles, 38 Response Strategies, 24 Opening Motif types. |
| **Transposition** | Real-time automatic note conforming on Key/Scale/Octave change. Preserves minor/major 3rds, 6ths, 7ths, and functional intervals. |
| **Piano Roll** | 2D drag & drop note positioning, double-click add/delete, note edge duration resizing, audition keys, accent & slide toggling. |
| **Step Grid** | Independent 16-step Call & Response lanes, shift left/right, octave transpose, inversion, reversal, accent & slide editing. |
| **Split Views** | Instant switching between **BOTH** (stacked step grids + piano roll), **CALL** (call only), and **RESP** (response only). |
| **303 Synth** | Built-in analog-modelled sawtooth/square oscillator, resonant 24dB low-pass filter, and snappy decay envelope. |
| **Performance** | Dynamic Density thinning/filling, global Gate articulation (10%–250%), Octave Jump leaps, and Cadence Resolve steering. |
| **Presets & History**| 8 quick-recall slots (A–H) with save/load indicators, 24-step undo/redo queue, panic button. |
| **Parameter Locks**| Vector padlock buttons for Style, Strategy, Key/Scale, and Motif to protect settings during Dice re-rolls. |
| **Mutation Suite** | 9 targeted mutation modes (Rhythm, Pitch, Articulation, Ending, Motif, Whole, Response, Call, Linked) across 5 intensity levels. |
| **MIDI Export** | Drag & Drop directly into DAW arrangements with Combined, Call Only, Response Only, or Layered multi-channel routing. |
| **Compatibility** | Universal 2 Binary (`arm64` Apple Silicon M1–M4 and `x86_64` Intel Macs). Pre-codesigned and dequarantined. |

---

## Installation & Setup

### macOS Installation (Recommended)
Double-click **`Call & Response MIDI v1.0.5 Installer.pkg`** to install:
- **VST3 Plugin**: `/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3`
- **Audio Unit (AU)**: `/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component`
- **User Documentation**: `/Library/Audio/Documentation/Viral Samples/Call & Response MIDI/`

The installer automatically de-quarantines the files, applies code signatures, sets POSIX permissions, and flushes `AudioComponentRegistrar` so your DAWs (Logic Pro, Ableton Live, FL Studio, Studio One, Cubase, Reaper) recognize the plugin immediately.

### Windows Installation
Run **`Call & Response MIDI v1.0.5 Windows Setup.exe`** (built via Inno Setup, `build_windows.bat`, or GitHub Actions):
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
| **Add / Delete Note in Piano Roll** | `Double-Click` on grid cell or existing note |
| **Move Note (Pitch & Step)** | Click note center and drag horizontally or vertically |
| **Resize Note Gate Duration** | Hover right edge of note (`↔` cursor) and drag |
| **Toggle Accent / Slide** | Right-Click note for Accent; Alt-Click note for Slide |
| **Parameter Lock / Unlock** | Click padlock icon next to Style, Strategy, Key, or Motif |
| **Audition Pitch** | Click piano keys along the left edge of the Piano Roll |

---

*Call & Response MIDI &bull; Copyright &copy; 2026 Viral Samples. All rights reserved.*
