#include "turtle.h"

#include <gtest/gtest.h>

#include "action.h"
#include "color.h"
#include "direction.h"
#include "grid.h"
#include "hex.h"
#include "rule_table.h"

namespace turtle_art {
namespace {

// A new RuleTable maps everything to kKeep, so tests only set the entries
// they care about. A new turtle's first step senses three unpainted tiles,
// which is the default SensedColors{}.

TEST(TurtleTest, StartsAtCenterHeadingEastWithPaint1) {
  const Turtle turtle;
  EXPECT_EQ(turtle.position(), Hex{});
  EXPECT_EQ(turtle.heading(), Direction::kEast);
  EXPECT_EQ(turtle.color(), Color::kPaint1);
}

TEST(TurtleTest, StepPaintsTheTileItLeaves) {
  Grid grid(3);
  Turtle turtle;
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(grid.ColorAt(Hex{}), Color::kPaint1);
}

TEST(TurtleTest, StepWithKeepMovesForward) {
  Grid grid(3);
  Turtle turtle;
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(turtle.position(), (Hex{.q = 1, .r = 0}));
}

TEST(TurtleTest, StepWithTurnLeftTurnsThenMoves) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({}, Action::kTurnLeft);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kNorthEast);
  EXPECT_EQ(turtle.position(), (Hex{.q = 1, .r = -1}));
}

TEST(TurtleTest, StepWithTurnRightTurnsThenMoves) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({}, Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
  EXPECT_EQ(turtle.position(), (Hex{.q = 0, .r = 1}));
}

TEST(TurtleTest, StepWithChangeColorPaintsWithNextColor) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({}, Action::kChangeColor);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(grid.ColorAt(Hex{}), Color::kPaint2);
  EXPECT_EQ(turtle.heading(), Direction::kEast);
}

TEST(TurtleTest, StepSensesColorOfCurrentTile) {
  Grid grid(3);
  grid.Paint(Hex{}, Color::kPaint3);
  RuleTable rules;
  rules.SetAction({.current = Color::kPaint3}, Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
}

TEST(TurtleTest, SecondStepSensesPreviousTile) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({.one_back = Color::kPaint1}, Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
}

TEST(TurtleTest, ThirdStepSensesTileTwoBack) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({.two_back = Color::kPaint1, .one_back = Color::kPaint1},
                  Action::kTurnLeft);
  Turtle turtle;
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kNorthEast);
}

TEST(TurtleTest, StepAtEdgePaintsButStaysPut) {
  Grid grid(0);
  Turtle turtle;
  EXPECT_FALSE(turtle.Step(grid, RuleTable()));
  EXPECT_EQ(turtle.position(), Hex{});
  EXPECT_EQ(grid.ColorAt(Hex{}), Color::kPaint1);
}

TEST(TurtleTest, WalkTakesMaxSteps) {
  Grid grid(5);
  Turtle turtle;
  EXPECT_EQ(Walk(turtle, grid, RuleTable(), 3), 3);
  EXPECT_EQ(turtle.position(), (Hex{.q = 3, .r = 0}));
}

TEST(TurtleTest, WalkStopsAtEdge) {
  Grid grid(1);
  Turtle turtle;
  EXPECT_EQ(Walk(turtle, grid, RuleTable(), 10), 2);
  EXPECT_EQ(turtle.position(), (Hex{.q = 1, .r = 0}));
}

}  // namespace
}  // namespace turtle_art
