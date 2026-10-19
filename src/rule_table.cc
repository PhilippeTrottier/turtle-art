#include "rule_table.h"

#include <cassert>
#include <cstddef>
#include <span>
#include <vector>

#include "action.h"
#include "color.h"

namespace turtle_art {
namespace {

// Returns `base` raised to `exponent`, both non-negative.
std::size_t Power(std::size_t base, int exponent) {
  std::size_t result = 1;
  for (int i = 0; i < exponent; ++i) {
    result *= base;
  }
  return result;
}

}  // namespace

RuleTable::RuleTable(int paint_count, int history_length)
    : paint_count_(paint_count), history_length_(history_length) {
  assert(paint_count >= 1 && paint_count <= kMaxPaintCount);
  assert(history_length >= 1 && history_length <= kMaxHistoryLength);
  // The vector constructor taking (count, value) fills it with copies.
  actions_ = std::vector<Action>(
      Power(static_cast<std::size_t>(paint_count) + 1, history_length),
      Action::kKeep);
}

// IndexOf() checks `sensed`, so its result is always in range.
Action RuleTable::ActionFor(std::span<const Color> sensed) const {
  return actions_[IndexOf(sensed)];
}

void RuleTable::SetAction(std::span<const Color> sensed, Action action) {
  actions_[IndexOf(sensed)] = action;
}

void RuleTable::SetActionAt(std::size_t index, Action action) {
  assert(index < actions_.size());
  actions_[index] = action;
}

std::size_t RuleTable::IndexOf(std::span<const Color> sensed) const {
  assert(static_cast<int>(sensed.size()) == history_length_);
  const std::size_t base = static_cast<std::size_t>(paint_count_) + 1;
  // Horner's method: for digits d1 d2 d3, ((d1 * base) + d2) * base + d3.
  std::size_t index = 0;
  for (const Color color : sensed) {
    assert(static_cast<int>(color) <= paint_count_);
    index = (index * base) + static_cast<std::size_t>(color);
  }
  return index;
}

}  // namespace turtle_art
