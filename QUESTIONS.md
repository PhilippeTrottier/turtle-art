# Questions and assumptions to review

Decisions taken while building v1 without stopping to ask. Each entry is
either a **question** (needs an answer) or an **assumption** (already built
this way; confirm or ask for a change). Once reviewed, answers move into the
README or the code, and the entry is deleted.

## Naming and style

1. **Assumption:** code uses American spelling (`Neighbor`, `Color`), as the
   Google style guide's own examples do; the README uses British spelling
   ("neighbour", "colour" nowhere yet). Should the README switch too?

## Tooling

2. **Assumption (bug found and fixed):** `.clang-tidy`'s header filter only
   matched `/` separators, so on Windows findings in our own headers were
   silently dropped. It now matches `/` and `\`. Fixing it surfaced three
   findings, which I resolved by changing the config, not the code:
   - disabled `performance-enum-size` (wants `std::uint8_t` enums; v1 does
     not optimise),
   - disabled `modernize-use-nodiscard` (wants `[[nodiscard]]` on every const
     getter). `[[nodiscard]]` is a good C++17 lesson, though; would you
     rather have it everywhere?
   - allowed lower_case method names (Google-style accessors like
     `radius()`). Side effect: any lower_case method passes the naming check.
3. **Note:** editing `.clang-tidy` does not trigger a rebuild. Use
   `cmake --build --preset msvc --clean-first` after changing it.

## Rules

4. **Assumption:** the sensed colors are ordered oldest first
   (`two_back`, `one_back`, `current`), and the seeded table fills
   combinations in that order. Changing the order changes every picture
   for a given seed.
5. **Assumption:** each action is equally likely (1 in 4) in a random
   table. Weighting (e.g. fewer color changes) is a possible v2 knob.
6. **Assumption:** random numbers come from `std::mt19937` with our own
   `% 4` mapping, not `std::uniform_int_distribution`, because only the
   former is guaranteed identical across standard libraries. A test pins
   seed 1's first entry so a platform difference would show up.

## The walk

7. **Assumption:** the turtle starts on the center tile, heading east,
   with `kPaint1`. A random or seeded start heading would be easy to add.
8. **Assumption:** a *change color* step changes color first, then paints,
   so the tile it stands on gets the new color (README order: sense,
   decide, paint, move).
9. **Assumption:** at the edge, the final step still paints its tile and
   counts as a step; the run then ends. `Walk` returns steps taken.
10. **Assumption:** `Grid` and `RuleTable` are passed to `Turtle::Step`
    instead of stored in the turtle, so tests can set them up freely.

## SVG output

11. **Assumption — palette:** unpainted `#f5f0e6` (off-white), then red
    `#e63946`, orange `#f4a261`, teal `#2a9d8f`, dark blue `#264653`. Pick
    your own, or make the palette an option later?
12. **Assumption:** unpainted tiles are drawn too (as the background), so
    the picture always shows the full hexagonal grid. The alternative is
    drawing painted tiles only, on a transparent background.
13. **Assumption:** tiles are 10 SVG units from center to corner, with
    coordinates printed to 2 decimals. Corner offsets are written out
    instead of computed with sin/cos, so output is identical everywhere.
14. **Question:** `std::format` needs GCC 13+, Clang 17+ (libc++) or MSVC
    19.29+. OK as the minimum compilers? Only MSVC is verified so far.

## The art itself (most important)

