#ifndef TURTLE_ART_SRC_ART_H_
#define TURTLE_ART_SRC_ART_H_

#include <cstdint>
#include <string>

#include "grid.h"
#include "rule_table.h"
#include "turtle.h"

namespace turtle_art {

// Everything that decides what a picture looks like. The defaults are the
// command line's and the web explorer's defaults (web/app.js): a small grid
// walked long enough to fill it, which gives the best pictures most often.
struct ArtSettings {
  // Seed for the rule table. With the other defaults, 1 draws streams of
  // color across the whole grid.
  std::uint32_t seed = 1;
  int steps = 15000;  // Maximum number of turtle steps.
  // The grid: a hexagon of `radius` tiles (zero or more), or a rectangle of
  // `width` x `height` square tiles (each at least 1). The sizes of the
  // other shape are ignored.
  GridShape shape = GridShape::kHexagon;
  int radius = 15;
  int width = 0;
  int height = 0;
  // What happens at the grid's edge. Wrapping is the default because with
  // kStop most turtles walk off the grid within a few dozen steps.
  EdgeMode edge_mode = EdgeMode::kWrap;
  // Number of palette colors, in [1, kMaxPaintCount].
  int paint_count = kDefaultPaintCount;
  // Number of recent tiles, the current one included, whose colors decide
  // each action, in [1, kMaxHistoryLength]. 1 means only the tile the turtle
  // stands on, as in Langton's ant.
  int history_length = 1;
  // How the turtle colors tiles: its own color, or each tile's next color.
  PaintMode paint_mode = PaintMode::kTurtleColor;
  // Steps per second to animate the picture at, replaying the walk (see
  // ToAnimatedSvg); 0 for a still picture of the final state.
  double hz = 0.0;
};

// Returns the unpainted grid the settings describe (shape, size, edge).
Grid MakeGrid(const ArtSettings& settings);

// Runs a turtle with a rule table made from the settings on a fresh grid,
// and returns the painted grid. The same settings always give the same grid.
Grid RunArt(const ArtSettings& settings);

// Returns the picture for the settings as an SVG image: still, or animated
// when settings.hz is more than 0.
std::string MakeArt(const ArtSettings& settings);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_ART_H_
