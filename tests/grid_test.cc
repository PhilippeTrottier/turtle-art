#include "grid.h"

#include <gtest/gtest.h>

#include <optional>

#include "color.h"
#include "direction.h"
#include "tile.h"

namespace turtle_art {
namespace {

TEST(GridTest, ContainsCenter) {
  const Grid grid(2);
  EXPECT_TRUE(grid.Contains(Tile{}));
}

TEST(GridTest, ContainsTileOnEdge) {
  const Grid grid(2);
  EXPECT_TRUE(grid.Contains(Tile{.q = 2, .r = -2}));
}

TEST(GridTest, DoesNotContainTileBeyondEdge) {
  const Grid grid(2);
  EXPECT_FALSE(grid.Contains(Tile{.q = 2, .r = 1}));
}

TEST(GridTest, RadiusZeroContainsOnlyCenter) {
  const Grid grid(0);
  EXPECT_FALSE(grid.Contains(Tile{.q = 1, .r = 0}));
}

TEST(GridTest, TileStartsUnpainted) {
  const Grid grid(2);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 1}), Color::kUnpainted);
}

TEST(GridTest, PaintedTileHasThatColor) {
  Grid grid(2);
  grid.Paint(Tile{.q = 1, .r = 1}, Color::kPaint3);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 1}), Color::kPaint3);
}

TEST(GridTest, PaintingAgainReplacesColor) {
  Grid grid(2);
  grid.Paint(Tile{}, Color::kPaint1);
  grid.Paint(Tile{}, Color::kPaint2);
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint2);
}

TEST(GridTest, PaintingLeavesOtherTilesUnpainted) {
  Grid grid(2);
  grid.Paint(Tile{}, Color::kPaint1);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 0}), Color::kUnpainted);
}

TEST(GridTest, RadiusOneHasSevenTiles) {
  const Grid grid(1);
  EXPECT_EQ(grid.Tiles().size(), 7U);
}

TEST(GridTest, TilesStartWithTopRowFromLeft) {
  const Grid grid(1);
  EXPECT_EQ(grid.Tiles().front(), (Tile{.q = 0, .r = -1}));
}

TEST(GridTest, TilesEndWithBottomRowAtRight) {
  const Grid grid(1);
  EXPECT_EQ(grid.Tiles().back(), (Tile{.q = 0, .r = 1}));
}

TEST(GridTest, NeighborInsideGrid) {
  const Grid grid(2);
  EXPECT_EQ(grid.NeighborOf(Tile{}, Direction::kEast), (Tile{.q = 1, .r = 0}));
}

TEST(GridTest, StopEdgeHasNoNeighborBeyond) {
  const Grid grid(1, EdgeMode::kStop);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, Direction::kEast),
            std::nullopt);
}

// On a wrapping hexagon, leaving through the east side comes back in on the
// opposite (west) side, shifted by the hexagon's geometry.
TEST(GridTest, WrapEdgeComesBackOnOppositeSide) {
  const Grid grid(1, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, Direction::kEast),
            (Tile{.q = -1, .r = 1}));
}

TEST(GridTest, WrapEdgeOnLargerGrid) {
  const Grid grid(2, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 2, .r = 0}, Direction::kEast),
            (Tile{.q = -2, .r = 2}));
}

TEST(GridTest, WrapEdgeGoingBackReturnsToStart) {
  const Grid grid(1, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = -1, .r = 1}, Direction::kWest),
            (Tile{.q = 1, .r = 0}));
}

TEST(GridTest, WrapEdgeOnSingleTileReturnsToIt) {
  const Grid grid(0, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{}, Direction::kNorthWest), Tile{});
}

}  // namespace
}  // namespace turtle_art
