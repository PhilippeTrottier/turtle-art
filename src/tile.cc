#include "tile.h"

#include <cstdlib>
#include <ostream>

namespace turtle_art {

int HexDistanceFromCenter(Tile tile) {
  // Axial coordinates are cube coordinates (q, r, s) with s = -q - r left
  // out. The distance is half the sum of the absolute cube coordinates.
  return (std::abs(tile.q) + std::abs(tile.r) + std::abs(tile.q + tile.r)) / 2;
}

std::ostream& operator<<(std::ostream& out, Tile tile) {
  return out << "{" << tile.q << ", " << tile.r << "}";
}

}  // namespace turtle_art
