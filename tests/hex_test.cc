#include "hex.h"

#include <gtest/gtest.h>

#include "direction.h"

namespace turtle_art {
namespace {

TEST(HexTest, NeighborEastOfCenter) {
  EXPECT_EQ(Neighbor(Hex{}, Direction::kEast), (Hex{.q = 1, .r = 0}));
}

TEST(HexTest, NeighborSouthWestOfCenter) {
  EXPECT_EQ(Neighbor(Hex{}, Direction::kSouthWest), (Hex{.q = -1, .r = 1}));
}

TEST(HexTest, NeighborNorthWestOfOtherTile) {
  EXPECT_EQ(Neighbor(Hex{.q = 2, .r = -1}, Direction::kNorthWest),
            (Hex{.q = 2, .r = -2}));
}

TEST(HexTest, DistanceOfCenterIsZero) {
  EXPECT_EQ(DistanceFromCenter(Hex{}), 0);
}

TEST(HexTest, DistanceAlongAnAxis) {
  EXPECT_EQ(DistanceFromCenter(Hex{.q = 0, .r = -2}), 2);
}

TEST(HexTest, DistanceOffAxis) {
  EXPECT_EQ(DistanceFromCenter(Hex{.q = -2, .r = -1}), 3);
}

}  // namespace
}  // namespace turtle_art
