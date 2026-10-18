#include "rule_table.h"

#include <cstddef>

#include "action.h"
#include "color.h"

namespace turtle_art {

RuleTable::RuleTable() { actions_.fill(Action::kKeep); }

Action RuleTable::ActionFor(SensedColors sensed) const {
  return actions_.at(IndexOf(sensed));
}

void RuleTable::SetAction(SensedColors sensed, Action action) {
  actions_.at(IndexOf(sensed)) = action;
}

std::size_t RuleTable::IndexOf(SensedColors sensed) {
  // Reads the three colors as the digits of a base-kColorCount number, so
  // each combination gets its own index in [0, kSize).
  //
  // `[](Color color) { ... }` is a lambda: an unnamed function written in
  // place. Storing it in `digit` lets us call it like a local function.
  const auto digit = [](Color color) {
    return static_cast<std::size_t>(color);
  };
  constexpr auto kBase = static_cast<std::size_t>(kColorCount);
  return (digit(sensed.two_back) * kBase * kBase) +
         (digit(sensed.one_back) * kBase) + digit(sensed.current);
}

}  // namespace turtle_art
