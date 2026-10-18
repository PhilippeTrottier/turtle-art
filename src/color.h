#ifndef TURTLE_ART_SRC_COLOR_H_
#define TURTLE_ART_SRC_COLOR_H_

namespace turtle_art {

// The color of a tile: unpainted, or one of the palette colors the turtle
// paints with. A run uses the first N palette colors (N = its paint count).
// Which actual color (RGB) each one is shown as is up to the output, not to
// this type.
enum class Color {
  kUnpainted,
  kPaint1,
  kPaint2,
  kPaint3,
  kPaint4,
  kPaint5,
  kPaint6,
  kPaint7,
  kPaint8,
};

// The largest paint count: the number of Color values other than kUnpainted.
//
// `inline` on a variable (C++17) lets a header define it: every file that
// includes the header shares one variable instead of each getting a copy.
inline constexpr int kMaxPaintCount = 8;

// Returns the palette color after `color` among the first `paint_count`
// paints, wrapping around: with 4 paints, kPaint1 -> kPaint2 -> kPaint3 ->
// kPaint4 -> kPaint1. kUnpainted is not a palette color; it returns kPaint1.
// `paint_count` must be in [1, kMaxPaintCount].
Color NextPaint(Color color, int paint_count);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_COLOR_H_
