#include "palette.h"

#include <gtest/gtest.h>

#include "color.h"

namespace turtle_art {
namespace {

TEST(PaletteTest, CssColorIsHashAndSixHexDigits) {
  EXPECT_EQ(CssColor({.red = 0xe6, .green = 0x39, .blue = 0x46}), "#e63946");
}

TEST(PaletteTest, CssColorKeepsLeadingZeros) {
  EXPECT_EQ(CssColor({.red = 0x00, .green = 0x0b, .blue = 0xd8}), "#000bd8");
}

TEST(PaletteTest, LightThemeLeavesTilesOffWhite) {
  EXPECT_EQ(CssColor(PaletteOf(Theme::kLight)[0]), "#f5f0e6");
}

TEST(PaletteTest, DarkThemeLeavesTilesBlueBlack) {
  EXPECT_EQ(CssColor(PaletteOf(Theme::kDark)[0]), "#09181f");
}

TEST(PaletteTest, ThemesHaveTheSameNumberOfColors) {
  EXPECT_EQ(PaletteOf(Theme::kDark).size(), kMaxPaintCount + 1U);
}

}  // namespace
}  // namespace turtle_art
