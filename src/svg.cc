#include "svg.h"

#include <cstddef>
#include <format>
#include <string>

#include "color.h"
#include "grid.h"
#include "layout.h"
#include "palette.h"
#include "tile.h"

namespace turtle_art {
namespace {

// Returns the <defs> and <style> elements every image starts with.
//
// One tile shape (a hexagon or a square) is defined once, centered on
// {0, 0}, and each tile is a short <use> element that places a copy of it.
// That keeps a large image several times smaller than one full <polygon>
// per tile. Colors are CSS classes: class "c0" is kUnpainted, "c1" kPaint1,
// and so on.
std::string Header(const Grid& grid) {
  std::string points;
  // A range-based for loop (C++11) visits each element of the vector.
  for (const Point& corner : TileOutline(grid)) {
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

// Returns one <use> element drawing `tile` of `grid` in `color`.
std::string TileElement(const Grid& grid, Tile tile, Color color) {
  const Point center = TileCenter(grid, tile);
  return std::format(
      "  <use href=\"#t\" x=\"{:.2f}\" y=\"{:.2f}\" class=\"c{}\"/>\n",
      center.x, center.y, static_cast<int>(color));
}

}  // namespace

std::string ToSvg(const Grid& grid) {
  const Box bounds = PictureBounds(grid);
  std::string svg = std::format(
      "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{:.2f}\" "
      "height=\"{:.2f}\" viewBox=\"{:.2f} {:.2f} {:.2f} {:.2f}\">\n",
      bounds.width, bounds.height, bounds.x, bounds.y, bounds.width,
      bounds.height);
  svg += Header(grid);
  for (const Tile tile : grid.Tiles()) {
    svg += TileElement(grid, tile, grid.ColorAt(tile));
  }
  svg += "</svg>\n";
  return svg;
}

}  // namespace turtle_art
