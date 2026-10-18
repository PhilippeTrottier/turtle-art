#include "random_rules.h"

#include <gtest/gtest.h>

#include "action.h"
#include "color.h"
#include "rule_table.h"

namespace turtle_art {
namespace {

TEST(RandomRulesTest, SameSeedGivesSameTable) {
  EXPECT_EQ(RandomRuleTable(7), RandomRuleTable(7));
}

TEST(RandomRulesTest, DifferentSeedsGiveDifferentTables) {
  EXPECT_NE(RandomRuleTable(1), RandomRuleTable(2));
}

// A fixed, known value. If this fails on one platform only, the generator is
// not producing the same numbers everywhere and pictures would differ.
TEST(RandomRulesTest, SeedOneStartsWithTurnLeft) {
  EXPECT_EQ(RandomRuleTable(1).ActionFor(
                {Color::kUnpainted, Color::kUnpainted, Color::kUnpainted}),
            Action::kTurnLeft);
}

TEST(RandomRulesTest, TableHasRequestedShape) {
  const RuleTable rules = RandomRuleTable(1, 6, 2);
  EXPECT_EQ(rules.paint_count(), 6);
  EXPECT_EQ(rules.history_length(), 2);
}

TEST(RandomRulesTest, SameSeedWithOtherShapeGivesOtherTable) {
  EXPECT_NE(RandomRuleTable(7, 4, 3), RandomRuleTable(7, 4, 2));
}

}  // namespace
}  // namespace turtle_art
