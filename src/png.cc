#include "png.h"

#include <lodepng.h>

#include <cassert>
#include <cstdint>
#include <vector>

#include "grid.h"
#include "palette.h"
#include "tile.h"

namespace turtle_art {

std::vector<std::uint8_t> ToPng(const Grid& grid) {
  assert(grid.shape() == GridShape::kRectangle);
  // One byte per pixel, the tile's Color value, which is its palette index.
  // Grid::Tiles() runs row by row from the top left, the order PNG wants.
  std::vector<std::uint8_t> pixels;
  for (const Tile tile : grid.Tiles()) {
    pixels.push_back(static_cast<std::uint8_t>(grid.ColorAt(tile)));
  }

  // lodepng (https://lodev.org/lodepng/) encodes PNG. `raw` describes the
  // pixels we hand it, `png` the file it writes: both are palette images
  // with kPalette's colors; the file packs 4 bits per pixel.
  lodepng::State state;
  for (const Rgb color : kPalette) {
    lodepng_palette_add(&state.info_raw, color.red, color.green, color.blue,
                        255);
    lodepng_palette_add(&state.info_png.color, color.red, color.green,
                        color.blue, 255);
  }
  state.info_raw.colortype = LCT_PALETTE;
  state.info_raw.bitdepth = 8;
  state.info_png.color.colortype = LCT_PALETTE;
  state.info_png.color.bitdepth = 4;
  // Keep exactly this format, rather than letting lodepng pick one.
  state.encoder.auto_convert = 0;

  std::vector<std::uint8_t> png;
  // [[maybe_unused]] (C++17): `error` is only read by assert(), which
  // compiles to nothing in Release builds; this keeps the compiler quiet.
  [[maybe_unused]] const unsigned error =
      lodepng::encode(png, pixels, static_cast<unsigned>(grid.width()),
                      static_cast<unsigned>(grid.height()), state);
  // Encoding valid pixels in memory only fails when out of memory, which
  // ends the program anyway (see "Style notes" in the README).
  assert(error == 0);
  return png;
}

}  // namespace turtle_art
