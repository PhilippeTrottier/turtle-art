#include "color.h"

#include <gtest/gtest.h>

namespace turtle_art {
namespace {

TEST(ColorTest, NextPaintAfterPaint1IsPaint2) {
  EXPECT_EQ(NextPaint(Color::kPaint1), Color::kPaint2);
}

TEST(ColorTest, NextPaintAfterPaint4WrapsToPaint1) {
  EXPECT_EQ(NextPaint(Color::kPaint4), Color::kPaint1);
}

TEST(ColorTest, NextPaintAfterUnpaintedIsPaint1) {
  EXPECT_EQ(NextPaint(Color::kUnpainted), Color::kPaint1);
}

}  // namespace
}  // namespace turtle_art
