#ifndef TURTLE_ART_SRC_GRID_STATS_H_
#define TURTLE_ART_SRC_GRID_STATS_H_

#include <vector>

#include "grid.h"

namespace turtle_art {

// Returns how many tiles of `grid` have each color: element i counts the
// tiles whose Color has value i (element 0 counts unpainted tiles). The
// vector has one element per Color value.
std::vector<int> ColorCounts(const Grid& grid);

// Returns the Shannon entropy, in bits, of a distribution given as counts:
// 0 when everything is in one bucket, up to log2(number of buckets) when the
// counts are equal. Empty buckets are ignored.
//
// Used as an "interestingness" score: flat one-color pictures and tiny loops
// score near 0, pictures spread evenly over several colors score high.
double Entropy(const std::vector<int>& counts);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_GRID_STATS_H_
