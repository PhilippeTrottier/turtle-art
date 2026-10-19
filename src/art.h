#ifndef TURTLE_ART_SRC_ART_H_
#define TURTLE_ART_SRC_ART_H_

#include <cstdint>
#include <string>

#include "grid.h"
#include "rule_table.h"
#include "turtle.h"

namespace turtle_art {

// Everything that decides what a picture looks like. The defaults are the
// command line's defaults.
struct ArtSettings {
  // Seed for the rule table. With the other defaults, 30 draws long straight
  // lines wrapping around a dense knot; most seeds are far less interesting.
  std::uint32_t seed = 30;
  int steps = 10000;  // Maximum number of turtle steps.
  int radius = 50;    // Grid radius in tiles; must be zero or more.
  // What happens at the grid's edge. Wrapping is the default because with
  // kStop most turtles walk off the grid within a few dozen steps.
  EdgeMode edge_mode = EdgeMode::kWrap;
  // Number of palette colors, in [1, kMaxPaintCount].
  int paint_count = kDefaultPaintCount;
  // Number of recent tiles, the current one included, whose colors decide
  // each action, in [1, kMaxHistoryLength].
  int history_length = kDefaultHistoryLength;
  // How the turtle colors tiles: its own color, or each tile's next color.
  PaintMode paint_mode = PaintMode::kTurtleColor;
};

// Runs a turtle with a rule table made from the settings on a fresh grid,
// and returns the painted grid. The same settings always give the same grid.
Grid RunArt(const ArtSettings& settings);

// Returns RunArt(settings) as an SVG image.
std::string MakeArt(const ArtSettings& settings);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_ART_H_
