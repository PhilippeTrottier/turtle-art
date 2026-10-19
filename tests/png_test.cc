#include "png.h"

#include <gtest/gtest.h>
#include <lodepng.h>

#include <cstdint>
#include <vector>

#include "color.h"
#include "grid.h"
#include "palette.h"
#include "tile.h"

namespace turtle_art {
namespace {

// The tests decode the PNG with lodepng into red, green, blue, alpha bytes:
// 4 per pixel, row by row from the top left.

TEST(PngTest, OnePixelPerTile) {
  const std::vector<std::uint8_t> png = ToPng(Grid::Rectangle(5, 3));
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  EXPECT_EQ(width, 5U);
  EXPECT_EQ(height, 3U);
}

TEST(PngTest, UnpaintedPixelHasTheBackgroundColor) {
  const std::vector<std::uint8_t> png = ToPng(Grid::Rectangle(1, 1));
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  // kUnpainted is #f5f0e6, fully opaque.
  EXPECT_EQ(rgba, (std::vector<std::uint8_t>{0xf5, 0xf0, 0xe6, 0xff}));
}

TEST(PngTest, PaintedTileIsAtItsPixel) {
  // A 3 x 1 rectangle: columns -1, 0 and 1, so the center tile is pixel 1.
  Grid grid = Grid::Rectangle(3, 1);
  grid.Paint(Tile{}, Color::kPaint1);
  const std::vector<std::uint8_t> png = ToPng(grid);
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  // Pixel 1 starts at byte 4; kPaint1 is #e63946.
  EXPECT_EQ(rgba[4], 0xe6);
  EXPECT_EQ(rgba[5], 0x39);
  EXPECT_EQ(rgba[6], 0x46);
}

TEST(PngTest, PixelsPerTileScalesTheImage) {
  const std::vector<std::uint8_t> png = ToPng(Grid::Rectangle(5, 3), 2);
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  EXPECT_EQ(width, 10U);
  EXPECT_EQ(height, 6U);
}

TEST(PngTest, ScaledTileIsASquareOfPixels) {
  // A 2 x 1 rectangle at 3 pixels per tile is 6 x 3 pixels. Column 0 is the
  // center tile, painted: pixels 3 to 5 of each row.
  Grid grid = Grid::Rectangle(2, 1);
  grid.Paint(Tile{}, Color::kPaint1);
  const std::vector<std::uint8_t> png = ToPng(grid, 3);
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  // Bytes 0 and 4 * 2 (pixels 0 and 2) are unpainted #f5..., bytes 4 * 3 and
  // 4 * 5 (pixels 3 and 5) are kPaint1 #e6..., and so is the last row's
  // pixel 3 (row 2 starts at byte 4 * 6 * 2 = 48).
  EXPECT_EQ(rgba[0], 0xf5);
  EXPECT_EQ(rgba[8], 0xf5);
  EXPECT_EQ(rgba[12], 0xe6);
  EXPECT_EQ(rgba[20], 0xe6);
  EXPECT_EQ(rgba[48 + 12], 0xe6);
  EXPECT_EQ(rgba[48 + 8], 0xf5);
}

TEST(PngTest, DarkThemePixelIsDarkSlate) {
  const std::vector<std::uint8_t> png =
      ToPng(Grid::Rectangle(1, 1), 1, Theme::kDark);
  std::vector<std::uint8_t> rgba;
  unsigned width = 0;
  unsigned height = 0;
  ASSERT_EQ(lodepng::decode(rgba, width, height, png), 0U);
  EXPECT_EQ(rgba, (std::vector<std::uint8_t>{0x20, 0x24, 0x2c, 0xff}));
}

// In a PNG file, byte 24 is the bit depth and byte 25 the color type, of
// which 3 means "palette".
TEST(PngTest, IsAFourBitPalettePng) {
  const std::vector<std::uint8_t> png = ToPng(Grid::Rectangle(2, 2));
  EXPECT_EQ(png[24], 4);
  EXPECT_EQ(png[25], 3);
}

}  // namespace
}  // namespace turtle_art
