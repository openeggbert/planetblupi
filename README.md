# Planet Blupi

## Introduction

**Planet Blupi** is the original source code of the classic 1997 strategy/adventure game of the same name, developed by Epsitec SA. The original Windows/DirectX codebase has been made buildable and portable across Linux, and Web (Emscripten), using the [Free Direct](https://github.com/openeggbert/free-direct) and [Free API](https://github.com/openeggbert/free-api) compatibility layers.

## Current Status

The game is playable on Linux and is being ported to the browser via Emscripten. Desktop builds are stable. The Web build is functional but some features (MIDI music, AVI movies) are not yet supported in the browser.

## Used Technologies

- C++
- SDL3 (via vendored submodules)
- Free Direct (DirectDraw/DirectSound compatibility layer)
- Free API (Win32 API compatibility layer)
- CMake
- Emscripten (for Web/browser builds)

## Goals

- [x] Build and run on Linux
- [x] Add Web/Emscripten build support
- [ ] Restore MIDI music playback in the Web build
- [ ] Restore AVI movie playback in the Web build
- [ ] Investigate Android support

## Local Setup

### Clone the Repository

After cloning the repository, initialize and update submodules:

```bash
git submodule update --init --recursive
```

This project vendors SDL via git submodules under `third_party/`:

- `third_party/SDL`
- `third_party/SDL_image`
- `third_party/SDL_mixer`

### Add Game Asset Files

Place the original game asset directories in the repository root:

- `data/`
- `image/`
- `sound/`
- `movie/`

These directories are expected by the game at runtime.

## Building for Linux / Desktop

```bash
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bin/PLANET_BLUPI_WINDOWS
```

## Building for Web with Emscripten

First, install and activate the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html):

```bash
source /path/to/emsdk/emsdk_env.sh
```

Then configure, build, and run:

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web -j
emrun build-web/bin/PLANET_BLUPI_WINDOWS.html
```

Or use the provided scripts:

```bash
bash scripts/build-web.sh
bash scripts/run-web.sh
```

Game assets (`data/`, `image/`, `sound/`, `movie/`) are preloaded from the repository root into the Emscripten virtual filesystem at build time — no manual copying is required.

### Persistent saves on Web

User config and save-game data are stored in the browser's **IndexedDB** (via Emscripten IDBFS) and persist across page reloads. Clearing browser site data resets persistence.

Export and import save data from the browser console:

```js
// Flush in-memory save data to IndexedDB
Module.ccall('PlanetBlupi_ExportPersistentData', null, [], []);

// Restore save data from IndexedDB into the virtual filesystem
Module.ccall('PlanetBlupi_ImportPersistentData', null, [], []);
```

## Known Limitations

- **MIDI music**: MCI-based MIDI playback does not work in Web builds. The game starts without music in the browser.
- **AVI movies**: MCI-based video playback does not work in Web builds. Cinematic sequences are skipped gracefully.
- **ASYNCIFY overhead**: The Web build uses Emscripten ASYNCIFY to support the blocking Win32 message loop. This roughly doubles the `.wasm` file size compared to a non-ASYNCIFY build.

## Disclaimer

Planet Blupi is the original game by Epsitec SA. This repository contains the source code as released or made available by the original authors, adapted for modern build systems and portability.
