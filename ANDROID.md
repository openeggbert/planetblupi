# Android Build Instructions — Planet Blupi

This document explains how to build, install, and run Planet Blupi as an
Android APK using SDL3, the Android NDK, and CMake.

---

## Prerequisites

| Tool | Recommended version |
|------|---------------------|
| Android Studio | Ladybug (2024.2) or newer |
| Android SDK | API level 35 |
| Android NDK | 30.0.14904198 (installed via SDK Manager) |
| CMake (NDK bundle) | 3.21+ (installed via SDK Manager) |
| Java (JDK) | 17 (bundled with Android Studio) |
| Git | any recent version |

### Install NDK and CMake via Android Studio

1. Open **Android Studio → Settings → SDK Manager → SDK Tools**.
2. Check **NDK (Side by side)** version **30.0.14904198**.
3. Check **CMake** (version 3.21 or higher).
4. Click **Apply** and let Android Studio download and install.

---

## Clone and initialise submodules

```bash
git clone <repository-url> planetblupi
cd planetblupi
git submodule update --init --recursive
```

The vendored SDL3 / SDL_image / SDL_mixer sources are required. They live under
`third_party/`.

---

## Prepare game assets

Game assets must be present in the Android assets directory before building.
Run these commands from the project root:

```bash
cd android/app/src/main/assets

# Symbolic links (recommended on Linux/macOS — saves disk space)
ln -s ../../../../../data  data
ln -s ../../../../../image image
ln -s ../../../../../sound/english sound
ln -s ../../../../../movie movie
```

If your build system does not follow symbolic links, copy the directories instead:

```bash
cp -r ../../../../../data  data
cp -r ../../../../../image image
cp -r ../../../../../sound/english sound
cp -r ../../../../../movie movie
```

> Other sound locales (deutsch, francais, us) can replace the `sound` link
> if you want to ship a different language.

---

## Configure local.properties

Create the file `android/local.properties` (based on the provided template):

```bash
cp android/local.properties.template android/local.properties
# Edit the file and set sdk.dir to your Android SDK path, e.g.:
# sdk.dir=/home/user/Android/Sdk
```

---

## Build the debug APK

```bash
cd android
./gradlew assembleDebug
```

The first build downloads Gradle (≈ 150 MB) and compiles the NDK libraries, so
it may take several minutes. Subsequent incremental builds are much faster.

The produced APK is at:

```
android/app/build/outputs/apk/debug/app-debug.apk
```

---

## Install and run on a device or emulator

Enable **USB debugging** on your device (or start an AVD in Android Studio).

```bash
# Install
adb install app/build/outputs/apk/debug/app-debug.apk

# Launch
adb shell am start -n org.openeggbert.planetblupi/.PlanetBlupiActivity

# View logs (filter by the app's tag or by pid)
adb logcat -s SDL PlanetBlupi
```

To uninstall:

```bash
adb uninstall org.openeggbert.planetblupi
```

---

## Creating a release signing key

Run this once to generate a keystore. Keep the keystore and passwords safe — you
will need them for every future update.

```bash
cd android
keytool -genkeypair -v \
  -keystore planet-blupi-release.keystore \
  -alias planet-blupi \
  -keyalg RSA \
  -keysize 2048 \
  -validity 10000
```

> ⚠️ **Do not commit the keystore to version control.**
> ⚠️ **Do not lose the keystore.** If it is lost, you cannot publish future updates
> signed with the same key — users will have to uninstall and reinstall the app.

---

## Creating key.properties

Create the file `android/key.properties` (one directory above `app/`) with your
real passwords:

```properties
storeFile=planet-blupi-release.keystore
storePassword=YOUR_STORE_PASSWORD
keyAlias=planet-blupi
keyPassword=YOUR_KEY_PASSWORD
```

> ⚠️ **Do not commit `key.properties` to version control.**
> Both `key.properties` and `*.keystore` are listed in `.gitignore`.

---

## Building a signed release APK

```bash
cd android
./gradlew clean assembleRelease
```

Release APK output:

```
android/app/build/outputs/apk/release/app-release.apk
```

If `key.properties` is not present, the build will stop immediately with a clear
error message rather than a confusing Gradle failure.

---

## Verifying APK signature

```bash
apksigner verify --verbose app/build/outputs/apk/release/app-release.apk
```

---

## Installing a release APK on a phone

```bash
adb install -r app/build/outputs/apk/release/app-release.apk
```

> Users who install the APK from outside the Play Store may need to enable
> **Install unknown apps** in their Android settings (Settings → Apps → Special
> app access → Install unknown apps).

