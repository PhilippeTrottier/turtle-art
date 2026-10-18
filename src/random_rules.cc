#include "random_rules.h"

#include <cstddef>
#include <cstdint>
#include <random>

#include "action.h"
#include "rule_table.h"

namespace turtle_art {

RuleTable RandomRuleTable(std::uint32_t seed, int paint_count,
                          int history_length) {
  // std::mt19937 (the Mersenne Twister) is specified exactly by the
  // standard, so it produces the same numbers everywhere. The standard
  // distributions, such as std::uniform_int_distribution, are not: each
  // standard library may implement them differently. So we map the raw
  // numbers to actions ourselves. The engine's 2^32 possible values divide
  // evenly by kActionCount (4), so every action is equally likely.
  std::mt19937 engine(seed);
  RuleTable rules(paint_count, history_length);
  for (std::size_t i = 0; i < rules.size(); ++i) {
    rules.SetActionAt(i, static_cast<Action>(engine() % kActionCount));
  }
  return rules;
}

}  // namespace turtle_art
