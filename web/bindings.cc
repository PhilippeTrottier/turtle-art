// Exposes turtle_art_lib to JavaScript, for the web explorer (see README).
//
// Emscripten compiles this file and the library to WebAssembly. Embind, its
// C++ binding library, generates the JavaScript glue: each
// emscripten::function below becomes a function on the module object that
// JavaScript gets from createTurtleArt(), and value_object / enum_ describe
// how C++ structs and enums map to plain JavaScript objects and values.

#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <cstdint>
#include <string>
#include <vector>

#include "art.h"
#include "grid.h"
#include "grid_stats.h"
#include "layout.h"
#include "palette.h"
#include "tile.h"
#include "turtle.h"

namespace {

using emscripten::val;

// Runs the turtle and returns a JavaScript object
//   {colors: Uint8Array, entropy: number}
// where colors[i] is the Color value of the i-th tile in Grid::Tiles() order,
// and entropy is the Entropy() of the color counts.
val Generate(const turtle_art::ArtSettings& settings) {
  const turtle_art::Grid grid = turtle_art::RunArt(settings);
  std::vector<std::uint8_t> colors;
  for (const turtle_art::Tile hex : grid.Tiles()) {
    colors.push_back(static_cast<std::uint8_t>(grid.ColorAt(hex)));
  }
  val result = val::object();
  // typed_memory_view lets JavaScript see `colors` without copying it. The
  // view dies with `colors` when this function returns, so we copy it into
  // a new Uint8Array that JavaScript owns.
  result.set("colors", val::global("Uint8Array")
                           .new_(emscripten::typed_memory_view(colors.size(),
                                                               colors.data())));
  result.set("entropy", turtle_art::Entropy(turtle_art::ColorCounts(grid)));
  return result;
}

// Returns where the tiles of the grid `settings` describe go, as a
// JavaScript object
//   {centers: Float32Array, outline: Float32Array, bounds: {x, y, width,
//    height}}
// where centers holds x, y for each tile in Grid::Tiles() order, outline
// holds x, y for each corner of one tile centered on {0, 0}, and bounds is
// the picture's box (see layout.h). The page draws with these alone.
val Layout(const turtle_art::ArtSettings& settings) {
  const turtle_art::Grid grid = turtle_art::MakeGrid(settings);
  std::vector<float> centers;
  for (const turtle_art::Tile tile : grid.Tiles()) {
    const turtle_art::Point center = turtle_art::TileCenter(grid, tile);
    centers.push_back(static_cast<float>(center.x));
    centers.push_back(static_cast<float>(center.y));
  }
  std::vector<float> outline;
  for (const turtle_art::Point corner : turtle_art::TileOutline(grid)) {
    outline.push_back(static_cast<float>(corner.x));
    outline.push_back(static_cast<float>(corner.y));
  }
  const turtle_art::Box box = turtle_art::PictureBounds(grid);
  val bounds = val::object();
  bounds.set("x", box.x);
  bounds.set("y", box.y);
  bounds.set("width", box.width);
  bounds.set("height", box.height);
  val result = val::object();
  // As in Generate(): copy each vector into an array JavaScript owns.
  result.set("centers", val::global("Float32Array")
                            .new_(emscripten::typed_memory_view(
                                centers.size(), centers.data())));
  result.set("outline", val::global("Float32Array")
                            .new_(emscripten::typed_memory_view(
                                outline.size(), outline.data())));
  result.set("bounds", bounds);
  return result;
}

// Returns x, y offsets, in picture units, of `rings` rings of copies
// around the picture `settings` describes, as a Float32Array: where a
// wrapping turtle continues (see WrapCopyOffsets in layout.h).
val WrapCopies(const turtle_art::ArtSettings& settings, int rings) {
  const turtle_art::Grid grid = turtle_art::MakeGrid(settings);
  std::vector<float> offsets;
  for (const turtle_art::Point offset :
       turtle_art::WrapCopyOffsets(grid, rings)) {
    offsets.push_back(static_cast<float>(offset.x));
    offsets.push_back(static_cast<float>(offset.y));
  }
  return val::global("Float32Array")
      .new_(emscripten::typed_memory_view(offsets.size(), offsets.data()));
}

// Returns how many rings of copies fill a screen `width` x `height`, in
// picture units, centered on the picture `settings` describes (see
// WrapRingsToFill in layout.h).
int WrapRingsToFill(const turtle_art::ArtSettings& settings, double width,
                    double height) {
  return turtle_art::WrapRingsToFill(turtle_art::MakeGrid(settings), width,
                                     height);
}

// Returns the picture for `settings` as SVG text (for downloading).
std::string Svg(const turtle_art::ArtSettings& settings) {
  return turtle_art::MakeArt(settings);
}

// Returns the palette as a JavaScript array of CSS colors, indexed by Color.
val Palette() {
  val colors = val::array();
  for (const std::string_view color : turtle_art::kPalette) {
    colors.call<void>("push", std::string(color));
  }
  return colors;
}

}  // namespace

// EMSCRIPTEN_BINDINGS registers everything below when the module loads.
EMSCRIPTEN_BINDINGS(turtle_art) {
  emscripten::enum_<turtle_art::EdgeMode>("EdgeMode")
      .value("stop", turtle_art::EdgeMode::kStop)
      .value("wrap", turtle_art::EdgeMode::kWrap);
  emscripten::enum_<turtle_art::GridShape>("GridShape")
      .value("hexagon", turtle_art::GridShape::kHexagon)
      .value("rectangle", turtle_art::GridShape::kRectangle);
  emscripten::enum_<turtle_art::PaintMode>("PaintMode")
      .value("turtle", turtle_art::PaintMode::kTurtleColor)
      .value("cycle", turtle_art::PaintMode::kCycle);
  // JavaScript passes ArtSettings as a plain object with these fields; all
  // of them must be present.
  emscripten::value_object<turtle_art::ArtSettings>("ArtSettings")
      .field("seed", &turtle_art::ArtSettings::seed)
      .field("steps", &turtle_art::ArtSettings::steps)
      .field("shape", &turtle_art::ArtSettings::shape)
      .field("radius", &turtle_art::ArtSettings::radius)
      .field("width", &turtle_art::ArtSettings::width)
      .field("height", &turtle_art::ArtSettings::height)
      .field("edgeMode", &turtle_art::ArtSettings::edge_mode)
      .field("paintCount", &turtle_art::ArtSettings::paint_count)
      .field("historyLength", &turtle_art::ArtSettings::history_length)
      .field("paintMode", &turtle_art::ArtSettings::paint_mode)
      .field("hz", &turtle_art::ArtSettings::hz);
  emscripten::function("generate", &Generate);
  emscripten::function("layout", &Layout);
  emscripten::function("wrapCopies", &WrapCopies);
  emscripten::function("wrapRingsToFill", &WrapRingsToFill);
  emscripten::function("svg", &Svg);
  emscripten::function("palette", &Palette);
}
