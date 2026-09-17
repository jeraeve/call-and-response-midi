# Call & Response MIDI — User Manual (v1.0.5)

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**  
*Formats: VST3, Audio Unit (AU), Standalone | Platforms: macOS (Apple Silicon & Intel) & Windows (64-bit)*

---

## 1. Overview & Architecture

**Call & Response MIDI** is a dual-lane algorithmic melody generator, performance sequencer, and MIDI composition instrument inspired by classic Roland TB-303 phrasing and the universal musical conversation of *Call & Response*.

Instead of random notes, Call & Response MIDI generates pairs of musical phrases that dialogue with one another:
- **Call Lane**: The question, theme, or lead hook.
- **Response Lane**: The answer, variation, or harmonic resolution shaped by sophisticated modal and rhythmic response strategies.

A built-in analog-modelled 303 preview synth with resonant low-pass filter allows instant standalone auditioning without having to wire a virtual instrument first.

---

## 2. System Requirements & Compatibility

### 2.1 macOS
- **Operating System**: macOS 11.0 (Big Sur) or higher (Monterey, Ventura, Sonoma, Sequoia fully supported).
- **CPU Architectures**:
  - **Apple Silicon** (`arm64`: M1, M2, M3, M4, Pro, Max, Ultra native).
  - **Intel Macs** (`x86_64`: Core i5, i7, i9, Xeon native).
- **Plugin Formats**: VST3 (64-bit universal), Audio Unit / AU (64-bit universal), Standalone Application.

### 2.2 Windows
- **Operating System**: Windows 10 or Windows 11 (64-bit).
- **CPU Architectures**: 64-bit x86-64 (Intel Core / AMD Ryzen).
- **Plugin Formats**: VST3 (64-bit), Standalone Application (`.exe`).

### 2.3 DAW Compatibility
Ableton Live 10/11/12 (macOS & Windows), Logic Pro X (macOS), FL Studio 20/21/24 (macOS & Windows), Cubase, Studio One, Reaper, Bitwig Studio, Pro Tools (via AU/VST3 wrapper).

---

## 3. Header & Utility Controls

```
[ CALL & RESPONSE ]  VIRAL SAMPLES | v1.0.5   [STEPS] [PIANO ROLL] [SPLIT]   [A] [B] [C] [D] [E] [F] [G] [H]   [UNDO] [REDO]   [PANIC]   [PREVIEW: ON]
```

### 3.1 Preset System (Slots A–H)
The plugin includes 8 rapid-access preset memory slots located in the top header:
- **Save Current State**: **Cmd+Click** (or **Shift+Click**, **Alt+Click**, **Ctrl+Click**) on any slot button (`A` through `H`). The status bar confirms: `"Saved current state to Preset Slot X"`.
- **Load Preset**: **Double-Click** on any slot button to immediately recall its pattern, scale, key, and settings.
- **Context Menu**: **Right-Click** on any slot button to access the popup menu:
  - *Save Current State to Slot X*
  - *Load Preset Slot X*
- **Visual Status**: Occupied slots feature a bright cyan indicator dot and highlighted border. Single-clicking inspects slot occupancy without overwriting unsaved work.

### 3.2 Undo & Redo History
- **Undo / Redo Buttons**: Store up to **24 states** of musical edits, generations, transpositions, and mutations. If an edit or random roll doesn't fit your arrangement, click **Undo** to instantly restore your previous pattern.

### 3.3 Audio Preview & Panic
- **PREVIEW: ON / MUTED**: Toggles the internal 303 synthesizer engine. Mute this button when routing MIDI output directly to external hardware or software synthesizers.
- **PANIC**: Immediately issues *All-Notes-Off* and *All-Sound-Off* commands across all 16 MIDI channels to eliminate hanging notes.

---

## 4. Generative Engine & Controls

### 4.1 Musical Controls
- **Key**: Sets the harmonic root (`C` through `B`).
- **Scale**: Choose from **35 curated musical scales**:
  - *Diatonic & Modes*: Major, Minor, Dorian, Phrygian, Lydian, Mixolydian, Locrian, Lydian Dominant, Aeolian b5, Dorian b2, Mixolydian b6.
  - *Pentatonic & Blues*: Minor Pentatonic, Major Pentatonic, Blues Minor, Blues Major, Egyptian Pentatonic.
  - *Exotic & Harmonic*: Phrygian Dominant, Double Harmonic, Hungarian Minor, Romanian Minor, Whole Tone, Diminished, Byzantine, Persian, Arabic, Hirajoshi, In Sen, Kumoi, Pelog.
  - *Minimalist & Arp Pools*: Power Fifths, Root + b7, Minor 7 Arp, Dominant 7 Arp, Diminished 7 Arp.
