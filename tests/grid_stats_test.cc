#include "grid_stats.h"

#include <gtest/gtest.h>

#include <vector>

#include "color.h"
#include "grid.h"
#include "tile.h"

namespace turtle_art {
namespace {

TEST(GridStatsTest, UnpaintedGridCountsEveryTileAsUnpainted) {
  const Grid grid = Grid::Hexagon(1);
  EXPECT_EQ(ColorCounts(grid), (std::vector<int>{7, 0, 0, 0, 0, 0, 0, 0, 0}));
}

TEST(GridStatsTest, PaintedTilesAreCountedByColor) {
  Grid grid = Grid::Hexagon(1);
  grid.Paint(Tile{}, Color::kPaint2);
  grid.Paint(Tile{.q = 1, .r = 0}, Color::kPaint2);
  grid.Paint(Tile{.q = -1, .r = 0}, Color::kPaint8);
  EXPECT_EQ(ColorCounts(grid), (std::vector<int>{4, 0, 2, 0, 0, 0, 0, 0, 1}));
}

TEST(GridStatsTest, EntropyOfOneBucketIsZero) {
  EXPECT_DOUBLE_EQ(Entropy({5, 0, 0}), 0.0);
}

TEST(GridStatsTest, EntropyOfTwoEqualBucketsIsOneBit) {
  EXPECT_DOUBLE_EQ(Entropy({3, 3}), 1.0);
}

TEST(GridStatsTest, EntropyOfFourEqualBucketsIsTwoBits) {
  EXPECT_DOUBLE_EQ(Entropy({2, 2, 2, 2, 0}), 2.0);
}

TEST(GridStatsTest, EntropyOfUnevenBucketsIsInBetween) {
  // -(3/4 * log2(3/4) + 1/4 * log2(1/4)), about 0.811.
  EXPECT_NEAR(Entropy({3, 1}), 0.811, 0.001);
}

}  // namespace
}  // namespace turtle_art
