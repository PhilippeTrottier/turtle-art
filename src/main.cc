// The turtle-art command-line program. It only parses options and writes the
// file; everything else lives in turtle_art_lib, where it is tested.

#include <CLI/CLI.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "art.h"

// clang-tidy warns that exceptions may escape main. Apart from the CLI11
// parse errors caught below, the only ones possible are failures such as
// running out of memory, which end the program: the policy for code without
// exceptions. NOLINT silences that one check on this line only.
int main(int argc, char** argv) {  // NOLINT(bugprone-exception-escape)
  turtle_art::ArtSettings settings;
  std::string out_path = "turtle-art.svg";

  CLI::App app("Procedural art made by a turtle walking on a hexagonal grid.");
  // Each option writes straight into the variable it is given.
  // capture_default_str() shows the variable's starting value in --help.
  app.add_option("--seed", settings.seed, "Seed for the rule table")
      ->capture_default_str();
  app.add_option("--steps", settings.steps, "Maximum number of steps")
      ->capture_default_str()
      ->check(CLI::NonNegativeNumber);
  app.add_option("--radius", settings.radius, "Grid radius in tiles")
      ->capture_default_str()
      ->check(CLI::NonNegativeNumber);
  app.add_option("--out", out_path, "Path of the SVG file to write")
      ->capture_default_str();

  // CLI11 reports bad options by throwing. This macro catches that, prints
  // the error (or the --help text) and returns the right exit code from
  // main. It is the one place where our code meets an exception.
  CLI11_PARSE(app, argc, argv);

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
