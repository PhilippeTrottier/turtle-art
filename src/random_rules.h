#ifndef TURTLE_ART_SRC_RANDOM_RULES_H_
#define TURTLE_ART_SRC_RANDOM_RULES_H_

#include <cstdint>

#include "rule_table.h"

namespace turtle_art {

// Returns a rule table of the given shape (see RuleTable) with a random
// action for every combination of sensed colors. The same arguments give the
// same table on every platform and compiler.
RuleTable RandomRuleTable(std::uint32_t seed,
                          int paint_count = kDefaultPaintCount,
                          int history_length = kDefaultHistoryLength);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_RANDOM_RULES_H_
