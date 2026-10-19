#ifndef TURTLE_ART_SRC_GRID_H_
#define TURTLE_ART_SRC_GRID_H_

#include <map>
#include <optional>
#include <vector>

#include "color.h"
#include "tile.h"

namespace turtle_art {

// What happens when a step would leave the grid.
enum class EdgeMode {
  kStop,  // There is no tile beyond the edge; the walk ends there.
  kWrap,  // The step comes back in on the opposite side of the grid.
};

// The shape of a grid, and with it the shape of its tiles.
enum class GridShape {
  // A hexagon of hexagonal ("pointy-top") tiles: every tile within a radius
  // of the center. 6 directions, 60 degrees apart.
  kHexagon,
  // A rectangle of square tiles, `width` columns by `height` rows; a square
  // is a rectangle with equal sides. 4 directions, 90 degrees apart.
  kRectangle,
};

// A grid of tiles around the center tile {0, 0}. Each tile has a color;
// tiles start unpainted.
//
// Directions are numbered, so that one turtle works on any grid shape: a
// heading counts left (counter-clockwise) turns from east, from 0 to
// direction_count() - 1.
// - hexagon: 0 east, 1 north-east, 2 north-west, 3 west, 4 south-west,
//   5 south-east.
// - rectangle: 0 east, 1 north, 2 west, 3 south.
//
// The NOLINT: clang-tidy expects moving an object never to throw, but with
// MSVC moving a std::map allocates memory, so Grid's compiler-written move
// constructor could throw std::bad_alloc. Running out of memory ends the
// program anyway, which is our policy for code without exceptions.
class Grid {  // NOLINT(bugprone-exception-escape)
 public:
  // Returns a hexagon grid of the given radius, all unpainted. `radius` must
  // be zero or more; a radius of 0 is the center tile alone.
  //
  // Static functions returning an object ("named constructors") say which
  // kind of grid is made, which two constructors taking ints could not.
  static Grid Hexagon(int radius, EdgeMode edge_mode = EdgeMode::kStop);

  // Returns a rectangle grid, `width` columns by `height` rows of square
  // tiles, all unpainted. Both must be at least 1. Columns run from
  // -(width / 2) and rows from -(height / 2), so {0, 0} is the center tile
  // (just right of / below the middle when a side is even).
  static Grid Rectangle(int width, int height,
                        EdgeMode edge_mode = EdgeMode::kStop);

  // Returns the shape given at construction.
  GridShape shape() const { return shape_; }

  // Returns a hexagon grid's radius (0 for a rectangle).
  int radius() const { return radius_; }

  // Returns a rectangle grid's number of columns (0 for a hexagon).
  int width() const { return width_; }

  // Returns a rectangle grid's number of rows (0 for a hexagon).
  int height() const { return height_; }

  // Returns the edge mode given at construction.
  EdgeMode edge_mode() const { return edge_mode_; }

  // Returns how many directions a tile has neighbors in (see above): 6 for
  // a hexagon, 4 for a rectangle.
  int direction_count() const;

  // Returns whether `tile` is a tile of this grid.
  bool Contains(Tile tile) const;

  // Returns every tile of this grid, row by row: r from low to high, and
  // within a row q from low to high. Outputs (SVG, the web explorer) rely on
  // this order.
  std::vector<Tile> Tiles() const;

  // Returns the tile one step from `tile` (a tile of this grid) in direction
  // `heading`, in [0, direction_count()). If the step leaves the grid,
  // returns the tile it wraps to with EdgeMode::kWrap, and std::nullopt (no
  // tile) with EdgeMode::kStop.
  std::optional<Tile> NeighborOf(Tile tile, int heading) const;

  // Returns the color of `tile`, which must be a tile of this grid.
  Color ColorAt(Tile tile) const;

  // Sets the color of `tile`, which must be a tile of this grid.
  void Paint(Tile tile, Color color);

 private:
  // Private: use Hexagon() or Rectangle(). The `: shape_(shape)` member
  // initializer list in grid.cc initializes members before the body runs.
  Grid(GridShape shape, int radius, int width, int height, EdgeMode edge_mode);

  // Returns the tile of this grid that `tile`, a tile just outside the
  // edge, wraps to.
  Tile Wrap(Tile tile) const;

  // The lowest column and row of a rectangle grid.
  int min_q() const { return -(width_ / 2); }
  int min_r() const { return -(height_ / 2); }

  GridShape shape_;
  int radius_;
  int width_;
  int height_;
  EdgeMode edge_mode_;
  // Only painted tiles are stored; a tile missing from the map is unpainted.
  // KISS: a map is simpler than turning coordinates into array indices.
  std::map<Tile, Color> colors_;
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_GRID_H_
