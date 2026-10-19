#include "palette.h"

#include <gtest/gtest.h>

namespace turtle_art {
namespace {

TEST(PaletteTest, CssColorIsHashAndSixHexDigits) {
  EXPECT_EQ(CssColor({.red = 0xe6, .green = 0x39, .blue = 0x46}), "#e63946");
}

TEST(PaletteTest, CssColorKeepsLeadingZeros) {
  EXPECT_EQ(CssColor({.red = 0x00, .green = 0x0b, .blue = 0xd8}), "#000bd8");
}

}  // namespace
}  // namespace turtle_art
