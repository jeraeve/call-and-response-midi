# Call & Response MIDI — User Manual (v2.0.0)

**Viral Samples | Intelligent Algorithmic Phrase Generator & 303-Style Sequencer**  
*Formats: VST3, Audio Unit (AU), Standalone | Platforms: macOS (Universal: Mojave Intel & Apple Silicon) & Windows (64-bit)*

---

## 1. Overview & Architecture

**Call & Response MIDI v2.0.0** is an advanced dual-lane algorithmic melody generator, performance sequencer, and MIDI composition instrument inspired by classic Roland TB-303 phrasing and the timeless musical dialogue of *Call & Response*.

Instead of disconnected random notes, Call & Response MIDI constructs coherent pairs of musical phrases that converse with one another:
- **Call Lane**: The theme, question, or melodic hook.
- **Response Lane**: The counter-melody, answer, or harmonic resolution shaped by sophisticated modal and rhythmic response strategies.

Version 2.0.0 introduces a completely redesigned, professional dark Ableton-inspired interface with fixed aspect-ratio scaling, interactive step-and-dropdown ribbon controls, continuous velocity and swing dragging, independent lane muting, lane locking, lane mutation percentage controls, rectangular note offset shifters, and an integrated 4-tab Inspector.

A built-in analog-modelled 303 preview synthesizer with resonant 24 dB low-pass filter and snappy envelope decay enables immediate auditioning without needing to route to an external synth.

---

## 2. What's New in Version 2.0.0

- **Sleek Modern UI**: Fully overhauled dark-mode interface with refined typography, high-contrast visual cues, and fixed proportional aspect-ratio scaling ($1180 \times 750$).
- **Top Ribbon Controls**:
  - Direct value displays for `OCT`, `KEY`, `SCALE`, `b/#`, `STEPS`, `RATE`, and `SWING`.
  - Double interaction paradigm: click to open dropdown selection or click the `▲` / `▼` arrow steppers.
  - **Tempo-Relative RATE**: Switch between `Half speed`, `0` (Normal tempo sync), and `Double speed`.
  - **Continuous SWING Dragging**: Click and drag vertically to smoothly adjust groove swing from `0%` to `100%`.
  - **8 Rapid-Access Preset Slots (A–H)**: Instant `Cmd+Click` saving, `Double-Click` loading, and right-click context menus.
  - **Dedicated Undo & Redo**: Fast step-by-step history undo/redo for parameters and note adjustments.
  - **Global PANIC Button**: Instant MIDI All-Notes-Off reset across all channels.
  - **PREVIEW Audio Toggle**: One-click internal 303 synth enable/disable.
- **Left Control Panel**:
  - High-visibility **NEW PAIR** and **DICE** buttons.
  - Master **DICE** randomizes unlocked Style, Strategy, Motif, Mode, and Ending settings.
  - Rotary knobs for **DENSITY**, **MUTATE**, and **OCT JUMP**.
  - Rectangular **`[ < ] OFFSET [ > ]`** buttons for nudging note patterns left and right.
- **Dual Phrase Lanes (CALL & RESPONSE)**:
  - **Color-Coded Tracks**: Vibrant orange for Call and deep cyan/teal for Response.
  - **MUTE Buttons**: Cleanly bypass sound and playback independently for each lane.
  - **DICE Mutator**: Rerolls content specifically for Call or Response.
  - **Modifiable MUTATE %**: Clickable percentage value to set custom per-lane mutation depth.
  - **Lane Lock (Padlock)**: Protects either lane from being regenerated during global rolls.
  - **Octave Transposition**: Individual `+` / `-` octave buttons per lane.
  - **Quick Tools**: Invert, Reverse, Copy, and dedicated MIDI channel routing displays.
