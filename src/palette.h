#ifndef TURTLE_ART_SRC_PALETTE_H_
#define TURTLE_ART_SRC_PALETTE_H_

#include <array>
#include <string_view>

#include "color.h"

namespace turtle_art {

// The CSS color each Color is drawn in, indexed by the Color's value: element
// 0 is kUnpainted, element 1 kPaint1, and so on. Runs with fewer paints use
// the first ones. Every output (SVG, the web explorer) uses this one list.
//
// std::string_view (C++17) is a read-only view of characters stored
// elsewhere, here string literals, without copying them.
inline constexpr std::array<std::string_view, kMaxPaintCount + 1> kPalette = {
    "#f5f0e6",  // kUnpainted: off-white background.
    "#e63946",  // kPaint1: red.
    "#f4a261",  // kPaint2: orange.
    "#2a9d8f",  // kPaint3: teal.
    "#264653",  // kPaint4: dark blue.
    "#8ab17d",  // kPaint5: sage green.
    "#9b5de5",  // kPaint6: purple.
    "#00b4d8",  // kPaint7: sky blue.
    "#ffd166",  // kPaint8: yellow.
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_PALETTE_H_
