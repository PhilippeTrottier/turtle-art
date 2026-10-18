#ifndef TURTLE_ART_SRC_COLOR_H_
#define TURTLE_ART_SRC_COLOR_H_

namespace turtle_art {

// The color of a tile: unpainted, or one of the four palette colors the
// turtle paints with. Which actual color (RGB) each one is shown as is up to
// the output, not to this type.
enum class Color {
  kUnpainted,
  kPaint1,
  kPaint2,
  kPaint3,
  kPaint4,
};

// The number of Color values, unpainted included.
//
// `inline` on a variable (C++17) lets a header define it: every file that
// includes the header shares one variable instead of each getting a copy.
inline constexpr int kColorCount = 5;

// Returns the palette color after `color`: kPaint1 -> kPaint2 -> kPaint3 ->
// kPaint4 -> kPaint1. kUnpainted is not a palette color; it returns kPaint1.
Color NextPaint(Color color);

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_COLOR_H_
