#include "svg.h"

#include <cassert>
#include <cstddef>
#include <format>
#include <map>
#include <string>
#include <vector>

#include "color.h"
#include "grid.h"
#include "layout.h"
#include "palette.h"
#include "tile.h"
#include "turtle.h"

namespace turtle_art {
namespace {

// Returns the opening <svg> tag, sized to fit every tile of `grid`.
std::string OpeningTag(const Grid& grid) {
  const Box bounds = PictureBounds(grid);
  return std::format(
      "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{:.2f}\" "
      "height=\"{:.2f}\" viewBox=\"{:.2f} {:.2f} {:.2f} {:.2f}\">\n",
      bounds.width, bounds.height, bounds.x, bounds.y, bounds.width,
      bounds.height);
}

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
  std::string svg = OpeningTag(grid) + Header(grid);
  for (const Tile tile : grid.Tiles()) {
    svg += TileElement(grid, tile, grid.ColorAt(tile));
  }
  svg += "</svg>\n";
  return svg;
}

std::string ToAnimatedSvg(const Grid& grid,
                          const std::vector<PaintEvent>& painted, double hz) {
  assert(hz > 0);
  // Each tile's color changes, as SMIL <set> elements: at begin time, the
  // tile's class becomes the painted color, and `fill="freeze"` keeps it
  // after that (until a later <set> on the same tile takes over).
  std::map<Tile, std::string> changes;
  for (std::size_t step = 0; step < painted.size(); ++step) {
    changes[painted[step].tile] += std::format(
        "<set attributeName=\"class\" to=\"c{}\" begin=\"{:.3f}s\" "
        "fill=\"freeze\"/>",
        static_cast<int>(painted[step].color), static_cast<double>(step) / hz);
  }
  std::string svg = OpeningTag(grid) + Header(grid);
  for (const Tile tile : grid.Tiles()) {
    const auto it = changes.find(tile);
    if (it == changes.end()) {
      // Never painted: the same element as in a still picture.
      svg += TileElement(grid, tile, Color::kUnpainted);
      continue;
    }
    const Point center = TileCenter(grid, tile);
    svg += std::format(
        "  <use href=\"#t\" x=\"{:.2f}\" y=\"{:.2f}\" class=\"c0\">{}</use>\n",
        center.x, center.y, it->second);
  }
  svg += "</svg>\n";
  return svg;
}

}  // namespace turtle_art
