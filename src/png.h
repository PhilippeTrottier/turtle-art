#ifndef TURTLE_ART_SRC_PNG_H_
#define TURTLE_ART_SRC_PNG_H_

#include <cstdint>
#include <vector>

#include "grid.h"

namespace turtle_art {

// Returns a PNG image of `grid`, which must be a rectangle grid: exactly one
// pixel per tile, `width` x `height` pixels, nothing scaled or smoothed.
//
// It is a palette PNG: the file holds the colors of kPalette once, and each
// pixel is a 4-bit index into them. With at most 9 colors that is 2 to 5
// times smaller than storing red, green and blue for every pixel.
//
// Hexagon grids have no pixel-perfect form, hence rectangles only.
std::vector<std::uint8_t> ToPng(const Grid& grid);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_PNG_H_
