#!/usr/bin/env bash
# Renders sample pictures, grouped by theme, and a gallery page with one tab
# per theme.
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
# Runs with bash on Linux and macOS (even macOS's old bash 3.2), and with Git
# Bash on Windows.

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
rm -rf "$out_dir"
mkdir -p "$out_dir"

# The page is built up in these variables: one tab button and one section
# per theme, one figure per sample.
tabs=""
sections=""
theme_id=""
count=0

# Starts a new theme (tab). Arguments: id, tab title, intro paragraph.
theme() {
  end_theme
  theme_id="$1"
  tabs+="
      <button role=\"tab\" data-theme=\"$1\">$2</button>"
  sections+="
    <section id=\"$1\" hidden>
      <p class=\"intro\">$3</p>
      <div class=\"grid\">"
}

# Closes the current theme's section, if one is open.
end_theme() {
  if [[ -n "$theme_id" ]]; then
    sections+="
      </div>
    </section>"
  fi
}

# Renders one picture into the current theme. Arguments: caption, then the
# turtle-art options to run with.
sample() {
  local caption="$1"
  shift
  count=$((count + 1))
  local file
  file="$(printf '%s-%02d.svg' "$theme_id" "$count")"
  "$program" "$@" --out "$out_dir/$file" > /dev/null
  echo "wrote gallery/$file"
  sections+="
        <figure>
          <a href=\"$file\"><img src=\"$file\" alt=\"$caption\" loading=\"lazy\"></a>
          <figcaption><strong>$caption</strong><br>
            <code>$*</code></figcaption>
        </figure>"
}

# --- The themes ------------------------------------------------------------
# Seeds come from a survey of seeds 1-100 per setting, ranked by how much of
# the grid they paint (see QUESTIONS.md).

theme highlights "Highlights" \
  "A few favorites from every theme. Wrapping edges (<code>--edge wrap</code>) keep the turtle going, which gives most of the large pictures."
sample "Default settings" --seed 228
sample "Wrap: diagonal highways" --seed 8 --edge wrap
sample "Wrap: long lines around a knot" --seed 30 --edge wrap
sample "Wrap: whole grid, 4 colors" --seed 20 --edge wrap
sample "8 colors" --seed 68 --edge wrap --colors 8
sample "6 colors" --seed 45 --edge wrap --colors 6
sample "3 colors, partly covered" --seed 87 --edge wrap --colors 3
sample "5-tile history" --seed 52 --edge wrap --history 5
sample "4-tile history, partly covered" --seed 19 --edge wrap --history 4
sample "Big grid (radius 100)" --seed 30 --edge wrap --radius 100 --steps 40000

theme edges "Edges: stop vs wrap" \
  "The same seed with <code>--edge stop</code> (the walk ends at the edge) and then <code>--edge wrap</code> (it comes back in on the opposite side). Most stopping walks end within a few hundred steps."
for seed in 228 137 8 30 20 68 46 3; do
  sample "Seed $seed, stop" --seed "$seed" --edge stop
  sample "Seed $seed, wrap" --seed "$seed" --edge wrap
done

theme sizes "Grid size" \
  "Radius 12, 25, 50 and 100 tiles, all wrapping. Steps grow with the grid (about 4 per tile), so each picture has time to develop."
sample "Radius 12" --seed 8 --edge wrap --radius 12 --steps 3000
sample "Radius 12" --seed 100 --edge wrap --radius 12 --steps 3000
sample "Radius 12" --seed 11 --edge wrap --radius 12 --steps 3000
sample "Radius 25" --seed 20 --edge wrap --radius 25 --steps 6000
sample "Radius 25" --seed 100 --edge wrap --radius 25 --steps 6000
sample "Radius 25" --seed 30 --edge wrap --radius 25 --steps 6000
sample "Radius 50" --seed 68 --edge wrap
sample "Radius 50" --seed 30 --edge wrap
sample "Radius 50" --seed 46 --edge wrap
sample "Radius 100" --seed 8 --edge wrap --radius 100 --steps 40000
sample "Radius 100" --seed 20 --edge wrap --radius 100 --steps 40000
sample "Radius 100" --seed 30 --edge wrap --radius 100 --steps 40000

theme colors "Colors" \
  "From 1 to 8 palette colors (<code>--colors</code>), all wrapping. More colors make a bigger rule table, (colors + 1)<sup>3</sup> entries, and busier pictures."
sample "1 color: only the path" --seed 8 --edge wrap --colors 1
sample "1 color: only the path" --seed 30 --edge wrap --colors 1
sample "2 colors" --seed 1 --edge wrap --colors 2
sample "2 colors" --seed 5 --edge wrap --colors 2
sample "2 colors" --seed 74 --edge wrap --colors 2
sample "3 colors" --seed 10 --edge wrap --colors 3
sample "3 colors" --seed 59 --edge wrap --colors 3
sample "3 colors" --seed 87 --edge wrap --colors 3
sample "4 colors (default)" --seed 20 --edge wrap
sample "4 colors (default)" --seed 30 --edge wrap
sample "6 colors" --seed 45 --edge wrap --colors 6
sample "6 colors" --seed 64 --edge wrap --colors 6
sample "6 colors" --seed 76 --edge wrap --colors 6
sample "8 colors" --seed 68 --edge wrap --colors 8
sample "8 colors" --seed 81 --edge wrap --colors 8
sample "8 colors" --seed 9 --edge wrap --colors 8

