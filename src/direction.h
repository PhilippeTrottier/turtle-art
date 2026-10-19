#ifndef TURTLE_ART_SRC_DIRECTION_H_
#define TURTLE_ART_SRC_DIRECTION_H_

namespace turtle_art {

// The six directions from a hex tile to its neighbors, for "pointy-top"
// hexes (a tile has neighbors straight east and west).
//
// The values are listed counter-clockwise, so turning left means moving to
// the next value and turning right means moving to the previous one.
//
// `enum class` (C++11) is a scoped enum: values must be written
// `Direction::kEast`, and they do not silently convert to int.
enum class Direction {
  kEast,
  kNorthEast,
  kNorthWest,
  kWest,
  kSouthWest,
  kSouthEast,
};

// Returns the direction 60 degrees counter-clockwise from `direction`.
Direction TurnLeft(Direction direction);

// Returns the direction 60 degrees clockwise from `direction`.
Direction TurnRight(Direction direction);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_DIRECTION_H_
