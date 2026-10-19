#include "tile.h"

#include <gtest/gtest.h>

namespace turtle_art {
namespace {

TEST(TileTest, HexDistanceOfCenterIsZero) {
  EXPECT_EQ(HexDistanceFromCenter(Tile{}), 0);
}

TEST(TileTest, HexDistanceAlongAnAxis) {
  EXPECT_EQ(HexDistanceFromCenter(Tile{.q = 0, .r = -2}), 2);
}

TEST(TileTest, HexDistanceOffAxis) {
  EXPECT_EQ(HexDistanceFromCenter(Tile{.q = -2, .r = -1}), 3);
}

}  // namespace
}  // namespace turtle_art
