#ifndef TURTLE_ART_SRC_SVG_H_
#define TURTLE_ART_SRC_SVG_H_

#include <string>

#include "grid.h"

namespace turtle_art {

// Returns an SVG image of `grid`: one hexagon per tile, unpainted tiles
// included, filled with each tile's palette color.
//
// Returning a string (rather than writing a file) keeps this function free
// of I/O, so tests can check its output directly.
std::string ToSvg(const Grid& grid);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_SVG_H_
