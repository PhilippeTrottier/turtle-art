#!/usr/bin/env bash
# Renders a few sample pictures and a gallery page that shows them.
#
# Usage: scripts/make-gallery.sh [--serve] [path/to/turtle-art]
#
# Build the program first (see README). Without a path, the script uses the
# first build it finds under build/. Output goes to gallery/ (ignored by git).
#
# With --serve, it then serves gallery/ at http://localhost:8000/demo.html
# (Python's built-in web server; stop it with Ctrl+C). Browsers, and Claude's
# built-in browser in particular, may refuse to open local files directly.
#
# Runs with bash on Linux and macOS, and with Git Bash on Windows.

# Stop on the first failing command, on unset variables, and on failures
# inside pipelines.
set -euo pipefail

repo_root="$(cd "$(dirname "$0")/.." && pwd)"

serve=false
program=""
for arg in "$@"; do
  if [[ "$arg" == "--serve" ]]; then
    serve=true
  else
    program="$arg"
  fi
done

if [[ -z "$program" ]]; then
  for candidate in "$repo_root"/build/*/src/turtle-art "$repo_root"/build/*/src/turtle-art.exe; do
    if [[ -x "$candidate" ]]; then
      program="$candidate"
      break
    fi
  done
fi
if [[ -z "$program" || ! -x "$program" ]]; then
  echo "error: turtle-art not found; build it first or pass its path" >&2
  exit 1
fi

out_dir="$repo_root/gallery"
mkdir -p "$out_dir"

# Each sample: "seed steps radius caption". Seeds 228, 137, 146, 78 and 292
# covered the most tiles in a survey of seeds 1-300 (see QUESTIONS.md).
# Seeds 11 and 6 show the two common endings, on a small grid: walking off
# the edge, and looping on a few tiles.
samples=(
  "228 10000 50 Default seed: one of the few that roam widely"
  "137 10000 50 Second-largest area among seeds 1-300"
  "146 10000 50 Seed 146"
  "78 10000 50 Seed 78"
  "292 10000 50 Seed 292"
  "11 10000 12 Typical ending 1: the turtle walks off the edge"
  "6 10000 12 Typical ending 2: the turtle loops on a few tiles"
)

figures=""
for sample in "${samples[@]}"; do
  # `read` splits the string on spaces; the caption takes the rest.
  read -r seed steps radius caption <<< "$sample"
  file="seed-$seed.svg"
  "$program" --seed "$seed" --steps "$steps" --radius "$radius" \
    --out "$out_dir/$file" > /dev/null
  echo "wrote gallery/$file"
  figures+="
      <figure>
        <a href=\"$file\"><img src=\"$file\" alt=\"Turtle art for seed $seed\" loading=\"lazy\"></a>
        <figcaption><strong>$caption</strong><br>
          <code>--seed $seed --steps $steps --radius $radius</code></figcaption>
      </figure>"
done

cat > "$out_dir/demo.html" <<EOF
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>turtle-art gallery</title>
  <style>
    :root {
      --bg: #faf8f3; --card: #ffffff; --text: #222222; --muted: #666666;
      --border: #e4dfd3;
    }
    @media (prefers-color-scheme: dark) {
      :root {
        --bg: #1b1d21; --card: #25282d; --text: #e8e6e1; --muted: #a0a0a0;
        --border: #3a3e45;
      }
    }
    body {
      margin: 0; padding: 24px 16px; background: var(--bg); color: var(--text);
      font: 15px/1.5 system-ui, sans-serif;
    }
    main { max-width: 1200px; margin: 0 auto; }
    h1 { margin: 0 0 4px; font-size: 24px; }
    p.lede { margin: 0 0 24px; color: var(--muted); }
    .grid {
      display: grid; gap: 16px;
      grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
    }
    figure {
      margin: 0; padding: 12px; background: var(--card);
      border: 1px solid var(--border); border-radius: 8px;
    }
    figure img { display: block; width: 100%; height: auto; }
    figcaption { margin-top: 8px; font-size: 14px; color: var(--muted); }
    figcaption strong { color: var(--text); font-weight: 600; }
    code { font-size: 12px; }
  </style>
</head>
<body>
  <main>
    <h1>turtle-art gallery</h1>
    <p class="lede">Each picture is one turtle on a hex grid, following a rule
      table generated from its seed. Click a picture to open it full size.</p>
    <div class="grid">$figures
    </div>
  </main>
</body>
</html>
EOF
echo "wrote gallery/demo.html"

if [[ "$serve" == true ]]; then
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
    echo "error: --serve needs Python 3 on the PATH" >&2
    exit 1
  fi
  echo "serving http://localhost:8000/demo.html (Ctrl+C to stop)"
  # `exec` replaces this script with the server, so stopping it ends both.
  exec "$python" -m http.server 8000 --bind 127.0.0.1 --directory "$out_dir"
fi