15. **Question — most seeds make tiny pictures.** Across seeds 1–300 at the
    defaults (radius 50, 10,000 steps), the number of tiles painted was:
    under 50 for 113 seeds, 50–499 for 183, 500–1999 for 3, and 2000+ for
    1 (seed 228: 2,563 tiles, all 4 colors). Checking where each run
    ended (after I could view the pictures) shows two causes:
    - **167 seeds hit the edge** and stop, most after walking almost
      straight out (~50 steps). This comes from choosing "stop at edge";
      a wrapping grid (torus) or bouncing off the edge would keep them
      going.
    - **133 seeds loop** forever on a few tiles. This comes from the rule
      itself: the two previous tiles always hold the turtle's own recent
      colors, so only the current tile feeds the grid back into the walk,
      and a loop that repaints with the same color never changes.

    Ideas for v2, none built:
    - wrap or bounce at the edge instead of stopping;
    - paint the tile with the *next* color after its current one
      (Langton's-ant style), so revisits always change the grid;
    - sense more of the grid (e.g. the tile ahead) instead of history;
    - weight actions (fewer turns, more keep);
    - report steps taken and tiles covered, or search seeds automatically.
16. **Assumption:** because of 15, the default seed is 228 instead of 1, so
    running with no options gives an interesting picture. Seen in the
    gallery: it looks right, and covers roughly the middle third of the
    grid in all four colors before reaching the edge.
17. **Assumption:** defaults are `--steps 10000 --radius 50 --out
    turtle-art.svg`. A radius-50 SVG is about 1.2 MB, since every tile is
    drawn.

## Command line

18. **Assumption:** bad options print CLI11's message and exit with CLI11's
    code (e.g. 105 for an out-of-range value); a file that cannot be
    written exits with 1. `main` silences clang-tidy's
    `bugprone-exception-escape` with a commented `NOLINT`.
19. **Note:** clang-tidy ignores MSVC's `/EHsc`, so CMake passes it again
    (`--extra-arg=/EHsc`); without that, CLI11's try/catch fails the lint.

## Knobs added after v1 (edge, colors, history)

20. **Assumption:** `--edge wrap` uses the standard hexagon wraparound
    (mirror centers), so leaving one side re-enters the *opposite* side,
    shifted: e.g. on radius 1, east of {1, 0} is {-1, 1}, not {-1, 0}. It
    is the only wrap where every tile keeps exactly 6 neighbors. The default
    stays `stop`, your original choice, though `wrap` makes far more seeds
    interesting. Switch the default?
21. **Finding:** with wrap, a turtle that never turns covers *every* tile
    (a straight line on this torus visits the whole hexagon), so many seeds
    "fill" the grid in one color. The gallery survey therefore ranks seeds
    by colors used as well as area.
22. **Assumption:** `--colors` is 1-8 and `--history` 1-6. The rule table
    has (colors + 1)^history entries, at most 9^6 = 531,441. Limits are
    `kMaxPaintCount` and `kMaxHistoryLength`.
23. **Assumption:** the 4 extra palette colors are sage `#8ab17d`, purple
    `#9b5de5`, sky blue `#00b4d8` and yellow `#ffd166`.
24. **Assumption:** the random table is filled in index order (sensed colors
    as base-(colors+1) digits, oldest first), so default settings still give
    exactly the same pictures as v1 (checked on seed 228).
25. **Cosmetic:** `--help` shows `--edge` as
    `ENUM:value in {stop->0,wrap->1} OR {0,1}`, CLI11's default rendering
    of a CheckedTransformer; it also accepts 0 and 1. Fine, or tidy it up?
26. **Change:** SVGs now define the hexagon once and place tiles with
    `<use>` and CSS classes; radius-50 files went from 1.24 MB to 411 KB.

## Gallery

27. **Assumption:** gallery seeds were chosen by a survey of seeds 1-100 per
    setting, ranked by color entropy (how evenly tiles spread over the
    colors, unpainted included), after a first ranking by area picked too
    many flat one-color fills. The survey scripts are throwaway (not in the
    repo); say if you want a `scripts/survey` tool committed.
28. **Assumption:** the gallery regenerates from scratch each run (it deletes
    `gallery/`, which only holds generated files) and uses the first build
    it finds under `build/`. 72 pictures, about 30 MB, about 10 s with a
    Release build.
29. **Question:** no Release preset exists; I made a throwaway
    `build/release-scratch` for speed. Add `msvc-release` / `gcc-release`
    presets?
30. **Finding:** more colors rarely means more colorful. With 6-8 colors
    most seeds still paint a field of one or two colors, and in seeds 1-100
    only seed 9 (8 colors) spreads tiles evenly. Longer histories (5-6)
    gave the most balanced pictures of any setting.
