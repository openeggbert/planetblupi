// web_persistence.cpp
//
// Emscripten-specific helpers for persisting save-game data between browser
// sessions via the IndexedDB-backed IDBFS virtual filesystem.
//
// On non-Emscripten builds this translation unit is intentionally empty so it
// can be included unconditionally via the GLOB in CMakeLists.txt.

#if defined(__EMSCRIPTEN__)

#include <emscripten.h>
#include <emscripten/html5.h>
#include <cstdio>

// ---------------------------------------------------------------------------
// Called from JavaScript (or from the browser console) to flush the Emscripten
// virtual filesystem's IDBFS mount to IndexedDB.  Exported as
// _PlanetBlupi_ExportPersistentData.
// ---------------------------------------------------------------------------
EMSCRIPTEN_KEEPALIVE
extern "C" void PlanetBlupi_ExportPersistentData()
{
    EM_ASM({
        if (typeof FS !== 'undefined' && FS.syncfs) {
            FS.syncfs(false, function(err) {
                if (err) {
                    console.error('PlanetBlupi: FS.syncfs (write) failed:', err);
                } else {
                    console.log('PlanetBlupi: save data synced to IndexedDB.');
                }
            });
        }
    });
}

// ---------------------------------------------------------------------------
// Called at startup to restore previously saved data from IndexedDB into the
// virtual filesystem.  Exported as _PlanetBlupi_ImportPersistentData.
// ---------------------------------------------------------------------------
EMSCRIPTEN_KEEPALIVE
extern "C" void PlanetBlupi_ImportPersistentData()
{
    EM_ASM({
        if (typeof FS !== 'undefined' && FS.syncfs) {
            FS.syncfs(true, function(err) {
                if (err) {
                    console.error('PlanetBlupi: FS.syncfs (read) failed:', err);
                } else {
                    console.log('PlanetBlupi: save data loaded from IndexedDB.');
                }
            });
        }
    });
}

#endif // __EMSCRIPTEN__
