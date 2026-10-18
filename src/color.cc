#include "color.h"

#include <cassert>

namespace turtle_art {

Color NextPaint(Color color, int paint_count) {
  assert(paint_count >= 1 && paint_count <= kMaxPaintCount);
  // Palette colors have values 1 to paint_count (kUnpainted is 0). Taking
  // the value modulo paint_count and adding 1 maps each to the next, the
  // last back to 1, and also 0 to 1.
  return static_cast<Color>((static_cast<int>(color) % paint_count) + 1);
}

}  // namespace turtle_art
