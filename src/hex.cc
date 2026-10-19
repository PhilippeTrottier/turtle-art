#include "hex.h"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <ostream>

namespace turtle_art {
namespace {

// The step from a tile to its neighbor, one per Direction, in the same order
// as the enum values so a Direction can be used as the index.
//
// std::array is a fixed-size array that knows its size and can be copied,
// unlike a built-in `Hex[6]`.
constexpr std::array<Hex, 6> kNeighborOffsets = {{
    {.q = 1, .r = 0},   // kEast
    {.q = 1, .r = -1},  // kNorthEast
    {.q = 0, .r = -1},  // kNorthWest
    {.q = -1, .r = 0},  // kWest
    {.q = -1, .r = 1},  // kSouthWest
    {.q = 0, .r = 1},   // kSouthEast
}};

}  // namespace

Hex Neighbor(Hex hex, Direction direction) {
  // Direction values are 0 to 5, so the index is always in range.
  const Hex offset = kNeighborOffsets[static_cast<std::size_t>(direction)];
  // `.q = ...` are designated initializers (C++20): they name each member.
  return {.q = hex.q + offset.q, .r = hex.r + offset.r};
}

int DistanceFromCenter(Hex hex) {
  // Axial coordinates are cube coordinates (q, r, s) with s = -q - r left
  // out. The distance is half the sum of the absolute cube coordinates.
  return (std::abs(hex.q) + std::abs(hex.r) + std::abs(hex.q + hex.r)) / 2;
}

std::ostream& operator<<(std::ostream& out, Hex hex) {
  return out << "{" << hex.q << ", " << hex.r << "}";
}

}  // namespace turtle_art
