#include "direction.h"

namespace turtle_art {

// An unnamed namespace makes its contents visible only in this file, the
// C++ replacement for `static` free functions.
namespace {

// `constexpr` means the value is known at compile time.
constexpr int kDirectionCount = 6;

// Rotates `direction` by `steps` sixths of a turn; positive is
// counter-clockwise. `steps` must be in [-kDirectionCount, kDirectionCount].
Direction Rotate(Direction direction, int steps) {
  // Adding kDirectionCount keeps the left side of % non-negative: in C++,
  // -1 % 6 is -1, not 5.
  const int index =
      (static_cast<int>(direction) + steps + kDirectionCount) % kDirectionCount;
  return static_cast<Direction>(index);
}

}  // namespace

Direction TurnLeft(Direction direction) { return Rotate(direction, 1); }

Direction TurnRight(Direction direction) { return Rotate(direction, -1); }

}  // namespace turtle_art
