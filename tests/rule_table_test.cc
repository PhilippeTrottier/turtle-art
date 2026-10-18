#include "rule_table.h"

#include <gtest/gtest.h>

#include "action.h"
#include "color.h"

namespace turtle_art {
namespace {

TEST(RuleTableTest, NewTableKeepsHeading) {
  const RuleTable rules;
  EXPECT_EQ(rules.ActionFor({.two_back = Color::kPaint1,
                             .one_back = Color::kPaint2,
                             .current = Color::kPaint3}),
            Action::kKeep);
}

TEST(RuleTableTest, SetActionIsReturnedByActionFor) {
  RuleTable rules;
  rules.SetAction({.two_back = Color::kPaint1,
                   .one_back = Color::kPaint2,
                   .current = Color::kPaint3},
                  Action::kTurnLeft);
  EXPECT_EQ(rules.ActionFor({.two_back = Color::kPaint1,
                             .one_back = Color::kPaint2,
                             .current = Color::kPaint3}),
            Action::kTurnLeft);
}

TEST(RuleTableTest, SetActionLeavesSameColorsInOtherOrderUnchanged) {
  RuleTable rules;
  rules.SetAction({.two_back = Color::kPaint1,
                   .one_back = Color::kPaint2,
                   .current = Color::kPaint3},
                  Action::kTurnLeft);
  EXPECT_EQ(rules.ActionFor({.two_back = Color::kPaint3,
                             .one_back = Color::kPaint2,
                             .current = Color::kPaint1}),
            Action::kKeep);
}

TEST(RuleTableTest, SetActionWorksForLastCombination) {
  RuleTable rules;
  rules.SetAction({.two_back = Color::kPaint4,
                   .one_back = Color::kPaint4,
                   .current = Color::kPaint4},
                  Action::kChangeColor);
  EXPECT_EQ(rules.ActionFor({.two_back = Color::kPaint4,
                             .one_back = Color::kPaint4,
                             .current = Color::kPaint4}),
            Action::kChangeColor);
}

TEST(RuleTableTest, TablesWithDifferentActionsAreNotEqual) {
  RuleTable changed;
  changed.SetAction({}, Action::kTurnRight);
  EXPECT_NE(changed, RuleTable());
}

}  // namespace
}  // namespace turtle_art
