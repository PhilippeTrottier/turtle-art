#ifndef TURTLE_ART_SRC_TURTLE_H_
#define TURTLE_ART_SRC_TURTLE_H_

#include <optional>

#include "color.h"
#include "direction.h"
#include "grid.h"
#include "hex.h"
#include "rule_table.h"

namespace turtle_art {

// The turtle that walks the grid and paints it. See "The walk" in README.md.
//
// The grid and the rules are passed to Step() rather than stored, so a test
// can set up any grid and rule table it needs and inspect them afterwards.
class Turtle {
 public:
  // Creates a turtle on the center tile, heading east, painting with
  // kPaint1, that has not visited any tile before.
  Turtle() = default;

  // Returns the tile the turtle stands on.
  Hex position() const { return position_; }

  // Returns the direction the turtle will move in.
  Direction heading() const { return heading_; }

  // Returns the color the turtle paints with.
  Color color() const { return color_; }

  // Takes one step: senses the colors of the last 3 tiles, applies the
  // action `rules` gives for them, paints the current tile, and moves one
  // tile forward. If that move would leave `grid`, the turtle stays put and
  // Step returns false; otherwise it returns true.
  //
  // The turtle must be on a tile of `grid`.
  bool Step(Grid& grid, const RuleTable& rules);

 private:
  Hex position_;
  Direction heading_ = Direction::kEast;
  Color color_ = Color::kPaint1;
  // The tiles visited before the current one. std::optional (C++17) holds
  // either a value or nothing: here, nothing means "not visited yet".
  std::optional<Hex> one_back_;
  std::optional<Hex> two_back_;
};

// Makes `turtle` take up to `max_steps` steps on `grid`, stopping early if
// it reaches the edge. Returns the number of steps taken, which includes the
// final step at the edge (it still paints).
int Walk(Turtle& turtle, Grid& grid, const RuleTable& rules, int max_steps);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_TURTLE_H_
