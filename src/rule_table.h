#ifndef TURTLE_ART_SRC_RULE_TABLE_H_
#define TURTLE_ART_SRC_RULE_TABLE_H_

#include <cstddef>
#include <vector>

#include "action.h"
#include "color.h"

namespace turtle_art {

// The shape of a RuleTable built without arguments (as most tests do): 4
// palette colors, and 3 recently visited tiles (the current one included)
// whose colors decide the next action, the original rule of this project.
// The program's own defaults are in ArtSettings (art.h).
inline constexpr int kDefaultPaintCount = 4;
inline constexpr int kDefaultHistoryLength = 3;

// The longest history a table supports. Tables grow as
// (paint count + 1) ^ (history length): 9^6 = 531,441 entries at most.
inline constexpr int kMaxHistoryLength = 6;

// Maps every combination of sensed colors to the action the turtle takes.
//
// The sensed colors are those of the last `history_length` tiles the turtle
// visited, oldest first and the current tile last. Each is kUnpainted or one
// of the first `paint_count` paints, so there are
// (paint_count + 1) ^ history_length combinations.
class RuleTable {
 public:
  // Creates a table where every combination maps to Action::kKeep.
  // `paint_count` must be in [1, kMaxPaintCount] and `history_length` in
  // [1, kMaxHistoryLength].
  explicit RuleTable(int paint_count = kDefaultPaintCount,
                     int history_length = kDefaultHistoryLength);

  // Returns the paint count given at construction.
  int paint_count() const { return paint_count_; }

  // Returns the history length given at construction.
  int history_length() const { return history_length_; }

  // Returns the number of combinations, i.e. of entries in the table.
  std::size_t size() const { return actions_.size(); }

  // Returns the action for `sensed`, which must hold history_length()
  // colors, oldest first.
  //
  // Taking a const reference to a std::vector lets callers pass a braced
  // list, as in ActionFor({Color::kPaint1, Color::kUnpainted, ...}).
  Action ActionFor(const std::vector<Color>& sensed) const;

  // Makes `sensed` map to `action`. `sensed` is as for ActionFor().
  void SetAction(const std::vector<Color>& sensed, Action action);

  // Makes the combination at `index`, in [0, size()), map to `action`.
  // Combinations are numbered by reading the sensed colors, oldest first, as
  // the digits of a number in base paint_count + 1: index 0 is all
  // kUnpainted, index 1 differs only in the current tile being kPaint1.
  void SetActionAt(std::size_t index, Action action);

  // Two tables are equal when they have the same shape and map every
  // combination to the same action.
  friend bool operator==(const RuleTable&, const RuleTable&) = default;

 private:
  // Returns the index of `sensed`, as described at SetActionAt().
  std::size_t IndexOf(const std::vector<Color>& sensed) const;

  int paint_count_;
  int history_length_;
  // std::vector, unlike std::array, gets its size at run time.
  std::vector<Action> actions_;
};

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_RULE_TABLE_H_
