#ifndef TURTLE_ART_SRC_GRID_H_
#define TURTLE_ART_SRC_GRID_H_

#include <map>

#include "color.h"
#include "hex.h"

namespace turtle_art {

// A hexagon-shaped grid of tiles: every tile within `radius` steps of the
// center {0, 0}. Each tile has a color; tiles start unpainted.
class Grid {
 public:
  // Creates a grid of the given radius, all unpainted. `radius` must be
  // zero or more; a radius of 0 is the center tile alone.
  //
  // `explicit` stops an int from silently converting to a Grid, e.g. when
  // passing 5 to a function that takes a Grid.
  explicit Grid(int radius);

  // Returns the radius given at construction.
  int radius() const { return radius_; }

  // Returns whether `hex` is a tile of this grid.
  bool Contains(Hex hex) const;

  // Returns the color of `hex`, which must be a tile of this grid.
  Color ColorAt(Hex hex) const;

  // Sets the color of `hex`, which must be a tile of this grid.
  void Paint(Hex hex, Color color);

 private:
  int radius_;
  // Only painted tiles are stored; a tile missing from the map is unpainted.
  // KISS: a map is simpler than turning hex coordinates into array indices.
  std::map<Hex, Color> colors_;
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_GRID_H_
