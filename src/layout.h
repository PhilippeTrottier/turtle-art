#ifndef TURTLE_ART_SRC_LAYOUT_H_
#define TURTLE_ART_SRC_LAYOUT_H_

#include <vector>

#include "grid.h"
#include "tile.h"

namespace turtle_art {

// Where tiles go in a picture, in picture units (SVG user units, which the
// web explorer scales to pixels). The center tile is at {0, 0} and y grows
// downward, as on screens. Every output draws from these, so they agree.

// A position in picture units.
struct Point {
  double x = 0.0;
  double y = 0.0;
};

// A rectangle in picture units: its top-left corner and its size.
struct Box {
  double x = 0.0;
  double y = 0.0;
  double width = 0.0;
  double height = 0.0;
};

// Returns the center of `tile`, a tile of `grid`.
Point TileCenter(const Grid& grid, Tile tile);

// Returns the corners of one of `grid`'s tiles centered on {0, 0},
// clockwise from the top right: 6 for a hexagon tile, 4 for a square.
std::vector<Point> TileOutline(const Grid& grid);

// Returns the smallest box holding every tile of `grid`.
Box PictureBounds(const Grid& grid);

// Returns where copies of the picture go around it, as offsets from the
// picture itself, so that a turtle wrapping off one side can be followed
// into the next copy (see Grid::WrapCopyCenters). `rings` is how many rings
// of copies, at least 1: ring 1 touches the picture (6 copies for a
// hexagon, 8 for a rectangle), ring 2 surrounds ring 1, and so on. Offsets
// are listed ring by ring, nearest first.
std::vector<Point> WrapCopyOffsets(const Grid& grid, int rings);

// Returns how many rings of copies it takes to fill a screen `width` x
// `height` (in picture units) centered on the picture, at least 1. It errs
// on the side of one ring too many rather than leaving a gap.
int WrapRingsToFill(const Grid& grid, double width, double height);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_LAYOUT_H_
