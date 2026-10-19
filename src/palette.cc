#include "palette.h"

#include <format>
#include <string>

namespace turtle_art {

std::string CssColor(Rgb rgb) {
  // {:02x} prints a number as 2 lowercase hexadecimal digits, e.g. 0x0a as
  // "0a".
  return std::format("#{:02x}{:02x}{:02x}", rgb.red, rgb.green, rgb.blue);
}

}  // namespace turtle_art