- **4-Tab Inspector Panel**:
  - **GENERATE**: Style (40 styles), Motif (24 types), Strategy (38 types), Mode (Call then response, Overlapped, Call solo), Ending (Resolve, Unresolved, Surprise) with individual vector lock padlocks.
  - **SHAPE**: Gate length ($0.25\times–2.0\times$), continuous click-and-drag **VELOCITY** ($1–127$), Resolve, and Drive.
  - **ARTICULATION**: Accent Boost, Slide Time, Filter Cutoff ($60\,\text{Hz}–14\,\text{kHz}$), Resonance, and Decay Time.
  - **OPTIONS**: Routing subtab (MIDI Channels 1–16, Combined / Split / Layered) and Timing subtab (Sync Mode: Timeline, Restart, Free).
- **Interactive Piano Roll**:
  - Matching lane note colors (Orange for Call, Teal for Response).
  - Repeatable right-edge gate resizing up to $4.0\times$ length.
  - Step auditioning with automatic sound stop on mouse release.
  - 2D pitch transposition, time shifting, and double-click note creation/deletion.

---

## 3. System Requirements & Compatibility

### 3.1 macOS Compatibility
- **Operating System**: macOS 10.14 (Mojave) or later (Catalina, Big Sur, Monterey, Ventura, Sonoma, Sequoia fully verified).
- **CPU Architectures**:
  - **Apple Silicon Native** (`arm64`: M1, M2, M3, M4, Pro, Max, Ultra).
  - **Intel Native** (`x86_64`: Core i5, i7, i9, Xeon).
- **Plugin Formats**: VST3 (64-bit Universal), Audio Unit / AU (64-bit Universal), Standalone Application.
- **AU Validation**: 100% passed Apple `auval` tool.

### 3.2 Windows Compatibility
- **Operating System**: Windows 10 or Windows 11 (64-bit).
- **CPU Architectures**: 64-bit x86-64 (Intel Core / AMD Ryzen).
- **Plugin Formats**: VST3 (64-bit), Standalone Application (`.exe`).
- **Dependencies**: Completely self-contained with static C++ runtime linking (no external MinGW DLLs required).

### 3.3 DAW Compatibility
- **Ableton Live** 10, 11, 12 (macOS & Windows)
- **Logic Pro X** & GarageBand (macOS Audio Unit)
- **FL Studio** 20, 21, 24 (macOS & Windows)
- **Cubase** 11, 12, 13, 14 & Nuendo (macOS & Windows)
- **Studio One** 5, 6, 7 (macOS & Windows)
- **Reaper** 6, 7 (macOS & Windows)
- **Bitwig Studio** 4, 5 (macOS & Windows)

---

## 4. Interface Tour & Controls

### 4.1 Top Ribbon Bar

```
[ CALL & RESPONSE v.2 ]  OCT: 3  KEY: C  SCALE: Minor  b/#  STEPS: 16  RATE: 0  SWING: 0%   [A][B][C][D][E][F][G][H]   UNDO REDO   PANIC   PREVIEW: ON
```

1. **Plugin Title**: Displays `CALL & RESPONSE v.2` and developer attribution `Viral Samples`.
2. **OCT (Octave)**: Baseline pitch octave (`C1`–`C5`). Change by clicking the box for a dropdown or using `▲`/`▼`.
3. **KEY**: Musical root note (`C` through `B`). Supports flats and sharps via the `b/#` toggle button.
4. **SCALE**: 35 curated musical scales. When changed, existing notes intelligently conform to the new scale while preserving interval relationships.
5. **STEPS**: Phrase loop length: `4`, `8`, `12`, or `16` steps.
6. **RATE**: Tempo-relative playback speed:
   - `Half speed`: Runs at half the host DAW tempo ($0.5\times$).
   - `0`: Standard $1\times$ tempo sync ($1/16\text{th}$ notes at host tempo).
   - `Double speed`: Runs at double the host DAW tempo ($2.0\times$).
7. **SWING**: Groove swing ($0\%–100\%$). Click and drag vertically for continuous adjustments, or click the stepper buttons.
8. **Preset Slots [A]–[H]**:
   - `Cmd+Click` (or `Ctrl+Click` / `Shift+Click` / `Alt+Click`): Save current pattern and settings to the slot.
   - `Double-Click`: Load pattern and settings from the slot.
   - `Right-Click`: Open context menu to Save or Load.
   - Active slots display a bright cyan indicator badge.
