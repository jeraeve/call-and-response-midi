# Call & Response MIDI v2.0.0

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**

Generative dual-phrase musical dialogue synthesizer, performance sequencer, and MIDI composition instrument for macOS and Windows.

---

## 📦 Downloads & Installers

| Operating System | Installer Package | Formats Included | Architecture |
|---|---|---|---|
| **macOS (10.14+)** | **`Call & Response MIDI v2.0.0 Installer.pkg`** | VST3, Audio Unit (AU), Docs | Universal (`arm64` Apple Silicon M1–M4 & `x86_64` Intel) |
| **Windows (10/11)** | **`Call & Response MIDI v2.0.0 Windows Setup.exe`** | 64-bit VST3, Standalone App (`.exe`), Docs | 64-bit x86-64 |
| **User Manual** | **`User Manual.pdf`** | Comprehensive 6-page guide | All platforms |
| **Read Me** | **`Read Me.pdf` / `Read Me.txt`** | Attributions, features & quick keys | All platforms |

> **macOS Compatibility Note:** The universal installer supports **macOS Mojave 10.14+** (Catalina, Big Sur, Monterey, Ventura, Sonoma, Sequoia) across both **Apple Silicon** and **Intel Macs**. The installer automatically de-quarantines binaries, applies ad-hoc codesigning, sets POSIX permissions, and refreshes the macOS `AudioComponentRegistrar` so your DAWs (Logic Pro, Ableton Live, FL Studio, Studio One, Cubase, Reaper) recognize the Audio Unit and VST3 immediately. Passes 100% of Apple's `auval` validation.
>
> **Windows Compatibility Note:** The 64-bit Windows setup wizard installs the VST3 plugin directly into standard `%COMMONPROGRAMFILES%\VST3\`, installs the standalone application with optional desktop shortcuts, and includes all documentation. Completely portable with static C++ runtime linking.

---

## 🌟 What's New in Version 2.0.0

### 1. Modernized Dark Interface & Proportional Resizing
- Completely redesigned aesthetic inspired by modern DAW workflows with high-contrast, clean visual hierarchy.
- **Fixed Aspect-Ratio Scaling**: UI maintains balanced proportions (1180 × 750) when resized without stretching or squishing elements.

### 2. Interactive Top Ribbon Bar
- **Direct Value Displays**: Dedicated parameter boxes for `OCT`, `KEY`, `SCALE`, `b/#`, `STEPS`, `RATE`, and `SWING`.
- **Double Interaction**: Click any value box to open a dropdown menu, or click the `▲` / `▼` arrow steppers to cycle values.
- **Tempo-Relative RATE**: Switch playback speed relative to host tempo between `Half speed` (0.5×), `0` (Normal 1× 16th-note sync), and `Double speed` (2.0×).
- **Continuous SWING Dragging**: Click and drag vertically to adjust swing smoothly from `0%` to `100%`.
- **8 Rapid-Access Preset Slots (A–H)**: `Cmd+Click` to save, `Double-Click` to load, right-click context menu, and active slot indicator badges.
- **Dedicated Undo / Redo**: Non-destructive history queue to step back through generative rolls, parameter tweaks, and note edits.
- **Global PANIC Button**: Immediate All-Notes-Off and All-Sound-Off reset across all 16 MIDI channels.
- **PREVIEW Audio Toggle**: One-click enable/disable for the internal 303 preview synth.

### 3. Left Control Section
- High-visibility **NEW PAIR** and **DICE** buttons.
- **Master DICE** randomizes unlocked Style, Strategy, Motif, Mode, and Ending parameters.
- Performance rotaries for **DENSITY**, **MUTATE**, and **OCT JUMP**.
- Rectangular **`[ < ] OFFSET [ > ]`** buttons to nudge notes left or right by one step.

### 4. Dual Phrase Lanes (CALL & RESPONSE)
- **Vibrant Track Themes**: High-visibility orange for Call and deep cyan/teal for Response.
- **Individual MUTE Buttons**: Bypasses sound and playback independently for each lane.
- **Dedicated DICE Mutators**: Rolls variations specifically for Call or Response.
- **Modifiable MUTATE % Depth**: Clickable percentage control to dial in exact mutation intensity.
- **Lane Lock (Padlock)**: Freezes either lane to protect custom melodies from global rolls.
- **Lane Transformation Tools**: Octave shift (`+`/`-`), Invert, Reverse, Copy, and MIDI channel routing displays.

### 5. Four-Tab Inspector Panel
- **GENERATE**: Style (40 musical styles), Motif (24 lead-in hooks), Strategy (38 response variations), Mode, and Ending with individual vector lock padlocks.
- **SHAPE**: Gate Length (0.25× – 2.0×), continuous click-and-drag **VELOCITY** (1 – 127), Resolve, and Drive.
- **ARTICULATION**: Accent Boost, Slide Time, Filter Cutoff (60 Hz – 14 kHz), Resonance, and Decay Time.
- **OPTIONS**: Routing subtab (MIDI Channels 1–16, Combined / Split / Layered) and Timing subtab (Sync Mode: Timeline, Restart, Free).

### 6. Interactive 2D Piano Roll
- Note blocks reflect lane colors (Orange for Call, Teal for Response).
- **Repeatable Gate Resizing**: Hover the right edge of any note (`↔` cursor) and drag horizontally up to 4.0× length.
- **Step Audition Auto-Stop**: Auditioning automatically ceases immediately upon mouse release without hanging notes.
- 2D pitch transposition, step movement, double-click add/delete, and left-side piano keyboard preview.

---

## 🙏 Attributions & Acknowledgments

- **Nordl3**: Deep gratitude and attribution to Nordl3 for conceptualizing and developing the original foundational algorithmic Call and Response music generation system.
- **SideBrain**: Sincere appreciation and attribution to SideBrain for his renowned Max for Live implementation, inspiring workflow concepts, and dedication to sharing innovative composition tools with the community.
