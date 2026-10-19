# turtle-art

Procedural art made by a turtle walking on a hexagonal grid.

> **Status:** the command-line program and the web explorer work on
> Windows/MSVC. The Linux/macOS presets have not been run yet. Open
> questions and assumptions are in [QUESTIONS.md](QUESTIONS.md).

## What it does

A turtle walks on a hexagon-shaped grid of hexagonal tiles, paints them, and
turns according to a rule. The painted grid is written out as an SVG image,
or browsed by the thousand in the [web explorer](#web-explorer).

```
turtle-art --seed 42 --steps 10000 --radius 50 --out art.svg
```

### The walk

The turtle has a **position** (a tile), a **heading** (one of the 6 neighbor
directions) and a **current color**. Each step:

1. **Sense** — read the grid colors of the last H tiles visited (`--history`,
   default 3): the tile it stands on and the H − 1 before it. Tiles never
   painted have the color *unpainted*. At the start, missing history counts
   as *unpainted*.
2. **Decide** — look up those H colors in the rule table, which gives exactly
   one action:
   - **keep** the heading,
   - **turn left** 60°,
   - **turn right** 60°,
   - **change color** to the next color in the palette (wrapping around).
3. **Paint** the current tile with the current color.
4. **Move** one tile forward along the heading.

The run ends after `--steps` steps. With `--edge wrap` (the default) a move
off the grid comes back in on the opposite side of the hexagon; with
`--edge stop` the run ends early instead, when the next move would leave the
grid.

Note: a turtle that only turns by 60° cannot come back to a tile in fewer than
6 moves (except on small wrapping grids, where wrapping around can bring it
back sooner). So the previous tiles
it senses (at most 5) still hold the colors it just painted, and only the
tile it stands on can hold an older color. That one tile is how the grid
feeds back into the walk, as in
[Langton's ant](https://en.wikipedia.org/wiki/Langton%27s_ant).

### Colors

- A palette of **N colors** (`--colors`, 1 to 8, default 4), plus
  *unpainted* (the background).
- The turtle starts with the first palette color; it never paints *unpainted*.
- *Change color* moves to the next palette color, cycling 1 → 2 → … → N → 1.

### The rule table

- The table maps every combination of H sensed colors to one action:
  (N + 1)^H entries, **125** with the defaults (5 × 5 × 5).
- It is generated from `--seed` with a seeded random number generator, so the
  same seed always produces the same table, and the same picture.

### The grid

- A **hexagon of radius R** (`--radius`): every tile within R steps of the
  center. The turtle starts at the center, heading east.
- Its edge either stops the walk or wraps around (`--edge`). Wrapping uses
  the hexagon's "mirror centers", so leaving through one side re-enters
  through the opposite one
  ([reference](https://www.redblobgames.com/grids/hexagons/#wraparound)).
- Tiles are addressed with **axial hex coordinates**
  ([reference](https://www.redblobgames.com/grids/hexagons/)), which keep
  neighbor and distance math simple.

### Command-line options

| Option     | Meaning                                  | Default          |
|------------|------------------------------------------|------------------|
| `--seed`   | Seed for the rule table                  | `30`             |
| `--steps`  | Maximum number of steps                  | `10000`          |
| `--radius` | Grid radius in tiles                     | `50`             |
| `--edge`   | At the edge: `wrap` around to the opposite side, or `stop` the walk | `wrap` |
| `--colors` | Number of palette colors, 1 to 8         | `4`              |
| `--history`| Number of recent tiles (current one included) whose colors decide each move, 1 to 6 | `3` |
| `--out`    | Path of the SVG file to write            | `turtle-art.svg` |

`--help` lists them too. Wrapping is the default edge because with
`--edge stop` most turtles walk off the grid within a few dozen steps. Even
so, most seeds give dull pictures (a few looping tiles, or one flat color);
the default seed is one of the good ones, and the
[web explorer](#web-explorer) finds others quickly.

## Building and testing

You need:

- CMake 3.25 or newer.
- A C++20 compiler with `std::format`: MSVC from Visual Studio 2022 (with
  *Desktop development with C++*) on Windows, GCC 13+ or Clang 17+ on
  Linux/macOS.
- [Ninja](https://ninja-build.org/), used by every preset.
- [vcpkg](https://learn.microsoft.com/vcpkg/get_started/get-started), with the
  `VCPKG_ROOT` environment variable pointing to it. CMake installs the
  packages listed in `vcpkg.json` on the first configure.
- Optional: bash (Git Bash on Windows) and Python 3 for the scripts in
  `scripts/`, and the Emscripten SDK for the [web explorer](#web-explorer).

Pick the preset for your platform (`msvc`, `gcc` or `clang`) and run the
commands below. On Windows, run them from a **Developer PowerShell for VS
2022** (Start menu), which puts `cl.exe` on the `PATH`; Ninja, unlike the
Visual Studio generator, does not look for the compiler itself.

```bash
cmake --preset msvc
```

```bash
cmake --build --preset msvc
```

```bash
ctest --preset msvc
```

Build output goes to `build/<preset>/`, which git ignores. The program is
`build/<preset>/src/turtle-art` (`.exe` on Windows).

### Web explorer

A page for browsing thousands of seeds quickly: the C++ core compiled to
WebAssembly with [Emscripten](https://emscripten.org/), plus a plain
HTML/JavaScript front end (`web/`). It needs no server-side code; any static
web server works.

One-time setup: install the Emscripten SDK, e.g. into `~/emsdk`, and point
`EMSDK` at it (the script also looks in `~/emsdk` by itself). Emscripten is a
compiler toolchain, like MSVC or GCC, so vcpkg does not install it; vcpkg
expects it to be installed already, and uses it to build the web build's
packages (GoogleTest) through its `wasm32-emscripten` triplet. Use the version the explorer is tested
with, **6.0.12**, so everyone builds the same thing (configuring warns if
another version is found):

```bash
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
```

```bash
~/emsdk/emsdk install 6.0.12 && ~/emsdk/emsdk activate 6.0.12
```

(On Windows, run `emsdk.bat` with the same arguments.) Then build and serve
it at <http://localhost:8001/> (needs Python 3):

```bash
scripts/explorer.sh
```

Or use the `wasm` preset like the native ones: `cmake --preset wasm`, then
`cmake --build --preset wasm`; the site is `build/wasm/web/`. The same build
compiles the unit tests to WebAssembly, and `ctest --preset wasm` runs them
with Node.js (installed by the Emscripten SDK), so the library is tested on
the platform the explorer runs on.

Favorites are saved per web address, port included, so the explorer always
uses port 8001: on another port, earlier favorites would not show. Use
**Copy as commands** to keep them anywhere else.

`.claude/launch.json` lets Claude Code's built-in browser start the
explorer. On Windows it calls Git Bash by its full path, because plain
`bash` there may be WSL's, which cannot write Windows paths.

Using it:

- The header holds the knobs, a start seed (🎲 or `r` for a random one), a
  **min interest** filter and the thumbnail size. Settings live in the URL,
  so a view can be bookmarked or shared.
- Thumbnails appear seed after seed as you scroll. Each shows its seed and
  interest score (the color `Entropy`); pictures below the filter are
  skipped, which hides the many flat fills and tiny loops.
- Click a picture (or press `Enter` for the first one) to view it large.
  In the viewer: `←` `→` browse, `f` favorite, `d` download the SVG, `c`
  copy its `turtle-art` command, `Esc` close.
- ☆ / `f` saves a favorite in this browser (localStorage). `g` or
  **★ Favorites** shows them; **Copy as commands** copies them all as
  `turtle-art` command lines.

The page itself (`index.html`, `app.js`) is not unit tested; keep it small. Everything
that decides what a picture looks like stays in C++.

### Formatting and linting

- **Compiler warnings are errors** (`/W4 /WX` with MSVC, `-Wall -Wextra
  -Wpedantic -Werror` elsewhere), so a warning fails the build.
- **clang-tidy** runs automatically on every file as it compiles, using the
  checks in `.clang-tidy`. Any finding fails the build. CMake finds
  clang-tidy on the `PATH`, or next to MSVC (Visual Studio ships it in
  `VC\Tools\Llvm\x64\bin`). To build without it, configure with
  `-DTURTLE_ART_CLANG_TIDY=OFF`. The `wasm` preset does that, so
  `web/bindings.cc` is formatted but not linted. Editing `.clang-tidy` does
  not trigger a rebuild; after changing it, build with `--clean-first`.
- **clang-format** applies the Google style from `.clang-format`. Run it on
  the files you changed before committing:

  ```bash
  clang-format -i src/*.h src/*.cc tests/*.cc web/*.cc
  ```

  On Windows it is not on the `PATH` by default; use the copy at
  `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe`
  (adjust the edition if yours is not Community).

### Repository layout

| Path                 | Contents                                         |
|----------------------|--------------------------------------------------|
| `src/`               | The program's logic, built as `turtle_art_lib`, plus `main.cc` for the `turtle-art` program |
| `tests/`             | GoogleTest unit tests, one `*_test.cc` per file  |
| `CMakeLists.txt`     | Top-level build, shared compiler settings        |
| `CMakePresets.json`  | Configure/build/test presets per compiler        |
| `vcpkg.json`         | Package dependencies, pinned by `builtin-baseline` |
| `scripts/`           | `explorer.sh`: builds and serves the web explorer |
| `.claude/launch.json`| Lets Claude Code's built-in browser start the explorer |
| `web/`               | The web explorer: `bindings.cc` (C++ → JavaScript via Embind), `index.html` and `app.js` |
| `QUESTIONS.md`       | Open questions and assumptions awaiting review   |

### Code map

Read `src/` in this order; each file uses only the ones above it. Every
`foo.cc` has its tests in `tests/foo_test.cc`.

| File              | What it holds                                              |
|-------------------|------------------------------------------------------------|
| `direction.h`     | `Direction`: the 6 hex directions, `TurnLeft`, `TurnRight` |
| `hex.h`           | `Hex`: axial tile coordinates, `Neighbor`, `DistanceFromCenter` |
| `color.h`         | `Color`: unpainted plus up to 8 paints, `NextPaint`        |
| `palette.h`       | `kPalette`: the CSS color of each `Color`                  |
| `grid.h`          | `EdgeMode` and `Grid`: hexagon of radius R, `Tiles`, `NeighborOf` (stop or wrap), `ColorAt`, `Paint` |
| `grid_stats.h`    | `ColorCounts` and `Entropy`: how interesting a grid is     |
| `action.h`        | `Action`: keep, turn left, turn right, change color        |
| `rule_table.h`    | `RuleTable`: sensed colors (any history length) → action   |
| `random_rules.h`  | `RandomRuleTable(seed, …)`: same table on every platform   |
| `turtle.h`        | `Turtle::Step` (sense, act, paint, move) and `Walk`        |
| `svg.h`           | `ToSvg`: a grid as an SVG string                           |
| `art.h`           | `ArtSettings`, `RunArt` (rules + walk) and `MakeArt` (+ SVG) |
| `main.cc`         | Command-line options, writes the SVG file                  |

`web/bindings.cc` is the web explorer's only C++: it exposes `RunArt`,
`MakeArt` and the palette to JavaScript.

## Why this repository exists

This project has two goals, and they are equally important:

1. **Make the art.** A working, fun generator of hex-grid turtle pictures.
2. **Teach C++ by example.** The codebase is meant to be read by people who
   are rusty with C++ and want to refresh how the language is used well
   today. It should show modern, idiomatic C++ in a project small enough to
   read in one sitting.

## How we work

How we build it matters as much as what we build.

- **Small commits.** Each commit does one thing and leaves the project
  building and its tests passing.
- **Always graspable.** At any commit, a newcomer can read the repository and
  understand it well enough to maintain it and add features quickly. No
  decision, desired outcome or constraint lives only in someone's head: it is
  written down in this README, or next to the code it concerns.
- **Modern, idiomatic C++**, built with mature, widely used tools (below).
- **Cross-platform.** The project builds and runs on Windows, Linux and macOS.
- **A learning tool.**
  - Every public function, class and method is documented.
  - Newer language features (C++17 and later) and any non-trivial code get a
    short comment explaining what the mechanism is and why it is used here.
  - Comments explain *why*, not *what*, except where the *what* is the lesson.

## Non-functional requirements

These are key; they take priority over cleverness and over features.

- **Readable, maintainable code**, guided by:
  - **KISS** — the simplest thing that works.
  - **YAGNI** — no code for a need we do not have yet.
- **Easy to test.** Dependencies that would make testing hard (randomness,
  file output, …) are injected rather than reached for globally. For example,
  the walk takes a rule table as input instead of creating one itself.
- **Trivial tests.** Test code is itself untested, so each test is
  straight-line: arrange, act, assert. No loops, branches or helpers with
  logic in them.
- **Real dependencies over mocks.** When a dependency is deterministic and
  fast, tests use the real thing and check inputs against outputs. Mocks and
  fakes are reserved for what is slow, non-deterministic or external.

## Tools and conventions

| Concern          | Choice                                                        |
|------------------|---------------------------------------------------------------|
| Language         | C++20                                                         |
| Build            | CMake, with `CMakePresets.json`                               |
| Packages         | vcpkg, manifest mode (`vcpkg.json`)                           |
| Unit tests       | GoogleTest                                                    |
| Command line     | CLI11                                                         |
| Web explorer     | Emscripten 6.0.12 + Embind; plain HTML/JavaScript, no framework |
| Scripts          | bash, plus Python 3's `http.server` to serve pages            |
| Style            | [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) |
| Format / lint    | clang-format and clang-tidy, configs committed                |
| License          | MIT                                                           |

Style notes that follow from the Google guide:

- No exceptions in our code. Errors are returned as values
  (`std::optional`, or a small result type).
- One exception to that, kept to one place: CLI11 reports parse errors by
  throwing, so `main` catches them and turns them into an exit code.
- Preconditions (a tile must be on the grid, a count in range) are checked
  with `assert`, which stops Debug builds and costs nothing in Release.
  Containers are indexed with `[]`, not `.at()`, which reports a bad index
  by throwing; indices are in range by construction or asserted.
- Running out of memory ends the program. Where clang-tidy cannot see that
  (`main`, and `Grid`'s move constructor with MSVC's `std::map`), a
  `NOLINT(bugprone-exception-escape)` says so, with a comment explaining why.
- Files are `snake_case.h` / `snake_case.cc`.

## Out of scope (for now)

- High-performance optimization (SIMD/SSE, multithreading, cache tuning, …).
- Live animation of the walk (the explorer shows finished pictures).
- User-written rule tables; rules come from a seed.
- Favorites shared across browsers or people (they live in one browser).
