#include "svg.h"

#include <gtest/gtest.h>

#include <string>

#include "color.h"
#include "grid.h"
#include "tile.h"

namespace turtle_art {
namespace {

TEST(SvgTest, SingleUnpaintedTile) {
  const Grid grid(0);
  EXPECT_EQ(ToSvg(grid),
            "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"17.32\" "
            "height=\"20.00\" viewBox=\"-8.66 -10.00 17.32 20.00\">\n"
            "  <defs><polygon id=\"t\" points=\"8.66,-5.00 8.66,5.00 "
            "0.00,10.00 -8.66,5.00 -8.66,-5.00 0.00,-10.00\" "
            "stroke-width=\"0.5\"/></defs>\n"
            "  <style>"
            ".c0{fill:#f5f0e6;stroke:#f5f0e6}"
            ".c1{fill:#e63946;stroke:#e63946}"
            ".c2{fill:#f4a261;stroke:#f4a261}"
            ".c3{fill:#2a9d8f;stroke:#2a9d8f}"
            ".c4{fill:#264653;stroke:#264653}"
            ".c5{fill:#8ab17d;stroke:#8ab17d}"
            ".c6{fill:#9b5de5;stroke:#9b5de5}"
            ".c7{fill:#00b4d8;stroke:#00b4d8}"
            ".c8{fill:#ffd166;stroke:#ffd166}"
            "</style>\n"
            "  <use href=\"#t\" x=\"0.00\" y=\"0.00\" class=\"c0\"/>\n"
            "</svg>\n");
}

TEST(SvgTest, PaintedTileUsesItsColorClass) {
  Grid grid(0);
  grid.Paint(Tile{}, Color::kPaint3);
  EXPECT_NE(ToSvg(grid).find("class=\"c3\""), std::string::npos);
}

TEST(SvgTest, TileEastOfCenterIsDrawnOneTileWidthRight) {
  const Grid grid(1);
  EXPECT_NE(ToSvg(grid).find("x=\"17.32\" y=\"0.00\""), std::string::npos);
}

TEST(SvgTest, TileSouthEastOfCenterIsDrawnBelowAndHalfRight) {
  const Grid grid(1);
  EXPECT_NE(ToSvg(grid).find("x=\"8.66\" y=\"15.00\""), std::string::npos);
}

}  // namespace
}  // namespace turtle_art
