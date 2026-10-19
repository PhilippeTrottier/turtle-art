#include "layout.h"

#include <gtest/gtest.h>

#include <vector>

#include "grid.h"
#include "tile.h"

namespace turtle_art {
namespace {

TEST(LayoutTest, CenterTileIsAtTheOrigin) {
  const Point center = TileCenter(Grid::Hexagon(2), Tile{});
  EXPECT_DOUBLE_EQ(center.x, 0.0);
  EXPECT_DOUBLE_EQ(center.y, 0.0);
}

TEST(LayoutTest, HexTileEastOfCenterIsOneHexWidthRight) {
  const Point center = TileCenter(Grid::Hexagon(2), Tile{.q = 1, .r = 0});
  EXPECT_NEAR(center.x, 17.32, 0.01);
  EXPECT_DOUBLE_EQ(center.y, 0.0);
}

TEST(LayoutTest, SquareTileIsTenUnitsPerColumnAndRow) {
  const Point center = TileCenter(Grid::Rectangle(5, 5), Tile{.q = 2, .r = -1});
  EXPECT_DOUBLE_EQ(center.x, 20.0);
  EXPECT_DOUBLE_EQ(center.y, -10.0);
}

TEST(LayoutTest, HexTileHasSixCorners) {
  EXPECT_EQ(TileOutline(Grid::Hexagon(1)).size(), 6U);
}

TEST(LayoutTest, SquareTileHasFourCorners) {
  EXPECT_EQ(TileOutline(Grid::Rectangle(2, 2)).size(), 4U);
}

TEST(LayoutTest, RectangleBoundsCoverEveryColumnAndRow) {
  // 4 x 3 tiles: columns -2 to 1, rows -1 to 1, 10 units each.
  const Box bounds = PictureBounds(Grid::Rectangle(4, 3));
  EXPECT_DOUBLE_EQ(bounds.x, -25.0);
  EXPECT_DOUBLE_EQ(bounds.y, -15.0);
  EXPECT_DOUBLE_EQ(bounds.width, 40.0);
  EXPECT_DOUBLE_EQ(bounds.height, 30.0);
}

TEST(LayoutTest, HexagonBoundsAreCenteredOnTheOrigin) {
  const Box bounds = PictureBounds(Grid::Hexagon(0));
  EXPECT_NEAR(bounds.x, -8.66, 0.01);
  EXPECT_DOUBLE_EQ(bounds.y, -10.0);
  EXPECT_NEAR(bounds.width, 17.32, 0.01);
  EXPECT_DOUBLE_EQ(bounds.height, 20.0);
}

TEST(LayoutTest, HexagonCopiesSitAroundThePicture) {
  // Radius 0: the first copy is the tile east of the center.
  const std::vector<Point> offsets = WrapCopyOffsets(Grid::Hexagon(0), 1);
  EXPECT_EQ(offsets.size(), 6U);
  EXPECT_NEAR(offsets.front().x, 17.32, 0.01);
  EXPECT_DOUBLE_EQ(offsets.front().y, 0.0);
}

TEST(LayoutTest, RectangleCopyIsOnePictureWidthAway) {
  const std::vector<Point> offsets = WrapCopyOffsets(Grid::Rectangle(4, 3), 1);
  EXPECT_DOUBLE_EQ(offsets.front().x, 40.0);
  EXPECT_DOUBLE_EQ(offsets.front().y, 0.0);
}

TEST(LayoutTest, HexagonSecondRingAddsTwelveCopies) {
  EXPECT_EQ(WrapCopyOffsets(Grid::Hexagon(3), 2).size(), 18U);
}

TEST(LayoutTest, RectangleSecondRingAddsSixteenCopies) {
  EXPECT_EQ(WrapCopyOffsets(Grid::Rectangle(4, 3), 2).size(), 24U);
}

TEST(LayoutTest, SecondRingStartsTwoCopiesAway) {
  // A 4 x 3 rectangle: ring 1 is 8 copies, then ring 2 starts two widths east.
  const std::vector<Point> offsets = WrapCopyOffsets(Grid::Rectangle(4, 3), 2);
  EXPECT_DOUBLE_EQ(offsets[8].x, 80.0);
  EXPECT_DOUBLE_EQ(offsets[8].y, 0.0);
}

TEST(LayoutTest, ScreenTheSizeOfThePictureNeedsOneRing) {
  const Grid grid = Grid::Hexagon(9);
  const Box bounds = PictureBounds(grid);
  EXPECT_EQ(WrapRingsToFill(grid, bounds.width, bounds.height), 1);
}

TEST(LayoutTest, WiderScreenNeedsMoreRings) {
  // Radius 9 copies sit about 285 units apart; a 4:3 screen about 2.5
  // pictures high needs 2 rings, an ultrawide one 3.
  const Grid grid = Grid::Hexagon(9);
  EXPECT_EQ(WrapRingsToFill(grid, 730, 547), 2);
  EXPECT_EQ(WrapRingsToFill(grid, 1232, 516), 3);
}

}  // namespace
}  // namespace turtle_art