- **Root Octave**: Sets baseline octave (`C1` through `C5`).
- **Steps**: Select loop lengths of **4, 8, 12, or 16 steps**.

### 4.2 Style & Response Strategy
- **Style (40 Styles)**: Governs rhythmic bias, syncopation, rest likelihood, and slide/accent characteristics (e.g., *Classic Acid, Raw Acid, Squelchy Acid, Dark Rave, Driving Techno, Minimal Techno, Jackin House, Deep House, Hooverish Riff, Breaky Electro*).
- **Response Strategy (38 Types)**: Dictates how the response answers the call:
  - *Same rhythm + resolve*, *Invert contour*, *Answer downward*, *Mirror contour*, *Echo*, *Response resolves to root*, *Tighter answer*, *Sparser answer*, *Transpose motif up/down*.
- **Motif Type (24 Types)**: Shapes the opening lead-in hook (*Pulse, Offbeat, Syncopated, Push, Rolling, Acid nibble, Arp fragment, Techno cell, House bounce*).
- **Ending Behaviour & Phrase Mode**: Configures cadence resolution and relationship between Call and Response phases (*Call then response, Call response overlapped, Call solo*).

### 4.3 Parameter Lock Icons
Adjacent to **Style**, **Strategy**, **Key/Scale**, and **Motif** are vector padlock icon buttons:
- **Unlocked (Open Padlock)**: Parameters can be randomized when clicking **DICE**.
- **Locked (Closed Padlock, Glowing Cyan)**: Freezes the parameter, protecting your key, scale, or favorite style from being modified when re-rolling patterns.

### 4.4 Generation Actions
- **NEW PAIR**: Generates a completely new correlated Call and Response phrase using current settings.
- **DICE**: Rerolls only the unlocked parameters and generates fresh phrases.
- **NOW vs NEXT PHRASE**: Sets regeneration timing. In *NEXT PHRASE* mode, pressing generate queues the transition seamlessly until the next 16-step bar boundary (*"QUEUED"*).

---

## 5. Performance Knobs & 303 Synth

### 5.1 Performance Rotaries
- **DENSITY**: Dynamically thins or populates note events without altering the core downbeats.
- **GATE**: Adjusts global articulation from snappy staccato ticks (10%) to extended sustained ties (250%).
- **OCT JUMP**: Introduces dynamic $\pm 12$ and $+24$ semitone octave leaps inspired by vintage 303 acid lines.
- **RESOLVE**: Directs phrase-ending notes toward harmonic tonic resolution or unresolved tension.

### 5.2 303 Preview Synthesizer
- **CUTOFF**: Adjusts low-pass filter cutoff frequency.
- **RESONANCE**: Amplifies resonant frequency peak for classic squelch.
- **DECAY**: Shapes the envelope decay release time.

---

## 6. Sequencer, Piano Roll & Split Views

Use the top view switcher to choose your preferred editing workspace:

### 6.1 Step Grid View (`[STEPS]`)
Features independent lanes for **CALL** and **RESPONSE**:
- **MIDI Channel Routing**: Assign each lane to distinct MIDI output channels (Channel 1–16) for driving multitimbral plugins or hardware.
- **Lane Actions**:
  - `MUTE`: Silences that lane independently.
  - `<` / `>`: Shifts the pattern horizontally left or right.
  - `-8ve` / `+8ve`: Transposes the entire lane up or down one octave.
  - `Inv`: Inverts scale degrees around the median pitch.
  - `Rev`: Reverses the sequence in time.
- **Step Editing**:
  - **Double-Click**: Adds or deletes a note at that step.
  - **Drag Horizontally**: Moves and swaps notes between steps left and right.
  - **Drag Vertically**: Changes scale degree / pitch.
  - **Right-Click**: Toggles **ACC** (Accent).
  - **Alt-Click**: Toggles **SLD** (Slide / Portamento).

### 6.2 Piano Roll View (`[PIANO ROLL]`)
Full interactive melody editor displaying pitch against time:
- **2D Note Manipulation**:
  - Click and drag a note **Up/Down** to transpose pitch with real-time audio auditioning.
  - Drag **Left/Right** to move notes across steps without scrambling neighboring columns.
  - Hovering note center displays 4-way move cursor (`✛`).
