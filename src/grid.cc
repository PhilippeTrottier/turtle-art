#include "grid.h"

// assert() checks a precondition in Debug builds and stops the program if it
// is false. It compiles to nothing in Release builds (when NDEBUG is set).
#include <cassert>
#include <optional>

#include "color.h"
#include "direction.h"
#include "hex.h"

namespace turtle_art {

// The `: radius_(radius)` member initializer list initializes members before
// the constructor body runs.
Grid::Grid(int radius, EdgeMode edge_mode)
    : radius_(radius), edge_mode_(edge_mode) {
  assert(radius >= 0);
}

bool Grid::Contains(Hex hex) const {
  return DistanceFromCenter(hex) <= radius_;
}

std::optional<Hex> Grid::NeighborOf(Hex hex, Direction direction) const {
  assert(Contains(hex));
  const Hex next = Neighbor(hex, direction);
  if (Contains(next)) {
    return next;
  }
  if (edge_mode_ == EdgeMode::kWrap) {
    return Wrap(next);
  }
  return std::nullopt;
}

Hex Grid::Wrap(Hex hex) const {
  // Copies of the grid tile the whole plane when centered on "mirror
  // centers". The nearest six are rotations of {2R + 1, -R}. A tile just
  // outside the grid lies inside one of those copies; subtracting that
  // copy's center gives the matching tile of the grid itself. See
  // https://www.redblobgames.com/grids/hexagons/#wraparound
  Hex mirror = {.q = (2 * radius_) + 1, .r = -radius_};
  for (int i = 0; i < 6; ++i) {
    const Hex candidate = {.q = hex.q - mirror.q, .r = hex.r - mirror.r};
    if (Contains(candidate)) {
      return candidate;
    }
    // Rotates `mirror` 60 degrees around the center.
    mirror = {.q = mirror.q + mirror.r, .r = -mirror.q};
  }
  assert(false && "Wrap() needs a tile just outside the grid");
  return hex;
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
