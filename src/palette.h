#ifndef TURTLE_ART_SRC_PALETTE_H_
#define TURTLE_ART_SRC_PALETTE_H_

#include <array>
#include <cstdint>
#include <string>

#include "color.h"

namespace turtle_art {

// A color as red, green and blue amounts, 0 to 255 each.
struct Rgb {
  std::uint8_t red = 0;
  std::uint8_t green = 0;
  std::uint8_t blue = 0;
};

// The color each Color is drawn in, indexed by the Color's value: element 0
// is kUnpainted, element 1 kPaint1, and so on. Runs with fewer paints use
// the first ones. Every output (SVG, PNG, the web explorer) uses this list.
inline constexpr std::array<Rgb, kMaxPaintCount + 1> kPalette = {{
    {.red = 0xf5, .green = 0xf0, .blue = 0xe6},  // kUnpainted: off-white.
    {.red = 0xe6, .green = 0x39, .blue = 0x46},  // kPaint1: red.
    {.red = 0xf4, .green = 0xa2, .blue = 0x61},  // kPaint2: orange.
    {.red = 0x2a, .green = 0x9d, .blue = 0x8f},  // kPaint3: teal.
    {.red = 0x26, .green = 0x46, .blue = 0x53},  // kPaint4: dark blue.
    {.red = 0x8a, .green = 0xb1, .blue = 0x7d},  // kPaint5: sage green.
    {.red = 0x9b, .green = 0x5d, .blue = 0xe5},  // kPaint6: purple.
    {.red = 0x00, .green = 0xb4, .blue = 0xd8},  // kPaint7: sky blue.
    {.red = 0xff, .green = 0xd1, .blue = 0x66},  // kPaint8: yellow.
}};

// Returns `rgb` as a CSS color, e.g. "#e63946", for SVG and the web page.
std::string CssColor(Rgb rgb);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_PALETTE_H_
