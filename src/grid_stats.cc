#include "grid_stats.h"

#include <cmath>
#include <cstddef>
#include <vector>

#include "color.h"
#include "grid.h"
#include "tile.h"

namespace turtle_art {

std::vector<int> ColorCounts(const Grid& grid) {
  std::vector<int> counts(kMaxPaintCount + 1, 0);
  for (const Tile hex : grid.Tiles()) {
    // Color values are 0 to kMaxPaintCount, so the index is in range.
    ++counts[static_cast<std::size_t>(grid.ColorAt(hex))];
  }
  return counts;
}

double Entropy(const std::vector<int>& counts) {
  double total = 0.0;
  for (const int count : counts) {
    total += count;
  }
  double entropy = 0.0;
  for (const int count : counts) {
    if (count > 0) {
      const double share = count / total;
      entropy -= share * std::log2(share);
    }
  }
  return entropy;
}

}  // namespace turtle_art
