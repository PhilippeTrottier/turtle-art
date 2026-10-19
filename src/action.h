#ifndef TURTLE_ART_SRC_ACTION_H_
#define TURTLE_ART_SRC_ACTION_H_

namespace turtle_art {

// What the turtle does in one step, before painting and moving on.
enum class Action {
  kKeep,         // Keep the current heading.
  kTurnLeft,     // Turn counter-clockwise: 60 degrees on hexagons, 90 on
                 // squares.
  kTurnRight,    // Turn clockwise, by the same angle.
  kChangeColor,  // Switch to the next palette color.
};

// The number of Action values.
inline constexpr int kActionCount = 4;

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_ACTION_H_