- **Double-Click**:
  - Double-clicking an empty cell adds a note conforming to the current scale.
  - Double-clicking an existing note deletes it.
- **Gate Resizing**: Hover over the right edge of any note (cursor changes to `↔`) and drag horizontally to customize gate length.
- **Pitch Audition**: Click any piano key on the left keyboard to preview notes.
- **Articulations**:
  - Right-click note to toggle Accent (orange badge).
  - Alt-click note to toggle Slide (cyan slide line).
  - Shift-click note to delete.

### 6.3 Split View (`[SPLIT]`)
Simultaneously displays Step Grids on top with the Piano Roll below:
- **`BOTH` (Default)**: Stacked Call and Response step grids above the combined Piano Roll.
- **`CALL`**: Call Step Grid on top with Call Piano Roll below.
- **`RESP`**: Response Step Grid on top with Response Piano Roll below.

---

## 7. Dynamic Scale & Key Transposition

Whenever you change the **Key**, **Scale**, or **Root Octave** dropdowns:
- Existing notes in both Call and Response are **automatically transposed** to conform to the new scale.
- **Musical Function Preservation**: Preserves modal relationships (minor 3rd $\leftrightarrow$ major 3rd, minor 6th $\leftrightarrow$ major 6th, minor 7th $\leftrightarrow$ major 7th, minor 2nd $\leftrightarrow$ major 2nd, tritone $\leftrightarrow$ perfect 5th).
- Transpositions reflect immediately in the **Piano Roll**, the **Step Grid**, the **Preview Synth**, and **Exported MIDI**.

---

## 8. Mutation Engine & MIDI Export

### 8.1 Mutation Engine
Select from **9 surgical mutation targets**:
1. **Rhythm only**: Syncopates and shifts step timing while preserving pitch.
2. **Pitch only**: Modifies scale degrees while preserving rhythm.
3. **Articulation only**: Re-rolls gate lengths, accents, and slides.
4. **Ending only**: Rewrites the final cadence steps (steps 13–16).
5. **Motif only**: Reinvents the opening 2–4 note theme.
6. **Whole phrase**: Comprehensive variation of rhythm, pitch, and feel.
7. **Response only**: Mutates exclusively the response lane.
8. **Call only**: Mutates exclusively the call lane.
9. **Call + response linked**: Mutates call and procedurally re-derives response.

*Amount options*: **Tiny (1 step)**, **Small (2 steps)**, **Medium (3 steps)**, **Strong (5 steps)**, **Brutal (8 steps)**.

### 8.2 MIDI Drag & Drop
Click and drag the **DRAG MIDI** button directly into your DAW arrangement or desktop:
- **Export Modes**:
  - `Combined`: Exports Call followed sequentially by Response in a single MIDI clip.
  - `Call Only`: Exports only the Call pattern.
  - `Response Only`: Exports only the Response pattern.
  - `Layered`: Exports Call on Channel 1 and Response on Channel 2 simultaneously.
- **Whole Bars**: Pads the export to full musical bar lengths for seamless DAW looping.

---

## 9. File Locations & Installation Directory

### 9.1 macOS
- **VST3 Plugins**:
  - System: `/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3`
  - User: `~/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3`
- **Audio Unit (AU) Plugins**:
  - System: `/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component`
  - User: `~/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component`
- **Standalone Application**:
  - `/Applications/Call & Response MIDI.app`
- **Documentation**:
  - `/Library/Audio/Documentation/Viral Samples/Call & Response MIDI/`
- **Installer**:
  - `Call & Response MIDI v1.0.5 Installer.pkg`

### 9.2 Windows
- **VST3 Plugins**:
  - `C:\Program Files\Common Files\VST3\Call & Response MIDI.vst3`
- **Standalone Application**:
  - `C:\Program Files\Viral Samples\Call & Response MIDI\Call & Response MIDI.exe`
- **Documentation**:
  - `C:\Program Files\Viral Samples\Call & Response MIDI\Documentation\`
- **Installer**:
  - `Call & Response MIDI v1.0.5 Windows Setup.exe` (built via Inno Setup script `installer/windows/installer.iss` or `build_windows.bat`)

---

*Copyright © 2026 Viral Samples. All rights reserved.*
