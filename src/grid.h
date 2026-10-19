#ifndef TURTLE_ART_SRC_GRID_H_
#define TURTLE_ART_SRC_GRID_H_

#include <map>
#include <optional>
#include <vector>

#include "color.h"
#include "direction.h"
#include "hex.h"

namespace turtle_art {

// What happens when a step would leave the grid.
enum class EdgeMode {
  kStop,  // There is no tile beyond the edge; the walk ends there.
  kWrap,  // The step comes back in on the opposite side of the hexagon.
};

// A hexagon-shaped grid of tiles: every tile within `radius` steps of the
// center {0, 0}. Each tile has a color; tiles start unpainted.
class Grid {
 public:
  // Creates a grid of the given radius, all unpainted. `radius` must be
  // zero or more; a radius of 0 is the center tile alone.
  //
  // `explicit` stops an int from silently converting to a Grid, e.g. when
  // passing 5 to a function that takes a Grid.
  explicit Grid(int radius, EdgeMode edge_mode = EdgeMode::kStop);

  // Returns the radius given at construction.
  int radius() const { return radius_; }

  // Returns the edge mode given at construction.
  EdgeMode edge_mode() const { return edge_mode_; }

  // Returns whether `hex` is a tile of this grid.
  bool Contains(Hex hex) const;

  // Returns every tile of this grid, row by row: r from -radius to radius,
  // and within a row q from low to high. Outputs (SVG, the web explorer)
  // rely on this order.
  std::vector<Hex> Tiles() const;

  // Returns the tile one step from `hex` in `direction`, which must be a tile
  // of this grid. If the step leaves the grid, returns the tile it wraps to
  // with EdgeMode::kWrap, and std::nullopt (no tile) with EdgeMode::kStop.
  std::optional<Hex> NeighborOf(Hex hex, Direction direction) const;

  // Returns the color of `hex`, which must be a tile of this grid.
  Color ColorAt(Hex hex) const;

  // Sets the color of `hex`, which must be a tile of this grid.
  void Paint(Hex hex, Color color);

 private:
  // Returns the tile of this grid that `hex`, a tile just outside the
  // edge, wraps to.
  Hex Wrap(Hex hex) const;

  int radius_;
  EdgeMode edge_mode_;
  // Only painted tiles are stored; a tile missing from the map is unpainted.
  // KISS: a map is simpler than turning hex coordinates into array indices.
  std::map<Hex, Color> colors_;
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_GRID_H_
