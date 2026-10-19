#ifndef TURTLE_ART_SRC_ART_H_
#define TURTLE_ART_SRC_ART_H_

#include <cstdint>
#include <string>

#include "grid.h"
#include "rule_table.h"

namespace turtle_art {

// Everything that decides what a picture looks like. The defaults are the
// command line's defaults.
struct ArtSettings {
  // Seed for the rule table. 228 is one of the few seeds below 300 whose
  // turtle covers a large area instead of looping on a few tiles.
  std::uint32_t seed = 228;
  int steps = 10000;  // Maximum number of turtle steps.
  int radius = 50;    // Grid radius in tiles; must be zero or more.
  EdgeMode edge_mode = EdgeMode::kStop;  // What happens at the grid's edge.
  // Number of palette colors, in [1, kMaxPaintCount].
  int paint_count = kDefaultPaintCount;
  // Number of recent tiles, the current one included, whose colors decide
  // each action, in [1, kMaxHistoryLength].
  int history_length = kDefaultHistoryLength;
};

// Runs a turtle with a rule table made from the settings on a fresh grid,
// and returns the painted grid. The same settings always give the same grid.
Grid RunArt(const ArtSettings& settings);

// Returns RunArt(settings) as an SVG image.
std::string MakeArt(const ArtSettings& settings);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_ART_H_
