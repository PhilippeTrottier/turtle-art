#ifndef TURTLE_ART_SRC_SVG_H_
#define TURTLE_ART_SRC_SVG_H_

#include <string>
#include <vector>

#include "grid.h"
#include "palette.h"
#include "turtle.h"

namespace turtle_art {

// Returns an SVG image of `grid`: one hexagon or square per tile, unpainted
// tiles included, filled with each tile's color in `theme`'s palette.
//
// Returning a string (rather than writing a file) keeps this function free
// of I/O, so tests can check its output directly.
std::string ToSvg(const Grid& grid, Theme theme = Theme::kLight);

// Returns an SVG image that replays a walk at `hz` steps per second (more
// than 0), using SMIL, the animation built into SVG: every tile starts
// unpainted and changes color when a step paints it. `painted` is what Walk()
// returned for `grid`; the last frame is the same picture as
// ToSvg(grid, theme).
std::string ToAnimatedSvg(const Grid& grid,
                          const std::vector<PaintEvent>& painted, double hz,
                          Theme theme = Theme::kLight);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_SVG_H_
