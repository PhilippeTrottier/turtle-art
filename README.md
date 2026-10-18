# turtle-art

Procedural art made by a turtle walking on a hexagonal grid.

> **Status:** v1 works end to end on Windows/MSVC. Open questions and
> assumptions from building it are in [QUESTIONS.md](QUESTIONS.md); the
> Linux/macOS presets have not been run yet.

## What it does

A turtle walks on a hexagon-shaped grid of hexagonal tiles, paints them, and
turns according to a rule. The painted grid is written out as an SVG image.

```
turtle-art --seed 42 --steps 10000 --radius 50 --out art.svg
```

### The walk

The turtle has a **position** (a tile), a **heading** (one of the 6 neighbour
directions) and a **current color**. Each step:

1. **Sense** — read the grid colors of the last 3 tiles visited: the tile it
   stands on and the two before it. Tiles never painted have the color
   *unpainted*. At the start, missing history counts as *unpainted*.
2. **Decide** — look up those 3 colors in the rule table, which gives exactly
   one action:
   - **keep** the heading,
   - **turn left** 60°,
   - **turn right** 60°,
   - **change color** to the next color in the palette (wrapping around).
3. **Paint** the current tile with the current color.
4. **Move** one tile forward along the heading.

The run ends after `--steps` steps, or earlier if the next move would leave
the grid.

Note: a turtle that only turns by 60° cannot come back to a tile in fewer than
6 moves, so the two previous tiles always still hold the colors the turtle
just painted. Only the tile it stands on can hold an older color. That one
tile is how the grid feeds back into the walk, as in
[Langton's ant](https://en.wikipedia.org/wiki/Langton%27s_ant).

### Colors

- A fixed palette of **4 colors**, plus *unpainted* (the background).
- The turtle starts with the first palette color; it never paints *unpainted*.
- *Change color* moves to the next palette color, cycling 1 → 2 → 3 → 4 → 1.

### The rule table

- The table maps every combination of 3 sensed colors to one action:
  5 × 5 × 5 = **125 entries**.
- It is generated from `--seed` with a seeded random number generator, so the
  same seed always produces the same table, and the same picture.

### The grid

- A **hexagon of radius R** (`--radius`): every tile within R steps of the
  center. The turtle starts at the center.
- Tiles are addressed with **axial hex coordinates**
  ([reference](https://www.redblobgames.com/grids/hexagons/)), which keep
  neighbour and distance math simple.

### Command-line options

| Option     | Meaning                                  | Default          |
|------------|------------------------------------------|------------------|
| `--seed`   | Seed for the rule table                  | `228`            |
| `--steps`  | Maximum number of steps                  | `10000`          |
| `--radius` | Grid radius in tiles                     | `50`             |
| `--out`    | Path of the SVG file to write            | `turtle-art.svg` |

`--help` lists them too. The default seed is one whose turtle covers a large
area; most seeds loop on a few dozen tiles (see QUESTIONS.md).

## Building and testing

You need:

- CMake 3.25 or newer.
- A C++20 compiler: MSVC (Visual Studio 2022 with *Desktop development with
  C++*) on Windows, GCC or Clang on Linux/macOS.
- [Ninja](https://ninja-build.org/), used by every preset.
- [vcpkg](https://learn.microsoft.com/vcpkg/get_started/get-started), with the
  `VCPKG_ROOT` environment variable pointing to it. CMake installs the
  packages listed in `vcpkg.json` on the first configure.

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

To render a few sample pictures and a page showing them, run
`scripts/make-gallery.sh` (bash; Git Bash on Windows) after building, then
open `gallery/demo.html`. Add `--serve` to also serve the gallery at
<http://localhost:8000/demo.html> (needs Python 3), for browsers that will
not open local files. `.claude/launch.json` runs the same thing for Claude
Code's built-in browser; on Windows it calls Git Bash by its full path,
because plain `bash` there may be WSL's, which cannot write Windows paths.

### Formatting and linting

- **clang-tidy** runs automatically on every file as it compiles, using the
  checks in `.clang-tidy`. Any finding fails the build. CMake finds
  clang-tidy on the `PATH`, or next to MSVC (Visual Studio ships it in
  `VC\Tools\Llvm\x64\bin`). To build without it, configure with
  `-DTURTLE_ART_CLANG_TIDY=OFF`.
- **clang-format** applies the Google style from `.clang-format`. Run it on
  the files you changed before committing:

  ```bash
  clang-format -i src/*.h src/*.cc tests/*.cc
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
| `scripts/`           | `make-gallery.sh`: renders samples and `gallery/demo.html` |
| `QUESTIONS.md`       | Open questions and assumptions awaiting review   |

### Code map

Read `src/` in this order; each file uses only the ones above it. Every
`foo.cc` has its tests in `tests/foo_test.cc`.

| File              | What it holds                                              |
|-------------------|------------------------------------------------------------|
| `direction.h`     | `Direction`: the 6 hex directions, `TurnLeft`, `TurnRight` |
| `hex.h`           | `Hex`: axial tile coordinates, `Neighbor`, `DistanceFromCenter` |
| `color.h`         | `Color`: unpainted plus 4 paints, `NextPaint`              |
| `grid.h`          | `Grid`: hexagon of tiles of radius R, `Contains`, `ColorAt`, `Paint` |
| `action.h`        | `Action`: keep, turn left, turn right, change color        |
| `rule_table.h`    | `SensedColors` and `RuleTable`: 3 colors → action          |
| `random_rules.h`  | `RandomRuleTable(seed)`: same table on every platform      |
| `turtle.h`        | `Turtle::Step` (sense, act, paint, move) and `Walk`        |
| `svg.h`           | `ToSvg`: a grid as an SVG string                           |
| `art.h`           | `ArtSettings` and `MakeArt`: rules + walk + SVG            |
| `main.cc`         | Command-line options, writes the SVG file                  |

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
| Style            | [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) |
| Format / lint    | clang-format and clang-tidy, configs committed                |
| License          | MIT                                                           |

Style notes that follow from the Google guide:

- No exceptions in our code. Errors are returned as values
  (`std::optional`, or a small result type).
- One exception to that, kept to one place: CLI11 reports parse errors by
  throwing, so `main` catches them and turns them into an exit code.
- Files are `snake_case.h` / `snake_case.cc`.

## Out of scope for v1

- High-performance optimisation (SIMD/SSE, multithreading, cache tuning, …).
- Live display or animation; v1 only writes SVG files.
- User-written rule tables; v1 rules come from a seed.
