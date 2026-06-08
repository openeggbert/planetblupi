# Planet Blupi — Comprehensive Technical Documentation

## Table of Contents

1. [What is Planet Blupi?](#what-is-planet-blupi)
2. [Historical Context & Relation to Speedy Blupi](#historical-context--relation-to-speedy-blupi)
3. [Architecture Overview](#architecture-overview)
4. [Source Files Reference](#source-files-reference)
5. [Key Data Structures](#key-data-structures)
6. [Game Phases (Screens)](#game-phases-screens)
7. [World File Format (.blp)](#world-file-format-blp)
8. [Image Assets (`image/` directory)](#image-assets-image-directory)
9. [Sound Assets (`sound/` directory)](#sound-assets-sound-directory)
10. [Sound Effect Index Reference (SOUND_*)](#sound-effect-index-reference-sound_)
11. [Music Tracks](#music-tracks)
12. [Data Assets (`data/` directory)](#data-assets-data-directory)
13. [Movie Assets (`movie/` directory)](#movie-assets-movie-directory)
14. [Characters & Entities](#characters--entities)
15. [Character Actions (ACTION_*)](#character-actions-action_)
16. [Goals & Task Interpreter (GOAL_*)](#goals--task-interpreter-goal_)
17. [World Grid & Terrain](#world-grid--terrain)
18. [Floor Tile Channels](#floor-tile-channels)
19. [Object Channels](#object-channels)
20. [Toolbar Buttons (BUTTON_*)](#toolbar-buttons-button_)
21. [Win Conditions (Term)](#win-conditions-term)
22. [Movement Directions (DIRECT_*)](#movement-directions-direct_)
23. [Pathfinding (A* / CPileTriee)](#pathfinding-a--cpileetriee)
24. [Fog of War](#fog-of-war)
25. [Minimap](#minimap)
26. [Statistics Panel](#statistics-panel)
27. [Demo Recording & Playback](#demo-recording--playback)
28. [Localisation](#localisation)
29. [Build System & Platforms](#build-system--platforms)
30. [Configuration File](#configuration-file)
31. [Web (Emscripten) Persistence](#web-emscripten-persistence)
32. [Cheat Codes](#cheat-codes)
33. [Skill Levels](#skill-levels)
34. [Regions (World Themes)](#regions-world-themes)
35. [Undo System](#undo-system)
36. [Mouse Cursor Sprites (SPRITE_*)](#mouse-cursor-sprites-sprite_)
37. [Error Codes (ERROR_*)](#error-codes-error_)

---

## What is Planet Blupi?

**Planet Blupi** is a real-time strategy / puzzle-adventure game developed by
**Epsitec SA** (Switzerland), originally released in **1997** for Windows.
The game was made open-source by Epsitec.

The player controls one or more **Blupi** — small yellow creatures — on a
200×200 isometric grid.  The goal is to build infrastructure, gather
resources, and satisfy mission-specific win conditions while fending off
enemies (spiders, viruses, robots, tracks/tanks, electro towers).

### Gameplay Pillars

| Pillar | Description |
|--------|-------------|
| **Build** | Construct huts, laboratories, factories, fences, towers, boats, jeeps |
| **Gather** | Chop wood, mine stone / iron, pick flowers, extract iron ore |
| **Defend** | Fight off spiders, viruses, robots, tanks using dynamite, mines, armour |
| **Complete missions** | Each world file has a `Term` struct specifying win conditions |

### Technical Summary

- **Engine**: C++ / Win32 / DirectDraw / DirectSound / MCI (AVI)
- **Compatibility layer**: [Free Direct](https://github.com/openeggbert/free-direct) (DirectDraw/DirectSound → SDL3), [Free API](https://github.com/openeggbert/free-api) (Win32 → cross-platform)
- **Platforms**: Windows (original), Linux (ported), Web/Emscripten (ported), Android (experimental)
- **Resolution**: fixed 640×480 pixels
- **Colour depth**: 8-bit palette (256 colours) on original hardware, 32-bit via SDL3 layer

---

## Historical Context & Relation to Speedy Blupi

Planet Blupi and **Speedy Blupi** share the same Epsitec "Blupi" universe:

| Property | Speedy Blupi | Planet Blupi |
|----------|-------------|--------------|
| Genre | Side-scrolling platformer | Isometric RTS / puzzle |
| Release | ~1995 | 1997 |
| Engine style | Tile automaton (actions.h) | Goal-based task interpreter (decgoal.h) |
| Characters | "Toto" (Speedy Blupi) | "Blupi" (Planet Blupi) |

The `include/actions.h` file in this repository is a **direct remnant of the
Speedy Blupi engine**.  It defines the `Action` enum (directional movement
opcodes for the platformer automaton), the `Objet` enum (animated decoration
states), and a `Sound` enum with SFX indices 1–39.  These types are
**not used by Planet Blupi's own engine** — they belong to the Speedy Blupi
animation subsystem that was imported / referenced during development.

> When decompiling or reverse-engineering **Speedy Blupi** using this
> repository as a reference, `actions.h` is the key starting point for
> understanding the character animation state machine.

---

## Architecture Overview

```
blupi.cpp          ← Win32 entry point, main message loop, global state
  │
  ├─ CEvent        ← Phase manager, input router, toolbar, demo rec/play
  │    ├─ CButton[] ← Toolbar buttons (up to 40)
  │    ├─ CJauge[]  ← HUD gauges (energy, progress)
  │    └─ CMenu     ← Context-action popup menu
  │
  ├─ CDecor        ← Game world: grid, characters, animations, I/O
  │    ├─ Cellule[100][100] ← World tile grid
  │    ├─ Blupi[100]        ← Characters (player + enemies)
  │    ├─ Move[100]         ← Animated decoration elements
  │    ├─ arrange.cpp       ← Auto-tiling
  │    ├─ obstacle.cpp      ← Passability / pathfinding helpers
  │    ├─ decblupi.cpp      ← Character lifecycle & goal interpreter
  │    ├─ decmove.cpp       ← Animated decoration steps & fire
  │    ├─ decio.cpp         ← Save/load world files
  │    ├─ decmap.cpp        ← Minimap
  │    ├─ decstat.cpp       ← Statistics panel
  │    ├─ chemin.cpp        ← A* pathfinding (CPileTriee)
  │    └─ decor.cpp         ← Rendering, coordinate transforms
  │
  ├─ CPixmap       ← DirectDraw sprite renderer (image channels)
  ├─ CSound        ← DirectSound audio manager (WAV + MIDI)
  └─ CMovie        ← MCI AVI video player (cinematics)
```

---

## Source Files Reference

| File | Purpose |
|------|---------|
| `src/blupi.cpp` | Win32 `WinMain`, `WindowProc`, global variables, timer |
| `src/action.cpp` | Per-frame animation step function `Action()`, sprite/sound tables |
| `src/arrange.cpp` | Auto-tiling: flood-fill terrain placement, mur/build auto-adjust |
| `src/button.cpp` | `CButton` widget implementation |
| `src/chemin.cpp` | A* pathfinding with `CPileTriee` open-list |
| `src/ddutil.cpp` | DirectDraw bitmap utility functions (MS SDK sample) |
| `src/decblupi.cpp` | Character lifecycle, goal interpreter, selection, highlight |
| `src/decgoal.cpp` | Static goal opcode tables for all 130+ action types |
| `src/decio.cpp` | Binary save/load for `.blp` world files |
| `src/decmap.cpp` | Minimap generation from world cell colours |
| `src/decmove.cpp` | Animated decoration movement & icon tables, fire spread |
| `src/decor.cpp` | `CDecor` core: create, init, render, coordinate conversions |
| `src/decstat.cpp` | Statistics panel (hatch counts, robots, homes) |
| `src/event.cpp` | `CEvent`: phase switching, message routing, demo, build-mode |
| `src/fifo.cpp` | `CPileTriee` priority-stack implementation |
| `src/fog.cpp` | Fog-of-war quadrant bit encoding/decoding |
| `src/jauge.cpp` | `CJauge` gauge widget implementation |
| `src/menu.cpp` | `CMenu` popup-menu widget implementation |
| `src/misc.cpp` | Utility functions (paths, random, error trace) |
| `src/movie.cpp` | `CMovie` MCI-based AVI playback |
| `src/obstacle.cpp` | Passability tables, enemy target-search helpers |
| `src/pixmap.cpp` | `CPixmap` DirectDraw rendering implementation |
| `src/sound.cpp` | `CSound` DirectSound + MIDI implementation |
| `src/tablefloor.cpp` | Default floor layout table for level editor |
| `src/tableobj.cpp` | Default object layout table for level editor |
| `src/text.cpp` | Sprite-based text rendering (fonts, big digits) |
| `src/wave.cpp` | `LoadWave()` — load WAV from Win32 resource |
| `src/web_persistence.cpp` | Emscripten IndexedDB save-game sync |

---

## Key Data Structures

### `Cellule` (one world tile)

```cpp
struct Cellule {
    short floorChannel;   // image channel for the floor sprite
    short floorIcon;      // sprite index within that channel
    short objectChannel;  // image channel for the object sprite
    short objectIcon;     // sprite index within that channel
    short fog;            // fog-of-war icon (0 = fully visible)
    short rankMove;       // index into m_move[] (-1 = none)
    short workBlupi;      // rank of Blupi working here (-1 = none)
    short fire;           // fire intensity counter
};
```

The world is stored as `Cellule m_decor[100][100]` (MAXCELX/2 × MAXCELY/2)
within `CDecor`.

### `Blupi` (one animated character)

Key fields:

| Field | Type | Description |
|-------|------|-------------|
| `bExist` | BOOL | Slot in use |
| `perso` | short | Character type (0=Blupi, 1=Spider, 2=Virus, 3=Tracks, 4=Robot, 5=Bomb, 6=Mine detonator, 7=Electro, 8=Disciple) |
| `cel` | POINT | Current grid cell |
| `destCel` | POINT | Destination cell |
| `action` | short | Current ACTION_* animation |
| `aDirect` | short | Current direction (DIRECT_*) |
| `energy` | short | Energy remaining (0…MAXENERGY=4000) |
| `goalAction` | short | Long-term goal action |
| `vehicule` | short | 0=foot, 1=boat, 2=jeep, 3=armour |
| `listButton[]` | short[10] | Queued action commands |

### `Move` (animated decoration element)

Used for fire spread, bridge animations, building construction steps, etc.
Up to 100 simultaneous animated decorations (`CDecor::m_move[100]`).

### `Term` (win conditions)

```cpp
struct Term {
    short bHachBlupi;    // Blupi must be on hatched cell
    short bHachPlanche;  // planks on hatched cell
    short bStopFire;     // all fires extinguished
    short nbMinBlupi;    // minimum Blupi count
    short nbMaxBlupi;    // maximum Blupi count
    short bHomeBlupi;    // Blupi must be home
    short bKillRobots;   // all enemies destroyed
    short bHachTomate;   // tomatoes on hatched cell
    short bHachMetal;    // metal on hatched cell
    short bHachRobot;    // robot on hatched cell
    short reserve[14];
};
```

---

## Game Phases (Screens)

Each phase corresponds to a `WM_PHASE_*` Windows message.  The active
phase determines which background image, buttons and event handlers are used.

| Constant | Description |
|----------|-------------|
| `WM_PHASE_INIT` | Startup initialisation |
| `WM_PHASE_PLAY` | Main gameplay (play mode) |
| `WM_PHASE_BUILD` | Level-editor / build mode |
| `WM_PHASE_READ` | Load-game screen |
| `WM_PHASE_WRITE` | Save-game screen |
| `WM_PHASE_INFO` | Mission briefing |
| `WM_PHASE_BUTTON` | Action selection via button strip |
| `WM_PHASE_TERM` | Mission-end statistics |
| `WM_PHASE_WIN` | Victory screen |
| `WM_PHASE_LOST` | Defeat screen |
| `WM_PHASE_STOP` | Pause / stop |
| `WM_PHASE_SETUP` | Settings screen |
| `WM_PHASE_MUSIC` | Music selection screen |
| `WM_PHASE_PLAYMOVIE` | Pre-gameplay cinematic |
| `WM_PHASE_WINMOVIE` | Victory cinematic |
| `WM_PHASE_SCHOOL` | Tutorial/school mission selection |
| `WM_PHASE_MISSION` | Main mission selection |
| `WM_PHASE_LASTWIN` | Final victory (game completed) screen |
| `WM_PHASE_REGION` | Region/theme selection |
| `WM_PHASE_INSERT` | CD-ROM insert prompt |
| `WM_PHASE_HISTORY0/1` | Story history screens |
| `WM_PHASE_HELP` | In-game help overlay |
| `WM_PHASE_BYE` | Quit confirmation |
| `WM_PHASE_SKILL1/2` | Skill-level selection |
| `WM_PHASE_DEMO` | Demo playback |
| `WM_PHASE_INTRO1/2` | Introduction animation screens |

---

## World File Format (.blp)

World files use the `.blp` extension (Blupi binary format).  All world
data, character positions and win conditions for one mission are stored in
a single file.

### File Naming Conventions

| Pattern | Purpose |
|---------|---------|
| `world000.blp` … `world005.blp` | Tutorial / school missions (6 levels) |
| `world100.blp` … `world153.blp` | Campaign missions (episodes 1–5, ~50+ levels) |
| `world200.blp` … `world215.blp` | Advanced / bonus missions |
| `user000.blp` … `user009.blp` | Player save slots (in-progress games) |
| `demo000.blp` … `demo003.blp` | Built-in demo recordings |
| `enigmes.blp` | Puzzle / enigma mode world |
| `info.blp` | Game information / credits world |

The file is written/read by `CDecor::Write()` and `CDecor::Read()` in
`src/decio.cpp`.

---

## Image Assets (`image/` directory)

All images are `.blp` files — a custom palette-based bitmap format (BLP =
Blupi image Package).  Each file is loaded into a numbered **image channel**
(see `CHBACK`…`CHBIGNUM` in `def.h`) via `CPixmap::Cache()`.

| File | Channel | Description |
|------|---------|-------------|
| `blupi.blp` | CHBLUPI (4) | All Blupi animation frames (stop, walk, build, jump, boat, jeep, armour, etc.) |
| `floor000.blp` | CHFLOOR (1) | Floor tiles — normal region (grass, earth, water, rock, ice, shore) |
| `floor001.blp` | CHFLOOR (1) | Floor tiles — palm/tropical region |
| `floor002.blp` | CHFLOOR (1) | Floor tiles — winter/snow region |
| `floor003.blp` | CHFLOOR (1) | Floor tiles — pine forest region |
| `obj000.blp` | CHOBJECT (2) | Objects layer 0 — trees, rocks, buildings, items (normal) |
| `obj001.blp` | CHOBJECT (2) | Objects layer 0 — tropical variant |
| `obj002.blp` | CHOBJECT (2) | Objects layer 0 — winter variant |
| `obj003.blp` | CHOBJECT (2) | Objects layer 0 — pine variant |
| `obj-o000.blp` | CHOBJECTo (3) | Objects overlay 0 — normal (upper parts of tall objects) |
| `obj-o001.blp` | CHOBJECTo (3) | Objects overlay — tropical |
| `obj-o002.blp` | CHOBJECTo (3) | Objects overlay — winter |
| `obj-o003.blp` | CHOBJECTo (3) | Objects overlay — pine |
| `hili.blp` | CHHILI (5) | Highlight / selection overlay icons (hover, select, error, go, build, stat) |
| `fog.blp` | CHFOG (6) | 15 fog-of-war tile patterns |
| `mask1.blp` | CHMASK1 (7) | Alpha mask for Blupi blending |
| `little.blp` | CHLITTLE (8) | Small pixel font (10 px characters) |
| `button.blp` | CHBUTTON (10) | Toolbar action-button icons (all 35 BUTTON_* types) |
| `button00.blp` | CHBUTTON (10) | Button 0 (Go) variant |
| `jauge.blp` | CHJAUGE (12) | Energy / progress gauge sprites |
| `text.blp` | CHTEXT (13) | Main text font (16×16 px characters), white, red and slim variants |
| `bignum.blp` | CHBIGNUM (14) | Large digit sprites for score/timer display |
| `build.blp` | — | Level-editor tool cursor overlays |
| `help.blp` | — | In-game help screen background |
| `history0.blp` | — | Story history screen 0 background |
| `history1.blp` | — | Story history screen 1 background |
| `info000.blp` | — | Mission info/briefing panel 0 |
| `info001.blp` | — | Mission info/briefing panel 1 |
| `info002.blp` | — | Mission info/briefing panel 2 |
| `init.blp` | — | Startup / loading screen background |
| `insert.blp` | — | CD-ROM insert prompt background |
| `intro1.blp` | — | Introduction animation screen 1 |
| `intro2.blp` | — | Introduction animation screen 2 |
| `last000.blp` | — | Final victory screen 0 |
| `last001.blp` | — | Final victory screen 1 |
| `last002.blp` | — | Final victory screen 2 |
| `lost.blp` | — | Defeat screen background |
| `movie.blp` | — | Movie / cinematic placeholder image |
| `music.blp` | — | Music selection screen background |
| `play.blp` | — | Main play screen background (HUD frame) |
| `read.blp` | — | Load-game screen background |
| `region.blp` | — | Region/theme selection screen background |
| `setup.blp` | — | Settings screen background |
| `stop000.blp` | — | Stop-game screen 0 background |
| `stop001.blp` | — | Stop-game screen 1 background |
| `stop002.blp` | — | Stop-game screen 2 background |
| `term.blp` | — | Mission-end (term) statistics screen background |
| `win.blp` | — | Victory screen background |
| `write.blp` | — | Save-game screen background |
| `bye.blp` | — | Quit confirmation screen background |
| `mask1.blp` | CHMASK1 (7) | Transparency mask for Blupi sprites |

---

## Sound Assets (`sound/` directory)

Sound files are also `.blp` but contain raw PCM WAV audio, not image data.
The `sound/` directory is structured as follows:

```
sound/
├── sound000.blp … sound084.blp   (85 SFX files, language-independent)
├── music000.blp … music009.blp   (10 MIDI music tracks)
├── deutsch/
│   └── sound000.blp … sound082.blp  (German voice variants)
├── english/
│   └── sound000.blp … sound082.blp  (English voice variants)
├── francais/
│   └── sound000.blp … sound082.blp  (French voice variants)
└── us/
    └── sound000.blp … sound084.blp  (US English voice variants)
```

The language-specific subdirectories contain localised voice/reaction sounds
that override the base `sound000.blp` … `sound082.blp` files.

---

## Sound Effect Index Reference (SOUND_*)

Sound constants are defined in `include/def.h`.  Each maps to
`sound/soundNNN.blp` (zero-padded three-digit index).

| Constant | Index | Description |
|----------|-------|-------------|
| `SOUND_CLICK` | 0 | UI button click |
| `SOUND_BOING` | 1 | Blupi bouncy acknowledgement (happy) |
| `SOUND_OK1` | 2 | Blupi confirms command (voice 1) |
| `SOUND_OK2` | 3 | Blupi confirms command (voice 2) |
| `SOUND_OK3` | 4 | Blupi confirms command (voice 3) |
| `SOUND_GO1` | 5 | Blupi starts moving (voice 1) |
| `SOUND_GO2` | 6 | Blupi starts moving (voice 2) |
| `SOUND_GO3` | 7 | Blupi starts moving (voice 3) |
| `SOUND_TERM1` | 8 | Blupi finishes task (voice 1) |
| `SOUND_TERM2` | 9 | Blupi finishes task (voice 2) |
| `SOUND_TERM3` | 10 | Blupi finishes task (voice 3) |
| `SOUND_COUPTERRE` | 11 | Digging earth |
| `SOUND_COUPTOC` | 12 | Hitting wood / chopping |
| `SOUND_SAUT` | 13 | Blupi jumping |
| `SOUND_HOP` | 14 | Blupi hop (small jump over obstacle) |
| `SOUND_SCIE` | 15 | Sawing wood |
| `SOUND_FEU` | 16 | Fire burning |
| `SOUND_BRULE` | 17 | Blupi burning / on fire |
| `SOUND_TCHAO` | 18 | Blupi disappearing (bye-bye) |
| `SOUND_MANGE` | 19 | Blupi eating |
| `SOUND_NAISSANCE` | 20 | Birth of new Blupi (hatching) |
| `SOUND_A_SAUT` | 21 | Spider jumping |
| `SOUND_A_HIHI` | 22 | Spider laugh/hiss |
| `SOUND_PLOUF` | 23 | Splash (falling into water) |
| `SOUND_BUT` | 24 | Goal/target reached |
| `SOUND_RAYON1` | 25 | Electro ray beam (start) |
| `SOUND_RAYON2` | 26 | Electro ray beam (sustained) |
| `SOUND_VIRUS` | 27 | Virus spreading / infecting |
| `SOUND_GLISSE` | 28 | Blupi sliding on ice |
| `SOUND_BOIT` | 29 | Blupi drinking (potion) |
| `SOUND_LABO` | 30 | Laboratory working |
| `SOUND_DYNAMITE` | 31 | Dynamite explosion |
| `SOUND_PORTE` | 32 | Door opening/closing |
| `SOUND_FLEUR` | 33 | Picking flowers |
| `SOUND_T_MOTEUR` | 34 | Tracks/tank engine |
| `SOUND_T_ECRASE` | 35 | Tracks/tank crushing object |
| `SOUND_PIEGE` | 36 | Trap triggered |
| `SOUND_AIE` | 37 | Blupi hurt ("ouch!") |
| `SOUND_A_POISON` | 38 | Spider poisoned |
| `SOUND_R_MOTEUR` | 39 | Robot motor running |
| `SOUND_R_APLAT` | 40 | Robot flattening |
| `SOUND_R_ROTATE` | 41 | Robot rotating |
| `SOUND_R_CHARGE` | 42 | Robot recharging |
| `SOUND_B_SAUT` | 43 | Bomb bouncing / hopping |
| `SOUND_BATEAU` | 44 | Boat / water movement |
| `SOUND_JEEP` | 45 | Jeep driving |
| `SOUND_MINE` | 46 | Mine exploding |
| `SOUND_USINE` | 47 | Factory operating |
| `SOUND_E_RAYON` | 48 | Electro tower ray firing |
| `SOUND_E_TOURNE` | 49 | Electro tower rotating |
| `SOUND_ARROSE` | 50 | Blupi watering plants |
| `SOUND_BECHE` | 51 | Blupi digging with spade |
| `SOUND_D_BOING` | 52 | Disciple acknowledgement |
| `SOUND_D_OK` | 53 | Disciple confirms (voice) |
| `SOUND_D_GO` | 54 | Disciple goes (voice) |
| `SOUND_D_TERM` | 55 | Disciple finishes (voice) |
| `SOUND_BOING1` | 56 | Alternative boing 1 |
| `SOUND_BOING2` | 57 | Alternative boing 2 |
| `SOUND_BOING3` | 58 | Alternative boing 3 |
| `SOUND_OK4` | 59 | OK voice 4 |
| `SOUND_OK5` | 60 | OK voice 5 |
| `SOUND_OK6` | 61 | OK voice 6 |
| `SOUND_OK1f` | 62 | OK voice 1 (tired) |
| `SOUND_OK2f` | 63 | OK voice 2 (tired) |
| `SOUND_OK3f` | 64 | OK voice 3 (tired) |
| `SOUND_OK1e` | 65 | OK voice 1 (enemy/electro) |
| `SOUND_OK2e` | 66 | OK voice 2 (enemy) |
| `SOUND_OK3e` | 67 | OK voice 3 (enemy) |
| `SOUND_GO4` | 68 | Go voice 4 |
| `SOUND_GO5` | 69 | Go voice 5 |
| `SOUND_GO6` | 70 | Go voice 6 |
| `SOUND_TERM4` | 71 | Term voice 4 |
| `SOUND_TERM5` | 72 | Term voice 5 |
| `SOUND_TERM6` | 73 | Term voice 6 |
| `SOUND_COUPSEC` | 74 | Dry impact (stone on stone) |
| `SOUND_COUPPIERRE` | 75 | Mining stone |
| `SOUND_COUPSOURD` | 76 | Muffled impact |
| `SOUND_COUPBREF` | 77 | Brief impact |
| `SOUND_OPEN` | 78 | Something opening |
| `SOUND_CLOSE` | 79 | Something closing |
| `SOUND_TELEPORTE` | 80 | Teleporter activating |
| `SOUND_ARMUREOPEN` | 81 | Armour opening |
| `SOUND_ARMURECLOSE` | 82 | Armour closing |
| `SOUND_WIN` | 83 | Mission victory fanfare |
| `SOUND_LOST` | 84 | Mission defeat sound |
| `SOUND_MOVIE` | 99 | Cinematic / AVI movie start sound |

---

## Music Tracks

MIDI music files are stored in `sound/music000.blp` … `sound/music009.blp`
(10 tracks).  They are played via the Windows MCI MIDI subsystem
(`CSound::PlayMusic()`).  Track selection is per-mission and stored in
the world `.blp` file as `m_music`.

> **Note:** MIDI playback does not work in the Web (Emscripten) build.

---

## Data Assets (`data/` directory)

The `data/` directory contains all gameplay data files:

| File/Pattern | Description |
|-------------|-------------|
| `config.def` | Runtime configuration (CD path, window mode, mouse type, speed, timer interval, language) |
| `world000.blp`–`world005.blp` | Tutorial / school missions (6 levels) |
| `world100.blp`–`world153.blp` | Main campaign missions (~55 levels across 3 difficulty episodes) |
| `world200.blp`–`world215.blp` | Advanced / bonus missions (16 levels) |
| `user000.blp`–`user009.blp` | Player save game slots (10 slots) |
| `demo000.blp`–`demo003.blp` | Pre-recorded demo gameplay files |
| `enigmes.blp` | Special puzzle / enigma world |
| `info.blp` | Game information / credits world |

### Mission Numbering

- **Episode 1 (school)**: worlds 000–005
- **Episode 2 (normal campaign)**: worlds 100–137
- **Episode 3 (advanced campaign)**: worlds 138–153
- **Episode 4 (bonus)**: worlds 200–215

Some world numbers are skipped (e.g. 138 is absent), indicating cut content.

---

## Movie Assets (`movie/` directory)

AVI video files for in-game cinematics, played via `CMovie` / Windows MCI.

| Pattern | Description |
|---------|-------------|
| `play101.avi` etc. | Mission briefing movies (shown before specific worlds) |
| `win005.avi`, `win129.avi` | Victory cinematics for specific milestone missions |
| `history2.avi` | Story / history chapter 2 cinematic |

Each AVI file has a matching `.blp` thumbnail/still image used as a
fallback when the movie cannot be played (e.g. in Web builds).

> **Note:** AVI playback does not work in the Web (Emscripten) build.
> The `.blp` thumbnails are displayed instead.

---

## Characters & Entities

All characters are stored as `Blupi` structs in `CDecor::m_blupi[]`.
The `perso` field identifies the character type:

| `perso` | Name | Description |
|---------|------|-------------|
| 0 | **Blupi** | The player character.  Can walk, build, carry objects, use vehicles. |
| 1 | **Spider (Araignée)** | Enemy that poisons Blupi on contact. Can jump obstacles. |
| 2 | **Virus** | Enemy that spreads to adjacent objects and destroys them. |
| 3 | **Tracks (Tank)** | Heavy enemy that crushes objects and Blupi in its path. |
| 4 | **Robot** | Intelligent enemy that builds, mines, and uses ray beams. |
| 5 | **Bomb** | Hopping explosive that seeks Blupi and detonates on contact. |
| 6 | **Mine detonator** | Invisible entity tied to a placed mine; detonates when triggered. |
| 7 | **Electro** | Stationary tower that fires an electric ray at Blupi in range. |
| 8 | **Disciple (Robot2)** | Friendly helper robot that can perform most Blupi tasks autonomously. |

### Vehicles

A Blupi's `vehicule` field indicates which vehicle it occupies:

| `vehicule` | Description |
|-----------|-------------|
| 0 | On foot (default) |
| 1 | In a boat (navigates water cells) |
| 2 | In a jeep (moves faster, crosses rough terrain) |
| 3 | In armour (invulnerable to most attacks) |

---

## Character Actions (ACTION_*)

Animation action codes are defined in `include/def.h`.  They control the
sprite sheet frame sequence played by the `Action()` function in `action.cpp`.

### Blupi Actions

| Constant | Code | Description |
|----------|------|-------------|
| `ACTION_STOP` | 0 | Standing still |
| `ACTION_STOPf` | 1 | Standing still (tired) |
| `ACTION_MARCHE` | 2 | Walking |
| `ACTION_MARCHEf` | 3 | Walking (tired) |
| `ACTION_BUILD` | 4 | Building |
| `ACTION_PIOCHE` | 5 | Mining / digging |
| `ACTION_ENERGY` | 6 | Collecting energy |
| `ACTION_TAKE` | 8 | Picking up object (east) |
| `ACTION_DEPOSE` | 9 | Putting down object |
| `ACTION_SCIE` | 10 | Sawing wood |
| `ACTION_BRULE` | 11 | On fire (burning to death) |
| `ACTION_TCHAO` | 12 | Vanishing / dying |
| `ACTION_MANGE` | 13 | Eating |
| `ACTION_NAISSANCE` | 14 | Being born (hatching from egg) |
| `ACTION_SAUTE2`–`ACTION_SAUTE5` | 15–18 | Jumping over obstacle (4 phases) |
| `ACTION_PONT` | 19 | Pushing a bridge segment |
| `ACTION_MISC1` | 20 | Idle animation: shrugging |
| `ACTION_MISC2` | 21 | Idle animation: scratching |
| `ACTION_MISC3` | 22 | Idle animation: yo-yo |
| `ACTION_MISC1f` | 23 | Idle (tired): bof-bof |
| `ACTION_GLISSE` | 24 | Sliding on ice |
| `ACTION_BOIT` | 25 | Drinking potion |
| `ACTION_LABO` | 26 | Working in laboratory |
| `ACTION_DYNAMITE` | 27 | Setting off dynamite |
| `ACTION_DELAY` | 28 | Waiting one frame |
| `ACTION_CUEILLE1`–`3` | 29–30, 44 | Picking flowers |
| `ACTION_MECHE` | 31 | Covering ears (before explosion) |
| `ACTION_STOPb` | 32 | Standing still in boat |
| `ACTION_MARCHEb` | 33 | Moving in boat |
| `ACTION_STOPj` | 34 | Standing still in jeep |
| `ACTION_MARCHEj` | 35 | Moving in jeep |
| `ACTION_ELECTRO` | 36 | Being electrocuted |
| `ACTION_GRILLE1`–`3` | 37–39 | Being electro-grilled (3 phases) |
| `ACTION_MISC4` | 40 | Idle: closing eyes |
| `ACTION_CONTENT` | 41 | Happy dance |
| `ACTION_ARROSE` | 42 | Watering plants |
| `ACTION_BECHE` | 43 | Digging with spade |
| `ACTION_BUILDBREF`–`BUILDPIERRE` | 45–48 | Build variants (brief, stone, silent, rock) |
| `ACTION_PIOCHEPIERRE`/`PIOCHESOURD` | 49–50 | Mine variants |
| `ACTION_MISC5` | 51 | Idle: waving |
| `ACTION_TELEPORTE1`–`3` | 52–54 | Teleporter animation (3 phases) |
| `ACTION_STOPa` | 55 | Standing in armour |
| `ACTION_MARCHEa` | 56 | Walking in armour |
| `ACTION_ARMUREOPEN`/`CLOSE` | 57–58 | Armour opening / closing |
| `ACTION_SAUTE1` | 59 | Jumping into jeep |
| `ACTION_MISC6` | 60 | Idle: diabolo trick |

### Enemy Actions

| Range | Entity | Description |
|-------|--------|-------------|
| `ACTION_A_*` (100–108) | Spider | Stop, walk, jump, electrocuted, poisoned, death (3 phases) |
| `ACTION_V_*` (200–202) | Virus | Stop, walk, electrocuted |
| `ACTION_T_*` (300–302) | Tracks | Stop, walk, crush |
| `ACTION_R_*` (400–406) | Robot | Stop, walk, flatten, build, delay, recharge, crush |
| `ACTION_B_*` (500–501) | Bomb | Stop, hop |
| `ACTION_D_DELAY` (600) | Mine detonator | Waiting |
| `ACTION_E_*` (700–703) | Electro | Stop, walk, startup, fire ray |
| `ACTION_D_*` (800–810) | Disciple | Full set of Blupi-equivalent actions |

---

## Goals & Task Interpreter (GOAL_*)

High-level multi-step tasks are encoded as sequences of **GOAL_* opcodes**
stored in static tables in `src/decgoal.cpp`.  The interpreter in
`CDecor::GoalNextOp()` executes one opcode per game tick.

| Opcode | Code | Parameters | Description |
|--------|------|-----------|-------------|
| `GOAL_TERM` | 0 | — | End of goal sequence |
| `GOAL_GOHILI` | 1 | dx,dy,bPass | Move to highlighted cell |
| `GOAL_GOBLUPI` | 3 | dx,dy,bPass | Move character to cell |
| `GOAL_PUTFLOOR` | 4 | dx,dy,ch,icon | Place a floor tile |
| `GOAL_PUTOBJECT` | 5 | dx,dy,ch,icon | Place an object tile |
| `GOAL_BUILDFLOOR` | 6 | dx,dy,ch,i,mch,mi,total,delay,step | Animate floor construction |
| `GOAL_BUILDOBJECT` | 7 | dx,dy,ch,i,mch,mi,total,delay,step | Animate object construction |
| `GOAL_ACTION` | 8 | action,direction | Play an animation |
| `GOAL_INTERRUPT` | 9 | level | Set interrupt priority (0=highest) |
| `GOAL_ENERGY` | 10 | min | Require minimum energy |
| `GOAL_ADDMOVES` | 11 | dx,dy,rank | Add movement animation to decoration |
| `GOAL_OTHER` | 12 | ch,f1,l1,f2,l2,action | Execute action on nearby matching object |
| `GOAL_FINISHMOVE` | 13 | — | Wait for animation to finish |
| `GOAL_TAKE` | 14 | dx,dy | Pick up object at cell |
| `GOAL_DEPOSE` | 15 | — | Put down carried object |
| `GOAL_GROUP` | 16 | nb | Group action for multiple Blupi |
| `GOAL_WORK` | 17 | dx,dy | Mark cell as workspace |
| `GOAL_TESTOBJECT` | 18 | dx,dy,ch,icon | Test if object matches |
| `GOAL_FIX` | 19 | dx,dy | Set fixed reference point |
| `GOAL_ADDICONS` | 21 | dx,dy,rank | Add icon sequence to decoration |
| `GOAL_NEWBLUPI` | 22 | dx,dy | Create new Blupi at cell |
| `GOAL_SOUND` | 23 | sound | Play SOUND_* effect |
| `GOAL_REPEAT` | 24 | bool | Enable/disable action repeating |
| `GOAL_OTHERLOOP` | 25 | action | Loop action until target found |
| `GOAL_NEXTLOOP` | 26 | — | Advance loop counter |
| `GOAL_LABO` | 28 | — | Laboratory work step |
| `GOAL_CACHE` | 29 | bool,bDynamite | Hide/show Blupi sprite |
| `GOAL_DELETE` | 30 | — | Delete the character |
| `GOAL_ELECTRO` | 31 | dx,dy,rank | Fire electro ray |
| `GOAL_NEWPERSO` | 32 | dx,dy,perso | Spawn new entity |
| `GOAL_USINEBUILD` | 33 | dx,dy | Factory build step |
| `GOAL_EXPLOSE1/2` | 35,36 | dx,dy | Explosion animation |
| `GOAL_VEHICULE` | 37 | type | Enter/exit vehicle |
| `GOAL_TAKEOBJECT` | 38 | dx,dy,ch,icon | Pick up specific object |
| `GOAL_FLOORJUMP` | 39 | ch,icon,action | Jump depending on floor type |
| `GOAL_ADDDRAPEAU` | 40 | dx,dy | Add flag to path list |
| `GOAL_AMORCE` | 41 | dx,dy | Prime mine detonator |
| `GOAL_MALADE` | 42 | bMalade | Set sick / poisoned state |
| `GOAL_TELEPORTE` | 47 | dx,dy | Teleport to destination |
| `GOAL_ACTUALISE` | 48 | — | Refresh character sprite |
| `GOAL_WAITFREE` | 49 | dx,dy | Wait until target cell is free |

---

## World Grid & Terrain

The world is **200×200 cells** (MAXCELX=200, MAXCELY=200) stored as a
halved array of `Cellule[100][100]` for memory efficiency.

Each cell is **60×30 pixels** in the isometric projection
(DIMCELX=60, DIMCELY=30).

The visible viewport is **480×450 pixels** (DIMDRAWX, DIMDRAWY) within
the 640×480 window, offset by POSDRAWX=144, POSDRAWY=15.

### Coordinate Conversion

- `CDecor::ConvCelToPos(cel)` — grid cell → screen pixel position
- `CDecor::ConvPosToCel(pos)` — screen pixel → grid cell
- The `m_celCoin` field stores the top-left visible cell (scroll position)

---

## Floor Tile Channels

Floor sprites are loaded from `floor000.blp`…`floor003.blp` into
`CHFLOOR` (channel 1) and `CHGROUND` (channel 11).

Key floor icon indices used in auto-tiling (arrange.cpp):

| Icon | Terrain type |
|------|-------------|
| 0 | Standard ground (walkable) |
| 1–12 | Shore/edge transition tiles |
| 13 | Empty/transparent (black border) |
| Water icons | Water cells (boats only) |
| Ice icons | Slippery ice |
| Snow icons | Winter region |

Floor regions are selectable (0=normal, 1=palm, 2=winter, 3=pine),
stored as `m_region` in `CDecor`.

---

## Object Channels

Objects use two channels: `CHOBJECT` (2) for the lower part and
`CHOBJECTo` (3) for the upper part of tall objects (overlapping the cell above).

Key object categories (by icon range within obj000.blp):

| Category | Examples |
|----------|---------|
| Vegetation | Trees, bushes, flowers, moss, grass |
| Resources | Logs, planks, stone, iron ore |
| Buildings | Hut, laboratory, factory, incubator, iron mine |
| Structures | Fence (palisade), wall, bridge, tower |
| Vehicles | Boat, jeep, armour |
| Enemies | Spider, virus entity markers, electro, robot, tracks |
| Items | Dynamite, mine, flag, key, teleporter, egg, tomato, potion |
| Hazards | Fire, trap |
| Special | Hatched cells (mission targets), special tiles |

---

## Toolbar Buttons (BUTTON_*)

Action buttons are defined in `def.h`.  Up to MAXBUTTON=40 buttons
populate the right-hand toolbar during play mode.

| Constant | Code | Action |
|----------|------|--------|
| `BUTTON_GO` | 0 | Move Blupi to target |
| `BUTTON_STOP` | 1 | Stop Blupi |
| `BUTTON_MANGE` | 2 | Eat (nearby food) |
| `BUTTON_CARRY` | 3 | Pick up / carry object |
| `BUTTON_DEPOSE` | 4 | Put down carried object |
| `BUTTON_ABAT` | 5 | Chop down tree |
| `BUTTON_ROC` | 6 | Mine rock |
| `BUTTON_CULTIVE` | 7 | Cultivate / tend crops |
| `BUTTON_BUILD1`–`BUILD6` | 8–13 | Build construction types 1–6 |
| `BUTTON_MUR` | 14 | Build wall |
| `BUTTON_PALIS` | 15 | Build palisade / fence |
| `BUTTON_ABATn` | 16 | Chop without replanting |
| `BUTTON_ROCn` | 17 | Mine without replacing |
| `BUTTON_PONT` | 18 | Build bridge |
| `BUTTON_TOUR` | 19 | Build watchtower |
| `BUTTON_BOIT` | 20 | Drink |
| `BUTTON_LABO` | 21 | Use laboratory |
| `BUTTON_FLEUR` | 22 | Pick flowers |
| `BUTTON_FLEURn` | 23 | Pick flowers (no replant) |
| `BUTTON_DYNAMITE` | 24 | Place dynamite |
| `BUTTON_BATEAU` | 25 | Board boat |
| `BUTTON_DJEEP` | 26 | Board jeep |
| `BUTTON_DRAPEAU` | 27 | Place flag (waypoint) |
| `BUTTON_EXTRAIT` | 28 | Extract iron ore |
| `BUTTON_FABJEEP` | 29 | Manufacture jeep |
| `BUTTON_FABMINE` | 30 | Manufacture mine |
| `BUTTON_FABDISC` | 31 | Create disciple / helper robot |
| `BUTTON_REPEAT` | 32 | Repeat last action |
| `BUTTON_DARMURE` | 33 | Board armour |
| `BUTTON_FABARMURE` | 34 | Manufacture armour |

---

## Win Conditions (Term)

Each world file embeds a `Term` struct specifying what must be achieved to
win the mission.  All non-zero conditions must be satisfied simultaneously.

| Field | Meaning |
|-------|---------|
| `bHachBlupi` | A Blupi must stand on the hatched target cell |
| `bHachPlanche` | Planks must be on the hatched cell |
| `bHachTomate` | Tomatoes must be on the hatched cell |
| `bHachMetal` | Metal must be on the hatched cell |
| `bHachRobot` | A robot must be on the hatched cell |
| `bStopFire` | All fires must be extinguished |
| `nbMinBlupi` | At least this many Blupi must be alive |
| `nbMaxBlupi` | At most this many Blupi may be alive |
| `bHomeBlupi` | All surviving Blupi must be inside a hut |
| `bKillRobots` | All enemies must be destroyed |

---

## Movement Directions (DIRECT_*)

Planet Blupi uses an 8-direction system in multiples of 16
(historically from the action phase counter):

```
        DIRECT_NO (80)
   DIRECT_O (64)    DIRECT_N (96)
         \    |    /
          \   |   /
           \  |  /
DIRECT_SO(48)--o--DIRECT_NE (112)
           /  |  \
          /   |   \
         /    |    \
   DIRECT_S(32)    DIRECT_E (0)
        DIRECT_SE (16)
```

| Constant | Value | Compass |
|----------|-------|---------|
| `DIRECT_E` | 0 | East |
| `DIRECT_SE` | 16 | South-East |
| `DIRECT_S` | 32 | South |
| `DIRECT_SO` | 48 | South-West |
| `DIRECT_O` | 64 | West |
| `DIRECT_NO` | 80 | North-West |
| `DIRECT_N` | 96 | North |
| `DIRECT_NE` | 112 | North-East |

---

## Pathfinding (A* / CPileTriee)

Pathfinding is implemented in `src/chemin.cpp` using `CPileTriee` (a
min-heap sorted priority queue from `src/fifo.cpp`).

The algorithm:
1. `CheminFillTerrain()` builds an obstacle bitmap for the character's type
2. `CDecor::CheminCherche()` runs the search from current cell to goal
3. `CPileTriee::put()` inserts candidate cells sorted by cumulative cost
4. `CPileTriee::get()` retrieves the lowest-cost candidate

The workspace is `CDecor::m_cheminWork[MAXCELX * MAXCELY]` (200×200 bytes).

---

## Fog of War

Fog-of-war is implemented in `src/fog.cpp` and integrated into
`CDecor::BlupiPushFog()`.

Each cell stores a `fog` icon index (0–14) encoding which of 4
quadrant quarters are hidden.  The 15 possible patterns are stored in
`tableFog[15*4]` in fog.cpp.

- `GetFogBits(icon, pBits)` — decode icon → 4 quadrant flags
- `GetFogIcon(pBits)` — encode 4 flags → icon

Fog is enabled during gameplay (`m_bFog = TRUE`) and can be disabled in
the level editor / cheat mode.

---

## Minimap

The minimap (128×128 pixels, positioned at POSMAPX=8, POSMAPY=15) is
generated by `CDecor::GenerateMap()` and displayed in the CHMAP (9)
channel.

Colours are assigned by `CDecor::MapInitColors()` based on floor and object
types.  The player's viewport rectangle is drawn as an outline over the
minimap.  Clicking on the minimap scrolls the viewport
(`CDecor::MapMove()`).

---

## Statistics Panel

The right-hand statistics panel (POSSTATX=12, POSSTATY=220) shows:

- Number of living Blupi
- Number of active fires
- Hatched-cell occupancy counts (Blupi, planks, tomatoes, metal, robots)
- Number of homes and how many are occupied
- Number of remaining enemies

Updated by `CDecor::StatisticUpdate()` and rendered by
`CDecor::StatisticDraw()`.

---

## Demo Recording & Playback

The engine supports recording and replaying gameplay:

- **Header**: `DemoHeader` struct (version, world, skill, mode flags)
- **Events**: array of `DemoEvent` structs (timestamp + Windows message)
- Stored in `data/demo000.blp` … `data/demo003.blp`

Recording: `CEvent::DemoRecStart()` / `DemoRecStop()`
Playback: `CEvent::DemoPlayStart()` — injects recorded events into the
message loop at the correct timestamps via `DemoStep()`.

---

## Localisation

Language-specific voice sounds are stored in subdirectories of `sound/`:

| Directory | Language |
|-----------|---------|
| `sound/english/` | English voice pack |
| `sound/deutsch/` | German voice pack |
| `sound/francais/` | French voice pack |
| `sound/us/` | US English voice pack (extended: 85 files vs 83) |

The active language is selected by the `Language=` key in `data/config.def`
(`F`=French, `E`=English, `D`=German, `U`=US English).

UI strings are stored as Win32 string resources in the `.rc` file and
accessed via `LoadString()` / `TX_*` resource IDs from `include/resource.h`.

---

## Build System & Platforms

The project uses CMake with two optional submodules providing the
Windows API compatibility layer:

```
third_party/
├── SDL/          (SDL3 — input, audio, windowing)
├── SDL_image/    (BLP image loading)
└── SDL_mixer/    (audio mixing)
```

| Target | How to build |
|--------|-------------|
| Linux / Desktop | `cmake -S . -B build && cmake --build build` |
| Web (Emscripten) | `emcmake cmake -S . -B build-web && cmake --build build-web` |
| Android | See `ANDROID.md` |

The main binary is `PLANET_BLUPI_WINDOWS` (name kept for compatibility).

---

## Configuration File

`data/config.def` — plain-text key=value format:

| Key | Values | Description |
|-----|--------|-------------|
| `CD-Rom` | path | CD-ROM data path (legacy, unused on Linux/Web) |
| `FullScreen` | 0/1 | Windowed (0) or full-screen (1) |
| `MouseType` | 1/2/3 | MOUSETYPEGRA=1 (GDI cursor), MOUSETYPEWIN=2 (Win32 cursor), MOUSETYPEWINPOS=3 (positioned) |
| `SpeedRate` | 1/2/3 | Game speed multiplier |
| `Timer` | 50ms | Timer interval (game tick rate) |
| `Language` | F/E/D/U | Active voice language |

---

## Web (Emscripten) Persistence

On the Web build, save-game data is persisted in the browser's
**IndexedDB** via Emscripten's IDBFS virtual filesystem.

Two JavaScript-callable functions are exported from
`src/web_persistence.cpp`:

```javascript
// Flush in-memory saves to IndexedDB
Module.ccall('PlanetBlupi_ExportPersistentData', null, [], []);

// Restore saves from IndexedDB into the virtual filesystem
Module.ccall('PlanetBlupi_ImportPersistentData', null, [], []);
```

Clearing browser site data resets all saved games.

---

## Cheat Codes

The `m_bInvincible` and `m_bSuper` flags in `CDecor` are set by
`CDecor::BlupiCheat()`.  The cheat sequence is tracked by
`CEvent::m_rankCheat` / `m_posCheat`.  When active:

- **Invincible**: Blupi cannot die or be poisoned
- **Super**: Blupi has unlimited energy and enhanced abilities

---

## Skill Levels

Three skill levels affect enemy difficulty and energy drain:

| Level | Description |
|-------|-------------|
| 0 | Easy — slower enemies, more energy |
| 1 | Normal — balanced |
| 2 | Hard — faster enemies, less energy |

Stored as `m_skill` in `CDecor`; set via `WM_PHASE_SKILL1` / `WM_PHASE_SKILL2`.

---

## Regions (World Themes)

The world region selects which set of floor and object sprite sheets to
load, changing the visual theme of the terrain:

| `m_region` | Theme | Floor sheet | Object sheet |
|-----------|-------|------------|-------------|
| 0 | Normal (green / temperate) | `floor000.blp` | `obj000.blp` |
| 1 | Tropical (palm trees) | `floor001.blp` | `obj001.blp` |
| 2 | Winter (snow / ice) | `floor002.blp` | `obj002.blp` |
| 3 | Pine forest | `floor003.blp` | `obj003.blp` |

---

## Undo System

The level editor supports a single-level undo:

- `CDecor::UndoOpen()` — begin recording changes
- `CDecor::UndoCopy()` — snapshot the current world state
- `CDecor::UndoClose()` — finalise the snapshot
- `CDecor::UndoBack()` — restore the snapshot
- `CDecor::IsUndo()` — returns TRUE when an undo snapshot is available

The snapshot is stored in `m_pUndoDecor` (a heap-allocated copy of
`m_decor[100][100]`).

---

## Mouse Cursor Sprites (SPRITE_*)

Custom cursor sprites drawn by CPixmap onto the back buffer:

| Constant | Code | Description |
|----------|------|-------------|
| `SPRITE_ARROW` | 1 | Standard arrow cursor |
| `SPRITE_POINTER` | 2 | Targeting / action pointer |
| `SPRITE_MAP` | 3 | Minimap hand cursor |
| `SPRITE_ARROWU` | 4 | Scroll up |
| `SPRITE_ARROWD` | 5 | Scroll down |
| `SPRITE_ARROWL` | 6 | Scroll left |
| `SPRITE_ARROWR` | 7 | Scroll right |
| `SPRITE_ARROWUL` | 8 | Scroll upper-left |
| `SPRITE_ARROWUR` | 9 | Scroll upper-right |
| `SPRITE_ARROWDL` | 10 | Scroll lower-left |
| `SPRITE_ARROWDR` | 11 | Scroll lower-right |
| `SPRITE_WAIT` | 12 | Busy / loading |
| `SPRITE_EMPTY` | 13 | Empty (hidden cursor area) |
| `SPRITE_FILL` | 14 | Flood-fill cursor |

---

## Error Codes (ERROR_*)

Displayed as tooltip messages when an action cannot be performed:

| Constant | Code | Reason |
|----------|------|--------|
| `ERROR_MISC` | 1 | Generic / miscellaneous error |
| `ERROR_GROUND` | 2 | Wrong terrain type |
| `ERROR_FREE` | 3 | Cell is occupied |
| `ERROR_PONTOP` | 4 | Bridge: cell on opposite bank is blocked |
| `ERROR_PONTTERM` | 5 | Bridge: no valid terminus found |
| `ERROR_TOURISOL` | 6 | Tower: must be built on land |
| `ERROR_TOUREAU` | 7 | Tower: must be next to water |
| `ERROR_TELE2` | 8 | Teleporter: no matching second teleporter |
