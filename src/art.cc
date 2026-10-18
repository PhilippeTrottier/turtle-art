#include "art.h"

#include <string>

#include "grid.h"
#include "random_rules.h"
#include "rule_table.h"
#include "svg.h"
#include "turtle.h"

namespace turtle_art {

std::string MakeArt(const ArtSettings& settings) {
  const RuleTable rules = RandomRuleTable(settings.seed, settings.paint_count,
                                          settings.history_length);
  Grid grid(settings.radius, settings.edge_mode);
  Turtle turtle;
  Walk(turtle, grid, rules, settings.steps);
  return ToSvg(grid);
}

}  // namespace turtle_art
