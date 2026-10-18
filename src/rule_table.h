#ifndef TURTLE_ART_SRC_RULE_TABLE_H_
#define TURTLE_ART_SRC_RULE_TABLE_H_

#include <array>
#include <cstddef>

#include "action.h"
#include "color.h"

namespace turtle_art {

// The grid colors of the last 3 tiles the turtle visited, oldest first.
// Tiles not visited yet count as unpainted.
//
// Named members (rather than three Color parameters) make call sites
// readable and hard to get in the wrong order:
//   rules.ActionFor({.two_back = a, .one_back = b, .current = c});
struct SensedColors {
  Color two_back = Color::kUnpainted;
  Color one_back = Color::kUnpainted;
  Color current = Color::kUnpainted;
};

// Maps every combination of sensed colors to the action the turtle takes.
// There are kColorCount^3 = 125 combinations.
class RuleTable {
 public:
  // Creates a table where every combination maps to Action::kKeep.
  RuleTable();

  // Returns the action for `sensed`.
  Action ActionFor(SensedColors sensed) const;

  // Makes `sensed` map to `action`.
  void SetAction(SensedColors sensed, Action action);

  // Two tables are equal when they map every combination to the same action.
  friend bool operator==(const RuleTable&, const RuleTable&) = default;

 private:
  static constexpr std::size_t kSize =
      static_cast<std::size_t>(kColorCount) * kColorCount * kColorCount;

  // Returns where `sensed` is stored in `actions_`.
  static std::size_t IndexOf(SensedColors sensed);

  std::array<Action, kSize> actions_;
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_RULE_TABLE_H_
