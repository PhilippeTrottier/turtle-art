#include "layout.h"

#include <gtest/gtest.h>

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

}  // namespace
}  // namespace turtle_art
