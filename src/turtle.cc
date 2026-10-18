#include "turtle.h"

#include <optional>

#include "action.h"
#include "color.h"
#include "direction.h"
#include "grid.h"
#include "hex.h"
#include "rule_table.h"

namespace turtle_art {
namespace {

// Returns the color of `tile` on `grid`, or kUnpainted if there is no tile.
Color ColorOrUnpainted(const Grid& grid, std::optional<Hex> tile) {
  // An optional converts to bool: true when it holds a value. `*tile` then
  // reads that value.
  return tile ? grid.ColorAt(*tile) : Color::kUnpainted;
}

}  // namespace

bool Turtle::Step(Grid& grid, const RuleTable& rules) {
  const SensedColors sensed = {
      .two_back = ColorOrUnpainted(grid, two_back_),
      .one_back = ColorOrUnpainted(grid, one_back_),
      .current = grid.ColorAt(position_),
  };

  switch (rules.ActionFor(sensed)) {
    case Action::kKeep:
      break;
    case Action::kTurnLeft:
      heading_ = TurnLeft(heading_);
      break;
    case Action::kTurnRight:
      heading_ = TurnRight(heading_);
      break;
    case Action::kChangeColor:
      color_ = NextPaint(color_);
      break;
  }

  grid.Paint(position_, color_);

  const std::optional<Hex> next = grid.NeighborOf(position_, heading_);
  if (!next) {
    return false;
  }
  two_back_ = one_back_;
  one_back_ = position_;
  position_ = *next;
  return true;
}

int Walk(Turtle& turtle, Grid& grid, const RuleTable& rules, int max_steps) {
  int steps = 0;
  while (steps < max_steps) {
    ++steps;
    if (!turtle.Step(grid, rules)) {
      break;
    }
  }
  return steps;
}

}  // namespace turtle_art
