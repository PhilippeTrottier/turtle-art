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
#include "hex.h"
#include "palette.h"
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
  for (const turtle_art::Hex hex : grid.Tiles()) {
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
  emscripten::enum_<turtle_art::PaintMode>("PaintMode")
      .value("turtle", turtle_art::PaintMode::kTurtleColor)
      .value("cycle", turtle_art::PaintMode::kCycle);
  // JavaScript passes ArtSettings as a plain object with these fields; all
  // of them must be present.
  emscripten::value_object<turtle_art::ArtSettings>("ArtSettings")
      .field("seed", &turtle_art::ArtSettings::seed)
      .field("steps", &turtle_art::ArtSettings::steps)
      .field("radius", &turtle_art::ArtSettings::radius)
      .field("edgeMode", &turtle_art::ArtSettings::edge_mode)
      .field("paintCount", &turtle_art::ArtSettings::paint_count)
      .field("historyLength", &turtle_art::ArtSettings::history_length)
      .field("paintMode", &turtle_art::ArtSettings::paint_mode);
  emscripten::function("generate", &Generate);
  emscripten::function("svg", &Svg);
  emscripten::function("palette", &Palette);
}