---

## Supported ABIs

The current Gradle configuration builds for **arm64-v8a** only. To add other
ABIs (e.g. `x86_64` for the emulator) edit
`android/app/build.gradle` and extend the `abiFilters` list:

```groovy
abiFilters 'arm64-v8a', 'x86_64'
```

---

## Asset layout

Game assets are packaged into the APK as Android assets. At first launch,
FreeApi automatically extracts them to the app's internal storage so that
the game code can access them via the standard C `fopen()` API (see
[FreeApi Android startup](#freeapi-android-startup-and-asset-extraction) below).

| Source directory         | Path in APK |
|--------------------------|-------------|
| `data/`                  | `data/`     |
| `image/`                 | `image/`    |
| `sound/english/`         | `sound/`    |
| `movie/`                 | `movie/`    |

---

## Writable / persistent storage

Save data and configuration are written to the app's private internal storage
via `SDL_GetPrefPath("org.openeggbert", "planetblupi")`. This storage:

- persists across app restarts,
- is cleared when the app is uninstalled,
- is **not** accessible to other apps.

---

## Android launcher icon

The launcher icon is generated from `icon.bmp` (or any square source image)
into `res/mipmap-*` PNG files. Android does not use BMP directly for launcher icons.

If the icon changes, regenerate the PNG density variants with ImageMagick:

```bash
# Flat icons
convert icon.bmp -resize 48x48   android/app/src/main/res/mipmap-mdpi/ic_launcher.png
convert icon.bmp -resize 72x72   android/app/src/main/res/mipmap-hdpi/ic_launcher.png
convert icon.bmp -resize 96x96   android/app/src/main/res/mipmap-xhdpi/ic_launcher.png
convert icon.bmp -resize 144x144 android/app/src/main/res/mipmap-xxhdpi/ic_launcher.png
convert icon.bmp -resize 192x192 android/app/src/main/res/mipmap-xxxhdpi/ic_launcher.png

# Round icons (circular crop)
for density_size in "mdpi:48" "hdpi:72" "xhdpi:96" "xxhdpi:144" "xxxhdpi:192"; do \
  density="${density_size%%:*}"; size="${density_size##*:}"; half=$((size/2)); \
  convert icon.bmp -resize ${size}x${size} \
    \( +clone -alpha extract -draw "fill white circle ${half},${half} ${half},0" \) \
    -alpha off -compose CopyOpacity -composite \
    android/app/src/main/res/mipmap-${density}/ic_launcher_round.png; \
done
```

---

## Architecture notes

Planet Blupi uses the **FreeDirect / FreeApi** abstraction layer to run the
original WinAPI/DirectX game code on SDL3 without modification. On Android:

- The game is built as `libmain.so` (shared library) instead of an executable.
- SDL3 is built as `libSDL3.so` (shared library) and loaded by `SDLActivity`.
- `free-api` and `free-direct` are linked statically into `libmain.so`.
- SDL3's Java glue (`SDLActivity`) handles lifecycle, input, audio, and surface
  management automatically.
- Game assets are read via `SDL_IOFromFile`, which transparently accesses the
  APK `AAssetManager` on Android.
- Persistent data (saves, config) is written via `SDL_GetPrefPath`.
- No WinAPI, DirectX SDK, Wine, MonoGame, or .NET is required.

---

## FreeApi Android startup and asset extraction

### Why extraction is needed

On Android, files bundled in an APK are only accessible through the Android
Asset Manager (or `SDL_IOFromFile`). Standard C `fopen()` with a relative path
looks in the process working directory, which is **not** the APK asset tree.
Because the original game code uses `fopen()` throughout, FreeApi extracts all
required assets to the app's internal storage on the first launch.

### How it works

`FreeApiAndroidSetup()` runs at the start of `SDL_main` (before `WinMain` is
called) and performs the following steps:

1. Reads `freeapi_android_assets.txt` from the APK assets root.  This manifest
   lists one top-level asset directory per line (e.g. `data`, `image`, `sound`,
   `movie`).
2. For each listed directory, recursively enumerates APK assets using
   `SDL_EnumerateDirectory` (which falls back to the APK Asset Manager) and
   reads each file via `SDL_IOFromFile`.
3. Writes every file into `SDL_GetAndroidInternalStoragePath()/<relpath>`,
   creating parent directories as needed.
4. Writes a sentinel file `.freeapi_extracted` so that extraction is skipped
   on subsequent launches.
5. Calls `chdir(SDL_GetAndroidInternalStoragePath())` so that every later
   `fopen(relative)` call resolves inside internal storage.

### The manifest file

`android/app/src/main/assets/freeapi_android_assets.txt` must list every
top-level directory that the game needs to access via `fopen()`:

```
data
image
sound
movie
```

Any FreeApi game targeting Android must ship this manifest in its APK assets
root and list all directories required by the game.

### Re-extraction after an app update

The sentinel `.freeapi_extracted` prevents re-extraction on every launch.
If assets change between app versions, delete the sentinel file or clear the
app data on the device:

```bash
adb shell pm clear org.openeggbert.planetblupi
```

---

## Troubleshooting

### Gradle build fails: "NDK not found"

Make sure the NDK version in `android/app/build.gradle` (`ndkVersion`)
matches what you installed in the SDK Manager.

### `adb: device not found`

- Enable **Developer Options** and **USB debugging** on your Android device.
- Try `adb devices` to confirm the device is listed.

### App exits immediately after "Running main function"

If logcat shows `Finished main function` almost immediately after `Running main
function SDL_main`, the game's `WinMain` is returning early.  The most common
cause is that FreeApi's asset extraction failed and `fopen("data/config.def")`
returned NULL.

Check logcat for `free-api:` log lines:

```bash
adb logcat -s SDL
```

Look for:
- `free-api: FreeApiAndroidSetup begin` — confirms setup ran.
- `free-api: internal storage path = …` — confirms path was obtained.
- `free-api: extracting APK assets to …` — confirms extraction started.
- `free-api: freeapi_android_assets.txt not found` — manifest is missing from
  the APK assets; add `android/app/src/main/assets/freeapi_android_assets.txt`.
- `free-api: SDL_main exiting ret=0` — WinMain returned FALSE; check
  initialisation errors earlier in the log.

### Message loop exits immediately (WM_QUIT posted at startup)

If logcat shows `FREEAPI_ANDROID: GetMessage returning FALSE (WM_QUIT)` shortly
after `GetMessage entered`, a quit message was posted before the game loop had a
chance to run.  Check which path triggered it:

```bash
adb logcat -s SDL | grep FREEAPI_ANDROID
```

Key tags to look for:

| Log tag | Meaning |
|---------|---------|
| `FREEAPI_ANDROID: PostQuitMessage called` | `PostQuitMessage()` was called — note the `nExitCode` |
| `FREEAPI_ANDROID: SDL event SDL_EVENT_QUIT translated to WM_QUIT` | SDL itself sent a quit event |
| `FREEAPI_ANDROID: SDL event SDL_EVENT_WINDOW_CLOSE_REQUESTED … translated to WM_CLOSE` | SDL requested the window be closed; WM_CLOSE was forwarded to the WndProc |
| `FREEAPI_ANDROID: DefWindowProcA WM_CLOSE -> DestroyWindow + PostQuitMessage` | WM_CLOSE fell through to `DefWindowProcA` (game did not handle it) |
| `FREEAPI_ANDROID: DefWindowProcA WM_DESTROY -> PostQuitMessage` | WM_DESTROY fell through to `DefWindowProcA` |

**What FreeApi does on Android (GetMessage / WaitMessage):**
- Uses `SDL_WaitEventTimeout(NULL, 10)` instead of `SDL_Delay(1)` so the
  Android event system receives proper CPU time.
- `GetMessage` still returns `FALSE` **only** on `WM_QUIT`; no other condition
  terminates the loop.
- Focus-lost events (`SDL_EVENT_WINDOW_FOCUS_LOST`) are suppressed and do **not**
  generate `WM_ACTIVATEAPP(0)`, preventing premature deactivation.

### App crashes on launch

Check `adb logcat` for native crash details. Common causes:

- Missing `libmain.so` — rebuild with `./gradlew assembleDebug`.
- Asset not found — verify that `data/`, `image/`, `sound/`, and `movie/`
  exist under `android/app/src/main/assets/` and are correctly linked or copied.
- `git submodule update --init --recursive` not completed.

### Audio is silent

SDL_mixer must be built with OGG/WAV support. The vendored build enables WAV
by default. If you see mixer errors in logcat, check that the sound files are
present in the APK using:

```bash
aapt dump resources app/build/outputs/apk/debug/app-debug.apk | grep sound
```

### Desktop Linux build broken after these changes

The Android changes in `CMakeLists.txt` and `cmake/ThirdPartySDL.cmake` are
fully conditional on `if(ANDROID)`. Desktop builds are unaffected.
