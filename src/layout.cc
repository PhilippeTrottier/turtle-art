#include "layout.h"

#include <numbers>
#include <vector>

#include "grid.h"
#include "tile.h"

namespace turtle_art {
namespace {

// Distance from a hexagon tile's center to each of its corners.
constexpr double kHexSize = 10.0;

// Width of a pointy-top hexagon, from its left side to its right side.
// std::numbers::sqrt3 (C++20) is the constant sqrt(3) as a double.
constexpr double kHexWidth = std::numbers::sqrt3 * kHexSize;

// Side of a square tile.
constexpr double kSquareSide = 10.0;

}  // namespace

Point TileCenter(const Grid& grid, Tile tile) {
  if (grid.shape() == GridShape::kRectangle) {
    return {.x = kSquareSide * tile.q, .y = kSquareSide * tile.r};
  }
  // Axial coordinates to pixels. See
  // https://www.redblobgames.com/grids/hexagons/#hex-to-pixel-axial
  return {.x = kHexWidth * (tile.q + (tile.r / 2.0)),
          .y = kHexSize * 1.5 * tile.r};
}

std::vector<Point> TileOutline(const Grid& grid) {
  if (grid.shape() == GridShape::kRectangle) {
    constexpr double kHalf = kSquareSide / 2;
    return {{.x = kHalf, .y = -kHalf},
            {.x = kHalf, .y = kHalf},
            {.x = -kHalf, .y = kHalf},
            {.x = -kHalf, .y = -kHalf}};
  }
  // Spelled out, rather than computed with sin/cos, so the corners are
  // exact and identical on every platform.
  return {{.x = kHexWidth / 2, .y = -kHexSize / 2},
          {.x = kHexWidth / 2, .y = kHexSize / 2},
          {.x = 0.0, .y = kHexSize},
          {.x = -kHexWidth / 2, .y = kHexSize / 2},
          {.x = -kHexWidth / 2, .y = -kHexSize / 2},
          {.x = 0.0, .y = -kHexSize}};
}

std::vector<Point> WrapCopyOffsets(const Grid& grid) {
  std::vector<Point> offsets;
  // The center tile is at {0, 0}, so a copy centered on a tile is offset by
  // that tile's position.
  for (const Tile center : grid.WrapCopyCenters()) {
    offsets.push_back(TileCenter(grid, center));
  }
  return offsets;
}

Box PictureBounds(const Grid& grid) {
  if (grid.shape() == GridShape::kRectangle) {
    // The top-left tile's center, minus half a tile.
    const Point top_left = TileCenter(grid, grid.Tiles().front());
    return {.x = top_left.x - (kSquareSide / 2),
            .y = top_left.y - (kSquareSide / 2),
            .width = kSquareSide * grid.width(),
            .height = kSquareSide * grid.height()};
  }
  // A hexagon is symmetric around the center: its outermost tile centers
  // are radius tiles away, plus the half tile beyond them.
  const double half_width = kHexWidth * (grid.radius() + 0.5);
  const double half_height = kHexSize * ((1.5 * grid.radius()) + 1.0);
  return {.x = -half_width,
          .y = -half_height,
          .width = 2 * half_width,
          .height = 2 * half_height};
}

}  // namespace turtle_art
