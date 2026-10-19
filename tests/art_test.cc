#include "art.h"

#include <gtest/gtest.h>

#include "grid.h"
#include "svg.h"
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
  EXPECT_EQ(MakeArt({.seed = 3, .steps = 0, .radius = 2}), ToSvg(Grid(2)));
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

}  // namespace
}  // namespace turtle_art
