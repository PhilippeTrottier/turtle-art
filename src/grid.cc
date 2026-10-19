#include "grid.h"

#include <array>
// assert() checks a precondition in Debug builds and stops the program if it
// is false. It compiles to nothing in Release builds (when NDEBUG is set).
#include <cassert>
#include <cstddef>
#include <optional>
#include <vector>

#include "color.h"
#include "tile.h"

namespace turtle_art {
namespace {

// The step from a tile to its neighbor in each direction, indexed by
// heading (see grid.h).
//
// std::array is a fixed-size array that knows its size and can be copied,
// unlike a built-in `Tile[6]`.
constexpr std::array<Tile, 6> kHexOffsets = {{
    {.q = 1, .r = 0},   // 0: east
    {.q = 1, .r = -1},  // 1: north-east
    {.q = 0, .r = -1},  // 2: north-west
    {.q = -1, .r = 0},  // 3: west
    {.q = -1, .r = 1},  // 4: south-west
    {.q = 0, .r = 1},   // 5: south-east
}};

// The same for square tiles. Rows grow downward (south), like screen
// coordinates.
constexpr std::array<Tile, 4> kSquareOffsets = {{
    {.q = 1, .r = 0},   // 0: east
    {.q = 0, .r = -1},  // 1: north
    {.q = -1, .r = 0},  // 2: west
    {.q = 0, .r = 1},   // 3: south
}};

// Returns `value` modulo `divisor` (which must be positive), in
// [0, divisor). Unlike C++'s %, which gives -1 for -1 % 4, this gives 3.
int PositiveModulo(int value, int divisor) {
  return ((value % divisor) + divisor) % divisor;
}

}  // namespace

Grid::Grid(GridShape shape, int radius, int width, int height,
           EdgeMode edge_mode)
    : shape_(shape),
      radius_(radius),
      width_(width),
      height_(height),
      edge_mode_(edge_mode) {}

Grid Grid::Hexagon(int radius, EdgeMode edge_mode) {
  assert(radius >= 0);
  return {GridShape::kHexagon, radius, 0, 0, edge_mode};
}

Grid Grid::Rectangle(int width, int height, EdgeMode edge_mode) {
  assert(width >= 1 && height >= 1);
  return {GridShape::kRectangle, 0, width, height, edge_mode};
}

int Grid::direction_count() const {
  return shape_ == GridShape::kHexagon
             ? static_cast<int>(kHexOffsets.size())
             : static_cast<int>(kSquareOffsets.size());
}

bool Grid::Contains(Tile tile) const {
  if (shape_ == GridShape::kHexagon) {
    return HexDistanceFromCenter(tile) <= radius_;
  }
  return tile.q >= min_q() && tile.q < min_q() + width_ && tile.r >= min_r() &&
         tile.r < min_r() + height_;
}

std::vector<Tile> Grid::Tiles() const {
  // Visit every (q, r) in the rectangle that encloses the grid (for a
  // rectangle, the grid itself), and keep those that are tiles of the grid.
  const bool hexagon = shape_ == GridShape::kHexagon;
  const int q_begin = hexagon ? -radius_ : min_q();
  const int q_end = hexagon ? radius_ + 1 : min_q() + width_;
  const int r_begin = hexagon ? -radius_ : min_r();
  const int r_end = hexagon ? radius_ + 1 : min_r() + height_;
  std::vector<Tile> tiles;
  for (int r = r_begin; r < r_end; ++r) {
    for (int q = q_begin; q < q_end; ++q) {
      const Tile tile = {.q = q, .r = r};
      if (Contains(tile)) {
        tiles.push_back(tile);
      }
    }
  }
  return tiles;
}

std::optional<Tile> Grid::NeighborOf(Tile tile, int heading) const {
  assert(Contains(tile));
  assert(heading >= 0 && heading < direction_count());
  const auto index = static_cast<std::size_t>(heading);
  const Tile offset = shape_ == GridShape::kHexagon ? kHexOffsets[index]
                                                    : kSquareOffsets[index];
  // `.q = ...` are designated initializers (C++20): they name each member.
  const Tile next = {.q = tile.q + offset.q, .r = tile.r + offset.r};
  if (Contains(next)) {
    return next;
  }
  if (edge_mode_ == EdgeMode::kWrap) {
    return Wrap(next);
  }
  return std::nullopt;
}

Tile Grid::Wrap(Tile tile) const {
  if (shape_ == GridShape::kRectangle) {
    // A rectangle wraps like a torus: columns and rows modulo the size.
    return {.q = min_q() + PositiveModulo(tile.q - min_q(), width_),
            .r = min_r() + PositiveModulo(tile.r - min_r(), height_)};
  }
  // Copies of the hexagon tile the whole plane when centered on "mirror
  // centers". The nearest six are rotations of {2R + 1, -R}. A tile just
  // outside the grid lies inside one of those copies; subtracting that
  // copy's center gives the matching tile of the grid itself. See
  // https://www.redblobgames.com/grids/hexagons/#wraparound
  Tile mirror = {.q = (2 * radius_) + 1, .r = -radius_};
  for (int i = 0; i < 6; ++i) {
    const Tile candidate = {.q = tile.q - mirror.q, .r = tile.r - mirror.r};
    if (Contains(candidate)) {
      return candidate;
    }
    // Rotates `mirror` 60 degrees around the center.
    mirror = {.q = mirror.q + mirror.r, .r = -mirror.q};
  }
  assert(false && "Wrap() needs a tile just outside the grid");
  return tile;
}

Color Grid::ColorAt(Tile tile) const {
  assert(Contains(tile));
  // find() returns end() when the key is missing, without inserting it, as
  // operator[] would. `auto` lets the compiler spell the iterator type.
  const auto it = colors_.find(tile);
  return it == colors_.end() ? Color::kUnpainted : it->second;
}

void Grid::Paint(Tile tile, Color color) {
  assert(Contains(tile));
  // operator[] inserts the key if missing, then assigns over its value.
  colors_[tile] = color;
}

}  // namespace turtle_art
