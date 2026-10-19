#include "turtle.h"

#include <gtest/gtest.h>

#include "action.h"
#include "color.h"
#include "direction.h"
#include "grid.h"
#include "rule_table.h"
#include "tile.h"

namespace turtle_art {
namespace {

// A new RuleTable maps everything to kKeep, so tests only set the entries
// they care about. A new turtle's first step senses only unpainted tiles.

TEST(TurtleTest, StartsAtCenterHeadingEastWithPaint1) {
  const Turtle turtle;
  EXPECT_EQ(turtle.position(), Tile{});
  EXPECT_EQ(turtle.heading(), Direction::kEast);
  EXPECT_EQ(turtle.color(), Color::kPaint1);
}

TEST(TurtleTest, StepPaintsTheTileItLeaves) {
  Grid grid(3);
  Turtle turtle;
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint1);
}

TEST(TurtleTest, StepWithKeepMovesForward) {
  Grid grid(3);
  Turtle turtle;
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(turtle.position(), (Tile{.q = 1, .r = 0}));
}

TEST(TurtleTest, StepWithTurnLeftTurnsThenMoves) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kUnpainted},
                  Action::kTurnLeft);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kNorthEast);
  EXPECT_EQ(turtle.position(), (Tile{.q = 1, .r = -1}));
}

TEST(TurtleTest, StepWithTurnRightTurnsThenMoves) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kUnpainted},
                  Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
  EXPECT_EQ(turtle.position(), (Tile{.q = 0, .r = 1}));
}

TEST(TurtleTest, StepWithChangeColorPaintsWithNextColor) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kUnpainted},
                  Action::kChangeColor);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint2);
  EXPECT_EQ(turtle.heading(), Direction::kEast);
}

TEST(TurtleTest, StepSensesColorOfCurrentTile) {
  Grid grid(3);
  grid.Paint(Tile{}, Color::kPaint3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kPaint3},
                  Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
}

TEST(TurtleTest, SecondStepSensesPreviousTile) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kPaint1, Color::kUnpainted},
                  Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
}

TEST(TurtleTest, ThirdStepSensesTileTwoBack) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kPaint1, Color::kPaint1, Color::kUnpainted},
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
  EXPECT_EQ(turtle.position(), Tile{});
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint1);
}

TEST(TurtleTest, WalkTakesMaxSteps) {
  Grid grid(5);
  Turtle turtle;
  EXPECT_EQ(Walk(turtle, grid, RuleTable(), 3), 3);
  EXPECT_EQ(turtle.position(), (Tile{.q = 3, .r = 0}));
}

TEST(TurtleTest, WalkStopsAtEdge) {
  Grid grid(1);
  Turtle turtle;
  EXPECT_EQ(Walk(turtle, grid, RuleTable(), 10), 2);
  EXPECT_EQ(turtle.position(), (Tile{.q = 1, .r = 0}));
}

TEST(TurtleTest, StepAtWrappingEdgeComesBackOnOppositeSide) {
  Grid grid(0, EdgeMode::kWrap);
  Turtle turtle;
  EXPECT_TRUE(turtle.Step(grid, RuleTable()));
  EXPECT_EQ(turtle.position(), Tile{});
}

TEST(TurtleTest, WalkOnWrappingGridTakesAllSteps) {
  Grid grid(1, EdgeMode::kWrap);
  Turtle turtle;
  EXPECT_EQ(Walk(turtle, grid, RuleTable(), 10), 10);
}

TEST(TurtleTest, OneTileHistorySensesOnlyCurrentTile) {
  Grid grid(3);
  RuleTable rules(4, 1);
  rules.SetAction({Color::kUnpainted}, Action::kTurnLeft);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kNorthEast);
}

TEST(TurtleTest, FourTileHistorySensesThreePreviousTiles) {
  Grid grid(5);
  RuleTable rules(4, 4);
  rules.SetAction(
      {Color::kPaint1, Color::kPaint1, Color::kPaint1, Color::kUnpainted},
      Action::kTurnRight);
  Turtle turtle;
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.heading(), Direction::kSouthEast);
}

TEST(TurtleTest, ChangeColorWithOnePaintKeepsPaint1) {
  Grid grid(3);
  RuleTable rules(1, 3);
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kUnpainted},
                  Action::kChangeColor);
  Turtle turtle;
  turtle.Step(grid, rules);
  EXPECT_EQ(turtle.color(), Color::kPaint1);
}

TEST(TurtleTest, CycleModePaintsUnpaintedTileWithPaint1) {
  Grid grid(3);
  Turtle turtle(PaintMode::kCycle);
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint1);
}

TEST(TurtleTest, CycleModeAdvancesPaintedTileToNextColor) {
  Grid grid(3);
  grid.Paint(Tile{}, Color::kPaint2);
  Turtle turtle(PaintMode::kCycle);
  turtle.Step(grid, RuleTable());
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint3);
}

TEST(TurtleTest, CycleModeWrapsLastPaintToPaint1) {
  Grid grid(3);
  grid.Paint(Tile{}, Color::kPaint2);
  Turtle turtle(PaintMode::kCycle);
  turtle.Step(grid, RuleTable(2, 3));
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint1);
}

TEST(TurtleTest, CycleModeIgnoresChangeColor) {
  Grid grid(3);
  RuleTable rules;
  rules.SetAction({Color::kUnpainted, Color::kUnpainted, Color::kUnpainted},
                  Action::kChangeColor);
  Turtle turtle(PaintMode::kCycle);
  turtle.Step(grid, rules);
  EXPECT_EQ(grid.ColorAt(Tile{}), Color::kPaint1);
}

}  // namespace
}  // namespace turtle_art
