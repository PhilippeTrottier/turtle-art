#include "direction.h"

#include <gtest/gtest.h>

namespace turtle_art {
namespace {

TEST(DirectionTest, TurnLeftFromEastIsNorthEast) {
  EXPECT_EQ(TurnLeft(Direction::kEast), Direction::kNorthEast);
}

TEST(DirectionTest, TurnLeftFromSouthEastWrapsToEast) {
  EXPECT_EQ(TurnLeft(Direction::kSouthEast), Direction::kEast);
}

TEST(DirectionTest, TurnRightFromNorthEastIsEast) {
  EXPECT_EQ(TurnRight(Direction::kNorthEast), Direction::kEast);
}

TEST(DirectionTest, TurnRightFromEastWrapsToSouthEast) {
  EXPECT_EQ(TurnRight(Direction::kEast), Direction::kSouthEast);
}

}  // namespace
}  // namespace turtle_art
