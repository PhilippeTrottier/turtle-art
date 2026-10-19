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
// the first ones. Every output (SVG, PNG, the web explorer) uses one.
//
// `using` gives a type a shorter name (an alias), like typedef.
using Palette = std::array<Rgb, kMaxPaintCount + 1>;

// Which palette a picture is drawn in. The colors only change how a picture
// looks, never the walk.
enum class Theme {
  kLight,  // Colors on off-white, for light pages and print.
  kDark,   // Muted colors on dark slate, for dark pages and screens.
};

inline constexpr Palette kLightPalette = {{
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

// Calm, muted colors on a dark slate rather than pitch black, after the
// Nord palette (nordtheme.com): its "aurora" and "frost" colors are soft
// enough to sit side by side without glowing. Each paint keeps its role
// from the light palette (red, orange, teal, blue, ...).
inline constexpr Palette kDarkPalette = {{
    {.red = 0x20, .green = 0x24, .blue = 0x2c},  // kUnpainted: dark slate.
    {.red = 0xbf, .green = 0x61, .blue = 0x6a},  // kPaint1: dusty rose.
    {.red = 0xd0, .green = 0x87, .blue = 0x70},  // kPaint2: soft orange.
    {.red = 0x8f, .green = 0xbc, .blue = 0xbb},  // kPaint3: sea green.
    {.red = 0x5e, .green = 0x81, .blue = 0xac},  // kPaint4: steel blue.
    {.red = 0xa3, .green = 0xbe, .blue = 0x8c},  // kPaint5: sage green.
    {.red = 0xb4, .green = 0x8e, .blue = 0xad},  // kPaint6: mauve.
    {.red = 0x88, .green = 0xc0, .blue = 0xd0},  // kPaint7: frost blue.
    {.red = 0xeb, .green = 0xcb, .blue = 0x8b},  // kPaint8: straw yellow.
}};

// Returns the palette of `theme`. A constexpr function can also run at
// compile time; it returns a reference, so nothing is copied.
constexpr const Palette& PaletteOf(Theme theme) {
  return theme == Theme::kDark ? kDarkPalette : kLightPalette;
}

// Returns `rgb` as a CSS color, e.g. "#e63946", for SVG and the web page.
std::string CssColor(Rgb rgb);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_PALETTE_H_
