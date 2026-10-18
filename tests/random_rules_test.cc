#include "random_rules.h"

#include <gtest/gtest.h>

#include "action.h"

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
  EXPECT_EQ(RandomRuleTable(1).ActionFor({}), Action::kTurnLeft);
}

}  // namespace
}  // namespace turtle_art
