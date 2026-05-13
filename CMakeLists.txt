cmake_minimum_required(VERSION 3.16)

project(PLANET_BLUPI_WINDOWS LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Include-path hygiene rules for this build
# -----------------------------------------------------------------------------
# free-api ships compatibility headers that intentionally use legacy WinAPI/CRT
# names (windows.h, io.h, direct.h, ...). On MinGW, exposing such a directory
# globally (e.g. via include_directories()) silently shadows MinGW's own
# headers and breaks libstdc++ <cmath>/<cstdio> resolution while compiling
# bundled SDL.
#
# Therefore:
#   * DO NOT use bare include_directories(...) at this scope or above SDL.
#   * DO NOT prepend project root, src/, include/ or freeapi compat dirs to
#     CMAKE_CXX_FLAGS / CMAKE_C_FLAGS or to a global INTERFACE target.
#   * Use target_include_directories(<our-target> PRIVATE/PUBLIC ...) only.
#   * Compatibility headers are owned by the dedicated INTERFACE target
#     `freeapi_compat_headers` (defined in free-api/CMakeLists.txt) and are
#     reachable ONLY through `free-api` -> game/free-direct. SDL and other
#     third_party targets must never link against `freeapi_compat_headers`.
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/ThirdPartySDL.cmake)

# Backend selection:
#   FREEDIRECT - native DirectX 3 replacement backend
set(PLANET_BLUPI_BACKEND "FREEDIRECT" CACHE STRING "Backend: FREEDIRECT")
set_property(CACHE PLANET_BLUPI_BACKEND PROPERTY STRINGS FREEDIRECT)

# Setting the output file
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Adding source code files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# web_persistence.cpp is included via GLOB above when __EMSCRIPTEN__;
# on native builds the file compiles to an empty translation unit (guarded
# by #if defined(__EMSCRIPTEN__)) so no special exclusion is needed.

# On Android the "executable" is a shared library loaded by SDLActivity.
# SDL's Java glue expects the native library to be named "libmain.so".
if(ANDROID)
    set(_planet_blupi_target main)
    add_library(${_planet_blupi_target} SHARED ${SOURCES})
else()
    set(_planet_blupi_target PLANET_BLUPI_WINDOWS)
    add_executable(${_planet_blupi_target} ${SOURCES})
endif()

# On Emscripten the output is HTML+JS+WASM; set the suffix accordingly.
if(EMSCRIPTEN)
    set_target_properties(${_planet_blupi_target} PROPERTIES SUFFIX ".html")
endif()

target_include_directories(${_planet_blupi_target}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(${_planet_blupi_target}
            PRIVATE
            /permissive-
            /W0
    )
else()
    target_compile_options(${_planet_blupi_target}
            PRIVATE
            -fpermissive
            -fms-extensions
            -fPIC
            -w
            -Wno-narrowing
            -Wno-int-to-pointer-cast
            -g
            -O0
    )
endif()

if (PLANET_BLUPI_BACKEND STREQUAL "FREEDIRECT")
    message(STATUS "Using FREEDIRECT backend")

    target_compile_definitions(${_planet_blupi_target} PRIVATE FREEDIRECT)
    configure_vendored_sdl()

    # Diagnostics: enable internal counters & heartbeat in Free Direct (1 s snapshots).
    # Default ON in this build; set FREE_DIRECT_DIAGNOSTICS=OFF to disable. Counters
    # are no-ops at runtime unless env var FREE_DIRECT_DIAGNOSTICS=1 is also set.
    set(FREE_DIRECT_DIAGNOSTICS ON CACHE BOOL "Enable Free Direct diagnostics counters")

    # Disable free-api tests by default when building for Emscripten or Android
    # (they are not runnable in those environments).
    if(EMSCRIPTEN OR ANDROID)
        set(FREE_API_BUILD_TESTS OFF CACHE BOOL "Build free-api tests" FORCE)
    endif()

    add_subdirectory(../free-api FREE_API)

    if(ANDROID)
        target_link_libraries(free-api PUBLIC android log)
    endif()

    if(ANDROID)
        add_subdirectory(../free-direct FREE_DIRECT EXCLUDE_FROM_ALL)
    else()
        add_subdirectory(../free-direct FREE_DIRECT)
    endif()
    if(ANDROID)
        # Android uses lld which resolves circular static-lib references without
        # --start-group. Link android and log for JNI/logging support.
        target_link_libraries(${_planet_blupi_target}
                PRIVATE
                free-api
                free-direct
                SDL3::SDL3
                android
                log
        )
    else()
        target_link_libraries(${_planet_blupi_target}
                PRIVATE
                free-api
                free-direct
        )
    endif()

    # Emscripten-specific link options: produce HTML output and preload game data
    # directly from the repository asset directories into the virtual filesystem.
    # Asset layout in the virtual FS mirrors the on-disk layout so that the
    # game's relative paths (data/, image/, sound/, movie/) resolve unchanged.
    if(EMSCRIPTEN)
        target_link_options(${_planet_blupi_target} PRIVATE
            -sUSE_SDL=0
            -sALLOW_MEMORY_GROWTH=1
            -sINITIAL_MEMORY=67108864
            -sASYNCIFY
            # ASYNCIFY_IMPORTS: list every C function that may block (SDL_Delay
            # is called inside GetMessageA; the others are defensive extras).
            "-sASYNCIFY_IMPORTS=['SDL_Delay','SDL_WaitEvent','SDL_WaitEventTimeout']"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/data@/data"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/image@/image"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/sound/english@/sound"
            "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/movie@/movie"
            -sFORCE_FILESYSTEM=1
            "-sEXPORTED_RUNTIME_METHODS=['FS','ccall','cwrap']"
            "-sEXPORTED_FUNCTIONS=['_main','_PlanetBlupi_ExportPersistentData','_PlanetBlupi_ImportPersistentData']"
        )
    endif()

    if(NOT ANDROID)
        planet_blupi_copy_sdl_runtime(${_planet_blupi_target})
        planet_blupi_copy_mingw_runtime(${_planet_blupi_target})
    endif()

else()
    message(FATAL_ERROR "Unknown PLANET_BLUPI_BACKEND: ${PLANET_BLUPI_BACKEND}")
endif()
