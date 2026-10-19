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
  kDark,   // Warm muted colors on dark brown, for dark screens.
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

// Warm, muted colors on a warm dark brown rather than pitch black, after
// the Gruvbox Material palette (github.com/sainnhe/gruvbox-material): soft
// enough to sit side by side without glowing. The first four, used by the
// default 4 colors, are the warmest and most distinct.
inline constexpr Palette kDarkPalette = {{
    {.red = 0x29, .green = 0x23, .blue = 0x1e},  // kUnpainted: dark brown.
    {.red = 0xea, .green = 0x69, .blue = 0x62},  // kPaint1: soft red.
    {.red = 0xe7, .green = 0x8a, .blue = 0x4e},  // kPaint2: orange.
    {.red = 0x89, .green = 0xb4, .blue = 0x82},  // kPaint3: aqua green.
    {.red = 0xd8, .green = 0xa6, .blue = 0x57},  // kPaint4: gold.
    {.red = 0x7d, .green = 0xae, .blue = 0xa3},  // kPaint5: muted blue.
    {.red = 0xd3, .green = 0x86, .blue = 0x9b},  // kPaint6: pink.
    {.red = 0xa9, .green = 0xb6, .blue = 0x65},  // kPaint7: olive.
    {.red = 0xd4, .green = 0xbe, .blue = 0x98},  // kPaint8: sand.
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
