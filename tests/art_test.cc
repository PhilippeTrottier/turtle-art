#include "art.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include "color.h"
#include "grid.h"
#include "palette.h"
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

TEST(ArtTest, ZeroHertzIsAStillPicture) {
  EXPECT_EQ(MakeArt({.seed = 3, .steps = 50, .radius = 4, .hz = 0}),
            ToSvg(RunArt({.seed = 3, .steps = 50, .radius = 4})));
}

TEST(ArtTest, PositiveHertzAnimates) {
  EXPECT_NE(MakeArt({.seed = 3, .steps = 50, .radius = 4, .hz = 10})
                .find("<set attributeName=\"class\""),
            std::string::npos);
}

TEST(ArtTest, ThemeChangesOnlyTheColors) {
  // The same walk in both palettes: the tiles' colors (their classes) are
  // the same, only the <style> differs.
  const Grid light = RunArt({.seed = 3, .steps = 200, .radius = 5});
  const Grid dark =
      RunArt({.seed = 3, .steps = 200, .radius = 5, .theme = Theme::kDark});
  for (const Tile tile : light.Tiles()) {
    EXPECT_EQ(light.ColorAt(tile), dark.ColorAt(tile));
  }
  EXPECT_NE(
      MakeArt({.seed = 3, .steps = 200, .radius = 5}),
      MakeArt({.seed = 3, .steps = 200, .radius = 5, .theme = Theme::kDark}));
}

TEST(ArtTest, MakePngWritesAPngFile) {
  const std::vector<std::uint8_t> png = MakePng({.seed = 3,
                                                 .steps = 50,
                                                 .shape = GridShape::kRectangle,
                                                 .width = 8,
                                                 .height = 5});
  // Every PNG file starts with byte 0x89, then the letters "PNG".
  EXPECT_EQ(png[0], 0x89);
  EXPECT_EQ(png[1], 'P');
  EXPECT_EQ(png[2], 'N');
  EXPECT_EQ(png[3], 'G');
}

}  // namespace
}  // namespace turtle_art
