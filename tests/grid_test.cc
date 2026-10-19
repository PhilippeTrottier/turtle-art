#include "grid.h"

#include <gtest/gtest.h>

#include <optional>

#include "color.h"
#include "tile.h"

namespace turtle_art {
namespace {

// Headings on a hexagon grid: left turns from east (see grid.h).
constexpr int kEast = 0;
constexpr int kNorthWest = 2;
constexpr int kWest = 3;
constexpr int kSouthWest = 4;

TEST(GridTest, ContainsCenter) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_TRUE(grid.Contains(Tile{}));
}

TEST(GridTest, ContainsTileOnEdge) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_TRUE(grid.Contains(Tile{.q = 2, .r = -2}));
}

TEST(GridTest, DoesNotContainTileBeyondEdge) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_FALSE(grid.Contains(Tile{.q = 2, .r = 1}));
}

TEST(GridTest, RadiusZeroContainsOnlyCenter) {
  const Grid grid = Grid::Hexagon(0);
  EXPECT_FALSE(grid.Contains(Tile{.q = 1, .r = 0}));
}

TEST(GridTest, TileStartsUnpainted) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 1}), Color::kUnpainted);
}

TEST(GridTest, PaintedTileHasThatColor) {
  Grid grid = Grid::Hexagon(2);
  grid.Paint(Tile{.q = 1, .r = 1}, Color::kPaint3);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 1}), Color::kPaint3);
}

TEST(GridTest, PaintingAgainReplacesColor) {
  Grid grid = Grid::Hexagon(2);
  grid.Paint(Tile{}, Color::kPaint1);
  grid.Paint(Tile{}, Color::kPaint2);
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint2);
}

TEST(GridTest, PaintingLeavesOtherTilesUnpainted) {
  Grid grid = Grid::Hexagon(2);
  grid.Paint(Tile{}, Color::kPaint1);
  EXPECT_EQ(grid.ColorAt(Tile{.q = 1, .r = 0}), Color::kUnpainted);
}

TEST(GridTest, RadiusOneHasSevenTiles) {
  const Grid grid = Grid::Hexagon(1);
  EXPECT_EQ(grid.Tiles().size(), 7U);
}

TEST(GridTest, TilesStartWithTopRowFromLeft) {
  const Grid grid = Grid::Hexagon(1);
  EXPECT_EQ(grid.Tiles().front(), (Tile{.q = 0, .r = -1}));
}

TEST(GridTest, TilesEndWithBottomRowAtRight) {
  const Grid grid = Grid::Hexagon(1);
  EXPECT_EQ(grid.Tiles().back(), (Tile{.q = 0, .r = 1}));
}

TEST(GridTest, NeighborInsideGrid) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_EQ(grid.NeighborOf(Tile{}, kEast), (Tile{.q = 1, .r = 0}));
}

TEST(GridTest, NeighborSouthWestOfCenter) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_EQ(grid.NeighborOf(Tile{}, kSouthWest), (Tile{.q = -1, .r = 1}));
}

TEST(GridTest, NeighborNorthWestOfOtherTile) {
  const Grid grid = Grid::Hexagon(3);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 2, .r = -1}, kNorthWest),
            (Tile{.q = 2, .r = -2}));
}

TEST(GridTest, HexagonHasSixDirections) {
  const Grid grid = Grid::Hexagon(2);
  EXPECT_EQ(grid.direction_count(), 6);
}

TEST(GridTest, StopEdgeHasNoNeighborBeyond) {
  const Grid grid = Grid::Hexagon(1, EdgeMode::kStop);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, kEast), std::nullopt);
}

// On a wrapping hexagon, leaving through the east side comes back in on the
// opposite (west) side, shifted by the hexagon's geometry.
TEST(GridTest, WrapEdgeComesBackOnOppositeSide) {
  const Grid grid = Grid::Hexagon(1, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, kEast),
            (Tile{.q = -1, .r = 1}));
}

TEST(GridTest, WrapEdgeOnLargerGrid) {
  const Grid grid = Grid::Hexagon(2, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 2, .r = 0}, kEast),
            (Tile{.q = -2, .r = 2}));
}

TEST(GridTest, WrapEdgeGoingBackReturnsToStart) {
  const Grid grid = Grid::Hexagon(1, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = -1, .r = 1}, kWest),
            (Tile{.q = 1, .r = 0}));
}

TEST(GridTest, WrapEdgeOnSingleTileReturnsToIt) {
  const Grid grid = Grid::Hexagon(0, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{}, kNorthWest), Tile{});
}

// Headings on a rectangle grid of square tiles (see grid.h).
constexpr int kSquareEast = 0;
constexpr int kSquareNorth = 1;

TEST(GridTest, RectangleHasFourDirections) {
  const Grid grid = Grid::Rectangle(3, 2);
  EXPECT_EQ(grid.direction_count(), 4);
}

TEST(GridTest, RectangleKnowsItsSize) {
  const Grid grid = Grid::Rectangle(5, 3);
  EXPECT_EQ(grid.shape(), GridShape::kRectangle);
  EXPECT_EQ(grid.width(), 5);
  EXPECT_EQ(grid.height(), 3);
}

// A 4 x 3 rectangle has columns -2 to 1 and rows -1 to 1.
TEST(GridTest, RectangleContainsItsCorners) {
  const Grid grid = Grid::Rectangle(4, 3);
  EXPECT_TRUE(grid.Contains(Tile{.q = -2, .r = -1}));
  EXPECT_TRUE(grid.Contains(Tile{.q = 1, .r = 1}));
}

TEST(GridTest, RectangleDoesNotContainTilesBeyondItsSides) {
  const Grid grid = Grid::Rectangle(4, 3);
  EXPECT_FALSE(grid.Contains(Tile{.q = 2, .r = 0}));
  EXPECT_FALSE(grid.Contains(Tile{.q = 0, .r = 2}));
}

TEST(GridTest, RectangleHasWidthTimesHeightTiles) {
  const Grid grid = Grid::Rectangle(4, 3);
  EXPECT_EQ(grid.Tiles().size(), 12U);
}

TEST(GridTest, RectangleTilesRunFromTopLeftToBottomRight) {
  const Grid grid = Grid::Rectangle(4, 3);
  EXPECT_EQ(grid.Tiles().front(), (Tile{.q = -2, .r = -1}));
  EXPECT_EQ(grid.Tiles().back(), (Tile{.q = 1, .r = 1}));
}

TEST(GridTest, RectangleNeighborNorthIsTheRowAbove) {
  const Grid grid = Grid::Rectangle(3, 3);
  EXPECT_EQ(grid.NeighborOf(Tile{}, kSquareNorth), (Tile{.q = 0, .r = -1}));
}

TEST(GridTest, RectangleStopEdgeHasNoNeighborBeyond) {
  const Grid grid = Grid::Rectangle(3, 3, EdgeMode::kStop);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, kSquareEast), std::nullopt);
}

TEST(GridTest, RectangleWrapsEastToTheFirstColumn) {
  const Grid grid = Grid::Rectangle(3, 3, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 1, .r = 0}, kSquareEast),
            (Tile{.q = -1, .r = 0}));
}

TEST(GridTest, RectangleWrapsNorthToTheLastRow) {
  const Grid grid = Grid::Rectangle(4, 3, EdgeMode::kWrap);
  EXPECT_EQ(grid.NeighborOf(Tile{.q = 0, .r = -1}, kSquareNorth),
            (Tile{.q = 0, .r = 1}));
}

}  // namespace
}  // namespace turtle_art
