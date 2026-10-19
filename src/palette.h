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
  kDark,   // Colors on black, for dark pages and screens.
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

// The same hues, brightened where they would vanish on black: kPaint4, the
// light palette's dark blue, becomes a light one.
inline constexpr Palette kDarkPalette = {{
    {.red = 0x00, .green = 0x00, .blue = 0x00},  // kUnpainted: black.
    {.red = 0xef, .green = 0x47, .blue = 0x6f},  // kPaint1: red.
    {.red = 0xf4, .green = 0xa2, .blue = 0x61},  // kPaint2: orange.
    {.red = 0x2e, .green = 0xc4, .blue = 0xb6},  // kPaint3: teal.
    {.red = 0xa8, .green = 0xda, .blue = 0xdc},  // kPaint4: powder blue.
    {.red = 0x95, .green = 0xd5, .blue = 0xb2},  // kPaint5: sage green.
    {.red = 0xb3, .green = 0x88, .blue = 0xeb},  // kPaint6: purple.
    {.red = 0x4c, .green = 0xc9, .blue = 0xf0},  // kPaint7: sky blue.
    {.red = 0xff, .green = 0xd1, .blue = 0x66},  // kPaint8: yellow.
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
