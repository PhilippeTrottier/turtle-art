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
