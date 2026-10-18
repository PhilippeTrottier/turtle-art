#include "art.h"

#include <gtest/gtest.h>

#include "grid.h"
#include "svg.h"

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

}  // namespace
}  // namespace turtle_art
