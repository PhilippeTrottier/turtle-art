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
  kDark,   // The same hues on blue-black, for dark screens.
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

// The light palette turned for a dark background. What makes the light one
// work is kept: the same hues (a warm pair, red and orange, against a cool
// pair, teal and blue), one vivid paint (red), one near-neutral anchor
// (blue), and a wide spread of lightness between tiles. That spread is
// flipped: the anchor, darkest on off-white, is the lightest here, and teal
// moves closest to the background. Orange and yellow stay light, since dark
// orange and yellow turn brown. The background is a deep blue-black in the
// anchor's hue, not pitch black.
inline constexpr Palette kDarkPalette = {{
    {.red = 0x09, .green = 0x18, .blue = 0x1f},  // kUnpainted: blue-black.
    {.red = 0xd6, .green = 0x46, .blue = 0x4d},  // kPaint1: red.
    {.red = 0xea, .green = 0x9d, .blue = 0x60},  // kPaint2: orange.
    {.red = 0x0c, .green = 0x79, .blue = 0x6e},  // kPaint3: deep teal.
    {.red = 0xba, .green = 0xd6, .blue = 0xe3},  // kPaint4: pale blue.
    {.red = 0x7c, .green = 0x9d, .blue = 0x74},  // kPaint5: sage green.
    {.red = 0x89, .green = 0x62, .blue = 0xc5},  // kPaint6: purple.
    {.red = 0x48, .green = 0xa5, .blue = 0xc7},  // kPaint7: sky blue.
    {.red = 0xef, .green = 0xce, .blue = 0x6f},  // kPaint8: yellow.
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
