#include "svg.h"

#include <array>
#include <cstddef>
#include <format>
#include <numbers>
#include <string>
#include <string_view>

#include "color.h"
#include "grid.h"
#include "hex.h"

namespace turtle_art {
namespace {

// Distance in SVG units from a tile's center to each of its corners.
constexpr double kTileSize = 10.0;

// Width of a pointy-top hexagon, from its left side to its right side.
// std::numbers::sqrt3 (C++20) is the constant sqrt(3) as a double.
constexpr double kTileWidth = std::numbers::sqrt3 * kTileSize;

// Fill colors, one per Color, in the same order as the enum values.
// std::string_view (C++17) is a read-only view of characters stored
// elsewhere, here string literals, without copying them.
constexpr std::array<std::string_view, kColorCount> kFills = {
    "#f5f0e6",  // kUnpainted: off-white background.
    "#e63946",  // kPaint1: red.
    "#f4a261",  // kPaint2: orange.
    "#2a9d8f",  // kPaint3: teal.
    "#264653",  // kPaint4: dark blue.
};

struct Point {
  double x = 0.0;
  double y = 0.0;
};

// The corners of a pointy-top hexagon centered on {0, 0}, clockwise from the
// top right. SVG's y axis points down. Spelled out, rather than computed with
// sin/cos, so the output is exact and identical on every platform.
constexpr std::array<Point, 6> kCorners = {{
    {.x = kTileWidth / 2, .y = -kTileSize / 2},
    {.x = kTileWidth / 2, .y = kTileSize / 2},
    {.x = 0.0, .y = kTileSize},
    {.x = -kTileWidth / 2, .y = kTileSize / 2},
    {.x = -kTileWidth / 2, .y = -kTileSize / 2},
    {.x = 0.0, .y = -kTileSize},
}};

// Returns the center of `hex` in SVG coordinates, with the grid's center
// tile at {0, 0}. See
// https://www.redblobgames.com/grids/hexagons/#hex-to-pixel-axial
Point CenterOf(Hex hex) {
  return {.x = kTileWidth * (hex.q + (hex.r / 2.0)),
          .y = kTileSize * 1.5 * hex.r};
}

// Returns one SVG <polygon> element drawing `hex` filled with `color`.
std::string Polygon(Hex hex, Color color) {
  const Point center = CenterOf(hex);
  std::string points;
  // A range-based for loop (C++11) visits each element of the array.
  for (const Point& corner : kCorners) {
    if (!points.empty()) {
      points += ' ';
    }
    // std::format (C++20) builds a string from a pattern; {:.2f} prints a
    // number with 2 decimals.
    points +=
        std::format("{:.2f},{:.2f}", center.x + corner.x, center.y + corner.y);
  }
  const std::string_view fill = kFills.at(static_cast<std::size_t>(color));
  // The stroke, in the fill color, covers the thin seams that anti-aliasing
  // would otherwise leave between neighboring tiles.
  return std::format(
      "  <polygon points=\"{}\" fill=\"{}\" stroke=\"{}\" "
      "stroke-width=\"0.5\"/>\n",
      points, fill, fill);
}

}  // namespace

std::string ToSvg(const Grid& grid) {
  const int radius = grid.radius();
  // Half the size of the whole grid: the outermost tile centers, plus the
  // half tile beyond them.
  const double half_width = kTileWidth * (radius + 0.5);
  const double half_height = kTileSize * ((1.5 * radius) + 1.0);

  std::string svg = std::format(
      "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{:.2f}\" "
      "height=\"{:.2f}\" viewBox=\"{:.2f} {:.2f} {:.2f} {:.2f}\">\n",
      2 * half_width, 2 * half_height, -half_width, -half_height,
      2 * half_width, 2 * half_height);
  // Visit every (q, r) in the square that encloses the hexagon, and keep
  // those that are tiles of the grid.
  for (int r = -radius; r <= radius; ++r) {
    for (int q = -radius; q <= radius; ++q) {
      const Hex hex = {.q = q, .r = r};
      if (grid.Contains(hex)) {
        svg += Polygon(hex, grid.ColorAt(hex));
      }
    }
  }
  svg += "</svg>\n";
  return svg;
}

}  // namespace turtle_art
