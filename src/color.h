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

}  // namespace turtle_art

#endif  // TURTLE_ART_SRC_COLOR_H_