theme history "History" \
  "How many recent tiles, the current one included, the turtle looks at (<code>--history</code>), all wrapping. With 1 it only sees the tile it stands on; longer histories give bigger rule tables and more varied walks."
sample "History 1" --seed 2 --edge wrap --history 1
sample "History 1" --seed 7 --edge wrap --history 1
sample "History 1" --seed 24 --edge wrap --history 1
sample "History 2" --seed 2 --edge wrap --history 2
sample "History 2" --seed 23 --edge wrap --history 2
sample "History 2" --seed 47 --edge wrap --history 2
sample "History 3 (default)" --seed 8 --edge wrap
sample "History 3 (default)" --seed 30 --edge wrap
sample "History 4" --seed 71 --edge wrap --history 4
sample "History 4" --seed 52 --edge wrap --history 4
sample "History 4" --seed 19 --edge wrap --history 4
sample "History 5" --seed 52 --edge wrap --history 5
sample "History 5" --seed 42 --edge wrap --history 5
sample "History 5" --seed 91 --edge wrap --history 5

theme endings "Common endings" \
  "Most seeds are not this lucky. These are the usual outcomes, on small grids so they are visible."
sample "Walks off the edge" --seed 11 --radius 12
sample "Loops on a few tiles" --seed 6 --radius 12
sample "Wraps forever in a straight line: one color fills everything" --seed 12 --edge wrap --radius 12 --steps 3000
sample "Loops even when wrapping" --seed 2 --edge wrap --radius 12 --steps 3000

end_theme

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
      --border: #e4dfd3; --accent: #e63946;
    }
    @media (prefers-color-scheme: dark) {
      :root {
        --bg: #1b1d21; --card: #25282d; --text: #e8e6e1; --muted: #a0a0a0;
        --border: #3a3e45; --accent: #f4a261;
      }
    }
    body {
      margin: 0; padding: 24px 16px; background: var(--bg); color: var(--text);
      font: 15px/1.5 system-ui, sans-serif;
    }
    main { max-width: 1200px; margin: 0 auto; }
    h1 { margin: 0 0 4px; font-size: 24px; }
    p.lede { margin: 0 0 16px; color: var(--muted); }
    [role="tablist"] {
      display: flex; flex-wrap: wrap; gap: 4px; margin-bottom: 16px;
      border-bottom: 1px solid var(--border);
    }
    [role="tab"] {
      padding: 8px 14px; border: 0; border-bottom: 2px solid transparent;
      background: none; color: var(--muted); font: inherit; cursor: pointer;
    }
    [role="tab"][aria-selected="true"] {
      color: var(--text); border-bottom-color: var(--accent); font-weight: 600;
    }
    p.intro { margin: 0 0 16px; max-width: 75ch; }
    .grid {
      display: grid; gap: 16px;
      grid-template-columns: repeat(auto-fill, minmax(260px, 1fr));
    }
    figure {
      margin: 0; padding: 12px; background: var(--card);
      border: 1px solid var(--border); border-radius: 8px;
    }
    figure img { display: block; width: 100%; height: auto; }
    figcaption { margin-top: 8px; font-size: 14px; color: var(--muted); }
    figcaption strong { color: var(--text); font-weight: 600; }
    code { font-size: 12px; overflow-wrap: anywhere; }
  </style>
</head>
<body>
  <main>
    <h1>turtle-art gallery</h1>
    <p class="lede">Each picture is one turtle on a hex grid, following a rule
      table generated from its seed. $count pictures; click one to open it
      full size.</p>
    <nav role="tablist">$tabs
    </nav>$sections
  </main>
  <script>
    // Shows one theme at a time. The theme is kept in the URL's #fragment,
    // so a tab can be linked to and survives a reload. Pictures in hidden
    // tabs are not downloaded until their tab is shown (loading="lazy").
    const tabs = document.querySelectorAll('[role="tab"]');
    function show(id) {
      if (!document.getElementById(id)) id = tabs[0].dataset.theme;
      for (const tab of tabs) {
        const selected = tab.dataset.theme === id;
        tab.setAttribute('aria-selected', selected);
        document.getElementById(tab.dataset.theme).hidden = !selected;
      }
    }
    for (const tab of tabs) {
      tab.addEventListener('click', () => { location.hash = tab.dataset.theme; });
    }
    window.addEventListener('hashchange', () => show(location.hash.slice(1)));
    show(location.hash.slice(1));
  </script>
</body>
</html>
EOF
echo "wrote gallery/demo.html ($count pictures)"

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
