#include "random_rules.h"

#include <cstdint>
#include <random>

#include "action.h"
#include "color.h"
#include "rule_table.h"

namespace turtle_art {

RuleTable RandomRuleTable(std::uint32_t seed) {
  // std::mt19937 (the Mersenne Twister) is specified exactly by the
  // standard, so it produces the same numbers everywhere. The standard
  // distributions, such as std::uniform_int_distribution, are not: each
  // standard library may implement them differently. So we map the raw
  // numbers to actions ourselves. The engine's 2^32 possible values divide
  // evenly by kActionCount (4), so every action is equally likely.
  std::mt19937 engine(seed);
  RuleTable rules;
  for (int two_back = 0; two_back < kColorCount; ++two_back) {
    for (int one_back = 0; one_back < kColorCount; ++one_back) {
      for (int current = 0; current < kColorCount; ++current) {
        const auto action = static_cast<Action>(engine() % kActionCount);
        rules.SetAction({.two_back = static_cast<Color>(two_back),
                         .one_back = static_cast<Color>(one_back),
                         .current = static_cast<Color>(current)},
                        action);
      }
    }
  }
  return rules;
}

}  // namespace turtle_art
