// The turtle-art command-line program. It only parses options and writes the
// file; everything else lives in turtle_art_lib, where it is tested.

#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>

#include "art.h"
#include "color.h"
#include "grid.h"
#include "rule_table.h"
#include "turtle.h"

// clang-tidy warns that exceptions may escape main. Apart from the CLI11
// parse errors caught below, the only ones possible are failures such as
// running out of memory, which end the program: the policy for code without
// exceptions. NOLINT silences that one check on this line only.
int main(int argc, char** argv) {  // NOLINT(bugprone-exception-escape)
  turtle_art::ArtSettings settings;
  std::string out_path = "turtle-art.svg";

  CLI::App app("Procedural art made by a turtle walking on a grid.");
  // Each option writes straight into the variable it is given.
  // capture_default_str() shows the variable's starting value in --help.
  app.add_option("--seed", settings.seed, "Seed for the rule table")
      ->capture_default_str();
  app.add_option("--steps", settings.steps, "Maximum number of steps")
      ->capture_default_str()
      ->check(CLI::NonNegativeNumber);
  // A hexagon grid by default; --width and --height (always together, and
  // never with --radius) make a rectangle of square tiles instead.
  CLI::Option* radius = app.add_option("--radius", settings.radius,
                                       "Hexagon grid: radius in tiles")
                            ->capture_default_str()
                            ->check(CLI::NonNegativeNumber);
  CLI::Option* width =
      app.add_option("--width", settings.width,
                     "Rectangle grid of square tiles: columns (with --height)")
          ->check(CLI::Range(1, std::numeric_limits<int>::max()))
          ->excludes(radius);
  app.add_option("--height", settings.height,
                 "Rectangle grid of square tiles: rows (with --width)")
      ->check(CLI::Range(1, std::numeric_limits<int>::max()))
      ->excludes(radius)
      ->needs(width);
  width->needs(app.get_option("--height"));
  // CheckedTransformer turns each word into its enum value and rejects any
  // other word.
  const std::map<std::string, turtle_art::EdgeMode> edge_modes = {
      {"stop", turtle_art::EdgeMode::kStop},
      {"wrap", turtle_art::EdgeMode::kWrap},
  };
  app.add_option("--edge", settings.edge_mode,
                 "At the grid's edge: stop the walk, or wrap around")
      ->transform(CLI::CheckedTransformer(edge_modes))
      ->default_str("wrap");
  const std::map<std::string, turtle_art::PaintMode> paint_modes = {
      {"turtle", turtle_art::PaintMode::kTurtleColor},
      {"cycle", turtle_art::PaintMode::kCycle},
  };
  app.add_option("--paint", settings.paint_mode,
                 "Paint tiles with the turtle's color, or cycle each tile to "
                 "its next color (Langton's ant style)")
      ->transform(CLI::CheckedTransformer(paint_modes))
      ->default_str("turtle");
  app.add_option("--colors", settings.paint_count,
                 "Number of palette colors the turtle paints with")
      ->capture_default_str()
      ->check(CLI::Range(1, turtle_art::kMaxPaintCount));
  app.add_option("--history", settings.history_length,
                 "Number of recent tiles (current one included) whose colors "
                 "decide each move")
      ->capture_default_str()
      ->check(CLI::Range(1, turtle_art::kMaxHistoryLength));
  app.add_option("--out", out_path, "Path of the SVG file to write")
      ->capture_default_str();

  // CLI11 reports bad options by throwing. This macro catches that, prints
  // the error (or the --help text) and returns the right exit code from
  // main. It is the one place where our code meets an exception.
  CLI11_PARSE(app, argc, argv);
  // An Option converts to true when it was given on the command line.
  if (*width) {
    settings.shape = turtle_art::GridShape::kRectangle;
  }

  // std::ofstream closes the file automatically when `out` goes out of
  // scope (RAII: the object owns the resource and releases it when it dies).
  std::ofstream out(out_path);
  out << turtle_art::MakeArt(settings);
  // A stream converts to false once any operation on it has failed.
  if (!out) {
    std::cerr << "error: could not write " << out_path << "\n";
    return 1;
  }
  std::cout << "wrote " << out_path << "\n";
  return 0;
}
