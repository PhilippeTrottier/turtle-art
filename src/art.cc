#include "art.h"

#include <string>
#include <utility>
#include <vector>

#include "grid.h"
#include "random_rules.h"
#include "rule_table.h"
#include "svg.h"
#include "turtle.h"

namespace turtle_art {

Grid MakeGrid(const ArtSettings& settings) {
  return settings.shape == GridShape::kHexagon
             ? Grid::Hexagon(settings.radius, settings.edge_mode)
             : Grid::Rectangle(settings.width, settings.height,
                               settings.edge_mode);
}

namespace {

// A finished walk: the painted grid, and what each step painted.
struct Run {
  Grid grid;
  std::vector<PaintEvent> painted;
};

// Runs a turtle with a rule table made from the settings on a fresh grid.
Run Walked(const ArtSettings& settings) {
  const RuleTable rules = RandomRuleTable(settings.seed, settings.paint_count,
                                          settings.history_length);
  Grid grid = MakeGrid(settings);
  Turtle turtle(settings.paint_mode);
  std::vector<PaintEvent> painted = Walk(turtle, grid, rules, settings.steps);
  // std::move (C++11) hands the grid and the list over instead of copying
  // them; neither is used here afterwards.
  return {.grid = std::move(grid), .painted = std::move(painted)};
}

}  // namespace

Grid RunArt(const ArtSettings& settings) { return Walked(settings).grid; }

std::string MakeArt(const ArtSettings& settings) {
  const Run run = Walked(settings);
  if (settings.hz <= 0) {
    return ToSvg(run.grid);
  }
  return ToAnimatedSvg(run.grid, run.painted, settings.hz);
}

}  // namespace turtle_art
