#include "turtle.h"

#include <cstddef>
#include <optional>
#include <vector>

#include "action.h"
#include "color.h"
#include "grid.h"
#include "rule_table.h"
#include "tile.h"

namespace turtle_art {

bool Turtle::Step(Grid& grid, const RuleTable& rules) {
  // Sense: the colors of the last history_length() tiles, oldest first.
  // Tiles not visited yet (at the start of the walk) count as unpainted.
  const auto history_length = static_cast<std::size_t>(rules.history_length());
  std::vector<Color> sensed;
  const std::size_t not_visited_yet = history_length - 1 - previous_.size();
  sensed.insert(sensed.end(), not_visited_yet, Color::kUnpainted);
  for (const Tile tile : previous_) {
    sensed.push_back(grid.ColorAt(tile));
  }
  sensed.push_back(grid.ColorAt(position_));

  switch (rules.ActionFor(sensed)) {
    case Action::kKeep:
      break;
    // Headings count left turns, so turning adds or subtracts one, modulo
    // the grid's number of directions. Adding the count before % keeps the
    // result non-negative: in C++, -1 % 6 is -1, not 5.
    case Action::kTurnLeft:
      heading_ = (heading_ + 1) % grid.direction_count();
      break;
    case Action::kTurnRight:
      heading_ =
          (heading_ - 1 + grid.direction_count()) % grid.direction_count();
      break;
    case Action::kChangeColor:
      color_ = NextPaint(color_, rules.paint_count());
      break;
  }

  const Color paint =
      paint_mode_ == PaintMode::kCycle
          ? NextPaint(grid.ColorAt(position_), rules.paint_count())
          : color_;
  grid.Paint(position_, paint);

  const std::optional<Tile> next = grid.NeighborOf(position_, heading_);
  if (!next) {
    return false;
  }
  previous_.push_back(position_);
  if (previous_.size() >= history_length) {
    previous_.pop_front();
  }
  position_ = *next;
  return true;
}

std::vector<PaintEvent> Walk(Turtle& turtle, Grid& grid, const RuleTable& rules,
                             int max_steps) {
  std::vector<PaintEvent> painted;
  while (static_cast<int>(painted.size()) < max_steps) {
    // Step() paints the tile the turtle stands on, then moves off it.
    const Tile tile = turtle.position();
    const bool moved = turtle.Step(grid, rules);
    painted.push_back({.tile = tile, .color = grid.ColorAt(tile)});
    if (!moved) {
      break;
    }
  }
  return painted;
}

}  // namespace turtle_art
