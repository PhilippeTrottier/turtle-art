#include "color.h"

namespace turtle_art {

Color NextPaint(Color color) {
  // Palette colors have values 1 to 4 (kUnpainted is 0). Taking the value
  // modulo 4 and adding 1 maps 1->2, 2->3, 3->4, 4->1, and also 0->1.
  constexpr int kPaintCount = kColorCount - 1;
  return static_cast<Color>((static_cast<int>(color) % kPaintCount) + 1);
}

}  // namespace turtle_art
