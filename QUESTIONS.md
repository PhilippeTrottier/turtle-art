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
