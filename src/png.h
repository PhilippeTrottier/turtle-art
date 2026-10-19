#ifndef TURTLE_ART_SRC_PNG_H_
#define TURTLE_ART_SRC_PNG_H_

#include <cstdint>
#include <vector>

#include "grid.h"

namespace turtle_art {

// Returns a PNG image of `grid`, which must be a rectangle grid: each tile
// is a square of `pixels_per_tile` x `pixels_per_tile` pixels (at least 1)
// of its color, nothing smoothed. With the default of 1, the image is
// exactly `width` x `height` pixels.
//
// It is a palette PNG: the file holds the colors of kPalette once, and each
// pixel is a 4-bit index into them. With at most 9 colors that is 2 to 5
// times smaller than storing red, green and blue for every pixel.
//
// Hexagon grids have no pixel-perfect form, hence rectangles only.
std::vector<std::uint8_t> ToPng(const Grid& grid, int pixels_per_tile = 1);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_PNG_H_