9. **UNDO / REDO**: Instantly revert or restore note edits, generator rolls, and parameter changes.
10. **PANIC**: Sends All-Notes-Off and All-Sound-Off across all 16 MIDI channels.
11. **PREVIEW: ON / OFF**: Enables or mutes the internal 303 synthesizer engine.

---

### 4.2 Left Control Section

1. **NEW PAIR**: Generates a brand-new correlated Call and Response melody using the active settings.
2. **DICE**: Rerolls unlocked parameters (Style, Strategy, Motif, Mode, Ending) and regenerates the phrases.
3. **Performance Knobs**:
   - **DENSITY**: Adjusts the rhythmic population of notes without disrupting key downbeats.
   - **MUTATE**: Governs mutation intensity applied during generative variations.
   - **OCT JUMP**: Injects classic 303-style octave leaps ($\pm 12$, $+24$ semitones).
4. **`[ < ] OFFSET [ > ]`**: Rectangular buttons that shift all notes in the active phrases left or right by one step.

---

### 4.3 Dual Phrase Lanes (CALL & RESPONSE)

Each lane controls its phrase independently:
- **MUTE**: Silences the lane during playback and stops MIDI output for that voice.
- **DICE**: Generates a musical variation specifically for that lane.
- **MUTATE xx%**: Modifiable mutation percentage depth for surgical phrase variations.
- **LOCK (Padlock)**: Protects the lane from being altered when pressing NEW PAIR or master DICE.
- **OCT (+/-)**: Transposes the lane up or down by 12 semitones.
- **INV**: Inverts pitch contours across the active scale.
- **REV**: Reverses the phrase rhythmically from back to front.
- **COPY**: Clones the current lane's phrase into the opposite lane.
- **CH (Channel)**: Displays and routes the lane's MIDI output channel (e.g. `CH: 1` or `CH: 2`).

---

### 4.4 Four-Tab Inspector Panel

#### Tab 1: GENERATE
- **Style (40 Musical Styles)**: Governs syncopation, rhythmic density, and slide/accent character (e.g., *Classic Acid, Raw Acid, Squelchy Acid, Dark Rave, Driving Techno, Minimal Techno, Jackin House, Deep House*).
- **Motif (24 Hook Types)**: Determines the lead-in pattern (*Pulse, Offbeat, Syncopated, Rolling, Acid nibble, Arp fragment*).
- **Strategy (38 Response Types)**: Controls how the Response answers the Call (*Same rhythm + resolve, Invert contour, Answer downward, Mirror contour, Echo, Response resolves to root*).
- **Mode**: Call then response, Overlapped, or Call solo.
- **Ending**: Resolve to tonic, Unresolved tension, or Surprise cadence.
- **Vector Lock Padlocks**: Click any padlock icon to freeze that parameter during master DICE re-rolls.

#### Tab 2: SHAPE
- **Gate Length**: Global note length multiplier ($0.25\times$ staccato to $2.0\times$ legato).
- **Velocity**: Click and drag vertically to smoothly set note velocity between `1` and `127`.
- **Resolve**: Strength of harmonic pull toward tonic root notes on cadence points.
- **Drive**: Saturation drive applied to the internal 303 synth filter stage.

#### Tab 3: ARTICULATION
- **Accent Boost**: Volume and filter envelope boost applied to accented steps.
- **Slide Time**: Portamento glide time between tied or slurred notes.
- **Filter Cutoff**: 303 low-pass filter frequency ($60\,\text{Hz}$ to $14\,\text{kHz}$).
- **Resonance**: Peak resonance emphasizing classic squelchy acid harmonics.
- **Decay Time**: Envelope decay duration ($0.05\,\text{s}$ to $1.5\,\text{s}$).

