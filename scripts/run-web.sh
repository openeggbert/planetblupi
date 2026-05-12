#!/usr/bin/env bash
# run-web.sh — Serve the Planet Blupi Web build in the default browser.
#
# Usage:
#   bash scripts/run-web.sh
#
# The script tries emrun first (part of the Emscripten SDK), then falls back
# to a plain Python HTTP server.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build-web"
HTML="$BUILD_DIR/bin/PLANET_BLUPI_WINDOWS.html"

if [ ! -f "$HTML" ]; then
    echo "ERROR: '$HTML' not found."
    echo "Run 'bash scripts/build-web.sh' first."
    exit 1
fi

if command -v emrun &>/dev/null; then
    echo "Starting with emrun..."
    emrun "$HTML"
else
    echo "emrun not found; starting Python HTTP server on port 8080."
    echo "Open: http://localhost:8080/PLANET_BLUPI_WINDOWS.html"
    python3 -m http.server 8080 --directory "$BUILD_DIR/bin"
fi
