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
#include "palette.h"

namespace turtle_art {
namespace {

// Distance in SVG units from a tile's center to each of its corners.
constexpr double kTileSize = 10.0;

// Width of a pointy-top hexagon, from its left side to its right side.
// std::numbers::sqrt3 (C++20) is the constant sqrt(3) as a double.
constexpr double kTileWidth = std::numbers::sqrt3 * kTileSize;

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

// Returns the <defs> and <style> elements every image starts with.
//
// The hexagon is defined once, centered on {0, 0}, and each tile is a short
// <use> element that places a copy of it. That keeps a large image several
// times smaller than one full <polygon> per tile. Colors are CSS classes:
// class "c0" is kUnpainted, "c1" kPaint1, and so on.
std::string Header() {
  std::string points;
  // A range-based for loop (C++11) visits each element of the array.
  for (const Point& corner : kCorners) {
    if (!points.empty()) {
      points += ' ';
    }
    // std::format (C++20) builds a string from a pattern; {:.2f} prints a
    // number with 2 decimals.
    points += std::format("{:.2f},{:.2f}", corner.x, corner.y);
  }
  std::string styles;
  for (std::size_t i = 0; i < kPalette.size(); ++i) {
    // The stroke, in the fill color, covers the thin seams that
    // anti-aliasing would otherwise leave between neighboring tiles. In a
    // format pattern, {{ and }} print literal braces.
    styles +=
        std::format(".c{}{{fill:{};stroke:{}}}", i, kPalette[i], kPalette[i]);
  }
  return std::format(
      "  <defs><polygon id=\"t\" points=\"{}\" stroke-width=\"0.5\"/>"
      "</defs>\n"
      "  <style>{}</style>\n",
      points, styles);
}

// Returns one <use> element drawing `hex` in `color`.
std::string Tile(Hex hex, Color color) {
  const Point center = CenterOf(hex);
  return std::format(
      "  <use href=\"#t\" x=\"{:.2f}\" y=\"{:.2f}\" class=\"c{}\"/>\n",
      center.x, center.y, static_cast<int>(color));
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
  svg += Header();
  for (const Hex hex : grid.Tiles()) {
    svg += Tile(hex, grid.ColorAt(hex));
  }
  svg += "</svg>\n";
  return svg;
}

}  // namespace turtle_art