#### Tab 4: OPTIONS
- **Routing Subtab**:
  - *Combined*: Both Call and Response output on MIDI Channel 1.
  - *Split Channels*: Call routes to Channel 1, Response routes to Channel 2 for multi-timbral setups.
  - *Separate*: Independent channel assignments for Call and Response.
  - *Layered*: Both phrases broadcast across multiple selected channels.
- **Timing & Clips Subtab**:
  - *Sync Mode*: Timeline (synced to DAW transport bar position), Restart (restarts on DAW transport play), Free (runs continuously).
  - *Clip Quantization*: Set re-roll and regeneration quantization to 1 Bar, 1/2 Bar, 1/4 Bar, or Immediate.

---

### 4.5 Interactive Piano Roll

- **Visual Clarity**: Call notes appear in vibrant orange; Response notes appear in bright cyan/teal.
- **Add / Delete Notes**: Double-click any empty grid cell to add a note; double-click an existing note to delete it.
- **Move Notes**: Click and drag any note horizontally to shift steps or vertically to transpose pitch.
- **Resize Gate Duration**: Hover over the right edge of any note (the cursor transforms to `↔`) and drag horizontally. Notes can be lengthened or shortened repeatedly up to $4.0\times$.
- **Audition Steps**: Click on any note to audition its sound through the internal synth. Auditioning automatically ceases immediately upon mouse release.
- **Piano Keyboard**: Click the piano keys on the left margin to audition individual pitches.
- **Accent & Slide**: Right-click a note to toggle Accent (marked with `▲`); Alt-click a note to toggle Slide (marked with `~`).

---

## 5. MIDI Drag & Drop Export

1. Locate the **DRAG MIDI** button at the bottom of the interface.
2. Click and hold the **DRAG MIDI** icon.
3. Drag directly onto any MIDI track in your DAW arrangement (Ableton Live, FL Studio, Logic Pro, Studio One, etc.).
4. The generated MIDI file includes:
   - Fully resolved note pitches matching your key and scale.
   - Precise note start times, swing offsets, and gate durations.
   - Accurate note velocities with accent boosts.
   - Separate MIDI channels if Split Routing is enabled.

---

## 6. Preset Management & Storage

Presets are stored in standard cross-platform JSON format:
- **macOS**: `~/Library/Application Support/Viral Samples/Call & Response MIDI/Presets/`
- **Windows**: `%APPDATA%\Viral Samples\Call & Response MIDI\Presets\`

Each preset saves:
- All 16 steps of both Call and Response phrases (pitch, length, velocity, accent, slide).
- Key, Scale, Root Octave, Steps, Rate, and Swing values.
- Selected Style, Motif, Strategy, Mode, and Ending.
- All Shape, Articulation, and Routing options.
- State of all parameter locks and lane mute/lock statuses.

---

## 7. Troubleshooting & FAQ

**Q: The plugin does not appear in my DAW on macOS.**  
A: Ensure you ran the official `.pkg` installer. On macOS, run `killall -9 AudioComponentRegistrar` in Terminal or restart your Mac to refresh the AU plugin cache.

**Q: How do I mute the internal synth and use external instruments?**  
A: Click **PREVIEW: ON** in the top ribbon so it toggles to **PREVIEW: MUTED**. Route the MIDI output of the Call & Response track to your desired software synth track in your DAW.

**Q: My DAW's tempo changes, how does RATE respond?**  
A: Call & Response MIDI automatically locks to host tempo. When `RATE` is set to `0`, phrases run in standard $1/16\text{th}$ note sync. Set to `Half speed` for half-time grooves or `Double speed` for drum & bass / double-time arpeggios.

**Q: How do I prevent my custom Call phrase from changing when clicking DICE?**  
A: Click the **LOCK** padlock on the CALL lane header. The Call melody will remain completely intact while the master DICE or lane DICE rolls the Response.

---

*Call & Response MIDI v2.0.0 &bull; Copyright &copy; 2026 Viral Samples. All rights reserved.*
