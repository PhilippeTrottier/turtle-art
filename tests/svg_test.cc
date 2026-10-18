#include "svg.h"

#include <gtest/gtest.h>

#include <string>

#include "color.h"
#include "grid.h"
#include "hex.h"

namespace turtle_art {
namespace {

TEST(SvgTest, SingleUnpaintedTile) {
  const Grid grid(0);
  EXPECT_EQ(ToSvg(grid),
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"17.32\" "
            "height=\"20.00\" viewBox=\"-8.66 -10.00 17.32 20.00\">\n"
            "  <polygon points=\"8.66,-5.00 8.66,5.00 0.00,10.00 "
            "-8.66,5.00 -8.66,-5.00 0.00,-10.00\" fill=\"#f5f0e6\" "
            "stroke=\"#f5f0e6\" stroke-width=\"0.5\"/>\n"
            "</svg>\n");
}

TEST(SvgTest, PaintedTileUsesItsPaletteColor) {
  Grid grid(0);
  grid.Paint(Hex{}, Color::kPaint1);
  EXPECT_NE(ToSvg(grid).find("fill=\"#e63946\""), std::string::npos);
}

TEST(SvgTest, TileEastOfCenterIsDrawnOneTileWidthRight) {
  const Grid grid(1);
  // The east tile's center is at x = 17.32, so its left corners are at
  // 17.32 - 8.66 = 8.66 and its top corner at 17.32.
  EXPECT_NE(ToSvg(grid).find("points=\"25.98,-5.00 25.98,5.00 17.32,10.00 "
                             "8.66,5.00 8.66,-5.00 17.32,-10.00\""),
            std::string::npos);
}

}  // namespace
}  // namespace turtle_art
