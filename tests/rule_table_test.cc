#include "rule_table.h"

#include <gtest/gtest.h>

#include <array>

#include "action.h"
#include "color.h"

namespace turtle_art {
namespace {

TEST(RuleTableTest, DefaultShapeIsFourPaintsAndThreeTiles) {
  const RuleTable rules;
  EXPECT_EQ(rules.paint_count(), 4);
  EXPECT_EQ(rules.history_length(), 3);
}

TEST(RuleTableTest, SizeIsPaintCountPlusOneToThePowerOfHistory) {
  EXPECT_EQ(RuleTable(4, 3).size(), 125U);
  EXPECT_EQ(RuleTable(2, 1).size(), 3U);
  EXPECT_EQ(RuleTable(8, 2).size(), 81U);
}

TEST(RuleTableTest, NewTableKeepsHeading) {
  const RuleTable rules;
  EXPECT_EQ(rules.ActionFor(
                std::array{Color::kPaint1, Color::kPaint2, Color::kPaint3}),
            Action::kKeep);
}

TEST(RuleTableTest, SetActionIsReturnedByActionFor) {
  RuleTable rules;
  rules.SetAction(std::array{Color::kPaint1, Color::kPaint2, Color::kPaint3},
                  Action::kTurnLeft);
  EXPECT_EQ(rules.ActionFor(
                std::array{Color::kPaint1, Color::kPaint2, Color::kPaint3}),
            Action::kTurnLeft);
}

TEST(RuleTableTest, SetActionLeavesSameColorsInOtherOrderUnchanged) {
  RuleTable rules;
  rules.SetAction(std::array{Color::kPaint1, Color::kPaint2, Color::kPaint3},
                  Action::kTurnLeft);
  EXPECT_EQ(rules.ActionFor(
                std::array{Color::kPaint3, Color::kPaint2, Color::kPaint1}),
            Action::kKeep);
}

TEST(RuleTableTest, SetActionWorksForLastCombination) {
  RuleTable rules;
  rules.SetAction(std::array{Color::kPaint4, Color::kPaint4, Color::kPaint4},
                  Action::kChangeColor);
  EXPECT_EQ(rules.ActionFor(
                std::array{Color::kPaint4, Color::kPaint4, Color::kPaint4}),
            Action::kChangeColor);
}

TEST(RuleTableTest, SetActionAtZeroIsAllUnpainted) {
  RuleTable rules;
  rules.SetActionAt(0, Action::kTurnRight);
  EXPECT_EQ(rules.ActionFor(std::array{Color::kUnpainted, Color::kUnpainted,
                                       Color::kUnpainted}),
            Action::kTurnRight);
}

TEST(RuleTableTest, SetActionAtOneChangesOnlyCurrentTile) {
  RuleTable rules;
  rules.SetActionAt(1, Action::kTurnRight);
  EXPECT_EQ(rules.ActionFor(std::array{Color::kUnpainted, Color::kUnpainted,
                                       Color::kPaint1}),
            Action::kTurnRight);
}

TEST(RuleTableTest, OneTileHistorySensesCurrentTileOnly) {
  RuleTable rules(4, 1);
  rules.SetAction(std::array{Color::kPaint2}, Action::kTurnLeft);
  EXPECT_EQ(rules.ActionFor(std::array{Color::kPaint2}), Action::kTurnLeft);
}

TEST(RuleTableTest, LongHistoryWithManyPaints) {
  RuleTable rules(8, 5);
  rules.SetAction(std::array{Color::kPaint8, Color::kPaint1, Color::kUnpainted,
                             Color::kPaint5, Color::kPaint8},
                  Action::kChangeColor);
  EXPECT_EQ(rules.ActionFor(std::array{Color::kPaint8, Color::kPaint1,
                                       Color::kUnpainted, Color::kPaint5,
                                       Color::kPaint8}),
            Action::kChangeColor);
}

TEST(RuleTableTest, TablesWithDifferentActionsAreNotEqual) {
  RuleTable changed;
  changed.SetActionAt(0, Action::kTurnRight);
  EXPECT_NE(changed, RuleTable());
}

TEST(RuleTableTest, TablesWithDifferentShapesAreNotEqual) {
  EXPECT_NE(RuleTable(4, 3), RuleTable(4, 2));
}

}  // namespace
}  // namespace turtle_art
