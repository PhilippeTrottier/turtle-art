#!/usr/bin/env bash
# Builds the web explorer (WebAssembly) and serves it at
# http://localhost:8001/ (Ctrl+C to stop).
#
# Usage: scripts/explorer.sh
#
# Needs the Emscripten SDK with EMSDK pointing to it, and Python 3 for the
# web server (see README). Runs with bash on Linux and macOS, and with Git
# Bash on Windows.

# Stop on the first failing command, on unset variables, and on failures
# inside pipelines.
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$repo_root"

# The wasm preset finds Emscripten through EMSDK and vcpkg through
# VCPKG_ROOT; default to ~/emsdk and ~/vcpkg.
export EMSDK="${EMSDK:-$HOME/emsdk}"
export VCPKG_ROOT="${VCPKG_ROOT:-$HOME/vcpkg}"
if [[ ! -d "$EMSDK/upstream/emscripten" ]]; then
  echo "error: Emscripten SDK not found at $EMSDK; install it or set EMSDK" \
       "(see README)" >&2
  exit 1
fi
if [[ ! -d "$VCPKG_ROOT" ]]; then
  echo "error: vcpkg not found at $VCPKG_ROOT; install it or set VCPKG_ROOT" \
       "(see README)" >&2
  exit 1
fi

cmake --preset wasm > /dev/null
cmake --build --preset wasm

# The Python command is python3 on Linux and macOS, often python on
# Windows; use the first one that exists.
python=""
for candidate in python3 python py; do
  if command -v "$candidate" > /dev/null; then
    python="$candidate"
    break
  fi
done
if [[ -z "$python" ]]; then
  echo "error: serving needs Python 3 on the PATH" >&2
  exit 1
fi

# PORT lets a launcher (like Claude Code's preview) choose the port.
port="${PORT:-8001}"
echo "serving http://localhost:$port/ (Ctrl+C to stop)"
# `exec` replaces this script with the server, so stopping it ends both.
exec "$python" -m http.server "$port" --bind 127.0.0.1 --directory build/wasm/web
