#include "grid.h"

// assert() checks a precondition in Debug builds and stops the program if it
// is false. It compiles to nothing in Release builds (when NDEBUG is set).
#include <cassert>

#include "color.h"
#include "hex.h"

namespace turtle_art {

// The `: radius_(radius)` member initializer list initializes members before
// the constructor body runs.
Grid::Grid(int radius) : radius_(radius) { assert(radius >= 0); }

bool Grid::Contains(Hex hex) const {
  return DistanceFromCenter(hex) <= radius_;
}

Color Grid::ColorAt(Hex hex) const {
  assert(Contains(hex));
  // find() returns end() when the key is missing, without inserting it, as
  // operator[] would. `auto` lets the compiler spell the iterator type.
  const auto it = colors_.find(hex);
  return it == colors_.end() ? Color::kUnpainted : it->second;
}

void Grid::Paint(Hex hex, Color color) {
  assert(Contains(hex));
  // operator[] inserts the key if missing, then assigns over its value.
  colors_[hex] = color;
}

}  // namespace turtle_art
