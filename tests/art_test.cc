#include "art.h"

#include <gtest/gtest.h>

#include <string>

#include "color.h"
#include "grid.h"
#include "svg.h"
#include "tile.h"
#include "turtle.h"

namespace turtle_art {
namespace {

TEST(ArtTest, SameSettingsGiveSameImage) {
  EXPECT_EQ(MakeArt({.seed = 3, .steps = 200, .radius = 5}),
            MakeArt({.seed = 3, .steps = 200, .radius = 5}));
}

TEST(ArtTest, DifferentSeedsGiveDifferentImages) {
  EXPECT_NE(MakeArt({.seed = 3, .steps = 200, .radius = 5}),
            MakeArt({.seed = 4, .steps = 200, .radius = 5}));
}

TEST(ArtTest, ZeroStepsGiveUnpaintedGrid) {
  EXPECT_EQ(MakeArt({.seed = 3, .steps = 0, .radius = 2}),
            ToSvg(Grid::Hexagon(2)));
}

TEST(ArtTest, EdgeModeChangesTheImage) {
  EXPECT_NE(
      MakeArt(
          {.seed = 1, .steps = 200, .radius = 3, .edge_mode = EdgeMode::kStop}),
      MakeArt({.seed = 1,
               .steps = 200,
               .radius = 3,
               .edge_mode = EdgeMode::kWrap}));
}

TEST(ArtTest, PaintCountChangesTheImage) {
  EXPECT_NE(MakeArt({.seed = 3,
                     .steps = 200,
                     .radius = 5,
                     .paint_count = 4,
                     .history_length = 3}),
            MakeArt({.seed = 3,
                     .steps = 200,
                     .radius = 5,
                     .paint_count = 7,
                     .history_length = 3}));
}

TEST(ArtTest, HistoryLengthChangesTheImage) {
  EXPECT_NE(
      MakeArt({.seed = 3, .steps = 200, .radius = 5}),
      MakeArt({.seed = 3, .steps = 200, .radius = 5, .history_length = 2}));
}

TEST(ArtTest, PaintModeChangesTheImage) {
  EXPECT_NE(MakeArt({.seed = 3, .steps = 200, .radius = 5}),
            MakeArt({.seed = 3,
                     .steps = 200,
                     .radius = 5,
                     .paint_mode = PaintMode::kCycle}));
}

TEST(ArtTest, RectangleSettingsMakeSquareTiles) {
  const std::string svg = MakeArt({.seed = 1,
                                   .steps = 50,
                                   .shape = GridShape::kRectangle,
                                   .width = 8,
                                   .height = 5});
  EXPECT_NE(svg.find("points=\"5.00,-5.00 5.00,5.00 -5.00,5.00 -5.00,-5.00\""),
            std::string::npos);
}

TEST(ArtTest, RectangleHasWidthTimesHeightTiles) {
  const Grid grid = RunArt({.seed = 1,
                            .steps = 50,
                            .shape = GridShape::kRectangle,
                            .width = 8,
                            .height = 5});
  EXPECT_EQ(grid.Tiles().size(), 40U);
}

TEST(ArtTest, MakeGridIsUnpaintedAndShapedBySettings) {
  const Grid grid = MakeGrid({.shape = GridShape::kRectangle,
                              .width = 3,
                              .height = 2,
                              .edge_mode = EdgeMode::kStop});
  EXPECT_EQ(grid.shape(), GridShape::kRectangle);
  EXPECT_EQ(grid.edge_mode(), EdgeMode::kStop);
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kUnpainted);
}

}  // namespace
}  // namespace turtle_art
