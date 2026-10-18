#include "color.h"

#include <gtest/gtest.h>

namespace turtle_art {
namespace {

TEST(ColorTest, NextPaintAfterPaint1IsPaint2) {
  EXPECT_EQ(NextPaint(Color::kPaint1, 4), Color::kPaint2);
}

TEST(ColorTest, NextPaintAfterLastPaintWrapsToPaint1) {
  EXPECT_EQ(NextPaint(Color::kPaint4, 4), Color::kPaint1);
}

TEST(ColorTest, NextPaintWithMorePaintsGoesPastPaint4) {
  EXPECT_EQ(NextPaint(Color::kPaint4, 8), Color::kPaint5);
}

TEST(ColorTest, NextPaintWithTwoPaintsAlternates) {
  EXPECT_EQ(NextPaint(Color::kPaint2, 2), Color::kPaint1);
}

TEST(ColorTest, NextPaintWithOnePaintStaysOnIt) {
  EXPECT_EQ(NextPaint(Color::kPaint1, 1), Color::kPaint1);
}

TEST(ColorTest, NextPaintAfterUnpaintedIsPaint1) {
  EXPECT_EQ(NextPaint(Color::kUnpainted, 4), Color::kPaint1);
}

}  // namespace
}  // namespace turtle_art
