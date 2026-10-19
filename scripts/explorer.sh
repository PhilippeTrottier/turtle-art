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

if [[ -z "${EMSDK:-}" ]]; then
  echo "error: EMSDK is not set; install the Emscripten SDK (see README)" >&2
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

echo "serving http://localhost:8001/ (Ctrl+C to stop)"
# `exec` replaces this script with the server, so stopping it ends both.
exec "$python" -m http.server 8001 --bind 127.0.0.1 --directory build/wasm/web
