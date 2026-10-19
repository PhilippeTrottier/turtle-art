#ifndef TURTLE_ART_SRC_TURTLE_H_
#define TURTLE_ART_SRC_TURTLE_H_

#include <deque>

#include "color.h"
#include "direction.h"
#include "grid.h"
#include "hex.h"
#include "rule_table.h"

namespace turtle_art {

// What color the turtle paints each tile it leaves.
enum class PaintMode {
  // The turtle's current color (changed by Action::kChangeColor).
  kTurtleColor,
  // The palette color after the tile's own, as in Langton's ant: every
  // visit changes the tile, so revisited areas keep evolving. The turtle's
  // own color is not used, so Action::kChangeColor has no visible effect.
  kCycle,
};

// The turtle that walks the grid and paints it. See "The walk" in README.md.
//
// The grid and the rules are passed to Step() rather than stored, so a test
// can set up any grid and rule table it needs and inspect them afterwards.
class Turtle {
 public:
  // Creates a turtle on the center tile, heading east, with color kPaint1,
  // that has not visited any tile before and paints as `paint_mode` says.
  explicit Turtle(PaintMode paint_mode = PaintMode::kTurtleColor)
      : paint_mode_(paint_mode) {}

  // Returns the tile the turtle stands on.
  Hex position() const { return position_; }

  // Returns the direction the turtle will move in.
  Direction heading() const { return heading_; }

  // Returns the color the turtle paints with.
  Color color() const { return color_; }

  // Takes one step: senses the colors of the last rules.history_length()
  // tiles, applies the action `rules` gives for them (changing color cycles
  // through rules.paint_count() paints), paints the current tile as the
  // paint mode says, and moves one
  // tile forward, wrapping around if `grid` wraps. If the move would leave a
  // grid that does not wrap, the turtle stays put and Step returns false;
  // otherwise it returns true.
  //
  // The turtle must be on a tile of `grid`.
  bool Step(Grid& grid, const RuleTable& rules);

 private:
  PaintMode paint_mode_;
  Hex position_;
  Direction heading_ = Direction::kEast;
  Color color_ = Color::kPaint1;
  // The tiles visited before the current one, oldest first; at most
  // history length - 1 of them. std::deque is a sequence with fast insertion
  // and removal at both ends: new tiles go in at the back, old ones leave
  // from the front.
  std::deque<Hex> previous_;
};

// Makes `turtle` take up to `max_steps` steps on `grid`, stopping early if
// it reaches the edge. Returns the number of steps taken, which includes the
// final step at the edge (it still paints).
int Walk(Turtle& turtle, Grid& grid, const RuleTable& rules, int max_steps);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_TURTLE_H_
