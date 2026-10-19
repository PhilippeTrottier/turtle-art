#ifndef TURTLE_ART_SRC_TILE_H_
#define TURTLE_ART_SRC_TILE_H_

#include <compare>  // Needed by operator<=>.
#include <ostream>

namespace turtle_art {

// A tile of a grid, with the center tile at {0, 0}. What q and r mean
// depends on the grid's shape (see Grid):
// - hexagon: axial coordinates; q grows to the east, r to the south-east.
//   See https://www.redblobgames.com/grids/hexagons/#coordinates-axial
// - rectangle (square tiles): q is the column (growing east), r the row
//   (growing south).
//
// A plain struct (public data, no invariants) is the Google-style choice for
// a passive value like this.
struct Tile {
  int q = 0;
  int r = 0;

  // The "spaceship" operator <=> (C++20), defaulted, makes the compiler
  // compare members in order (q, then r) and derive ==, !=, <, <=, > and >=.
  // Ordering lets Tile be a key of std::map.
  friend auto operator<=>(const Tile&, const Tile&) = default;
};

// Returns how many steps separate `tile`, in axial hex coordinates, from the
// center tile {0, 0} of a hexagon grid.
int HexDistanceFromCenter(Tile tile);

// Writes `tile` as "{q, r}". GoogleTest uses this to print values in failed
// assertions.
std::ostream& operator<<(std::ostream& out, Tile tile);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_TILE_H_
