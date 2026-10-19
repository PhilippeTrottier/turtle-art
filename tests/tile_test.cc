#include "tile.h"

#include <gtest/gtest.h>

#include "direction.h"

namespace turtle_art {
namespace {

TEST(TileTest, NeighborEastOfCenter) {
  EXPECT_EQ(Neighbor(Tile{}, Direction::kEast), (Tile{.q = 1, .r = 0}));
}

TEST(TileTest, NeighborSouthWestOfCenter) {
  EXPECT_EQ(Neighbor(Tile{}, Direction::kSouthWest), (Tile{.q = -1, .r = 1}));
}

TEST(TileTest, NeighborNorthWestOfOtherTile) {
  EXPECT_EQ(Neighbor(Tile{.q = 2, .r = -1}, Direction::kNorthWest),
            (Tile{.q = 2, .r = -2}));
}

TEST(TileTest, DistanceOfCenterIsZero) {
  EXPECT_EQ(DistanceFromCenter(Tile{}), 0);
}

TEST(TileTest, DistanceAlongAnAxis) {
  EXPECT_EQ(DistanceFromCenter(Tile{.q = 0, .r = -2}), 2);
}

TEST(TileTest, DistanceOffAxis) {
  EXPECT_EQ(DistanceFromCenter(Tile{.q = -2, .r = -1}), 3);
}

}  // namespace
}  // namespace turtle_art
