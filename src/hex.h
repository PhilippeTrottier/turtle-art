#ifndef TURTLE_ART_SRC_HEX_H_
#define TURTLE_ART_SRC_HEX_H_

#include <compare>  // Needed by operator<=>.
#include <ostream>

#include "direction.h"

namespace turtle_art {

// A tile of the hex grid, in axial coordinates: `q` grows to the east, `r`
// grows to the south-east, and the center tile is {0, 0}. See
// https://www.redblobgames.com/grids/hexagons/#coordinates-axial
//
// A plain struct (public data, no invariants) is the Google-style choice for
// a passive value like this.
struct Hex {
  int q = 0;
  int r = 0;

  // The "spaceship" operator <=> (C++20), defaulted, makes the compiler
  // compare members in order (q, then r) and derive ==, !=, <, <=, > and >=.
  // Ordering lets Hex be a key of std::map.
  friend auto operator<=>(const Hex&, const Hex&) = default;
};

// Returns the tile next to `hex` in `direction`.
Hex Neighbor(Hex hex, Direction direction);

// Returns how many steps separate `hex` from the center tile {0, 0}.
int DistanceFromCenter(Hex hex);

// Writes `hex` as "{q, r}". GoogleTest uses this to print values in failed
// assertions.
std::ostream& operator<<(std::ostream& out, Hex hex);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_HEX_H_
