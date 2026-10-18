# Top-level build script. Configure with a preset, e.g. `cmake --preset msvc`
# (see CMakePresets.json and the README).
cmake_minimum_required(VERSION 3.25)

project(turtle-art
  VERSION 0.1.0
  DESCRIPTION "Procedural art made by a turtle walking on a hexagonal grid"
  LANGUAGES CXX)

# Standard C++ only (e.g. -std=c++20, not -std=gnu++20). Set globally because
# target properties, unlike compile features, do not pass through linking.
set(CMAKE_CXX_EXTENSIONS OFF)

# Settings shared by every target. An INTERFACE library builds nothing itself;
# linking to it hands its compile features and options to the linking target.
add_library(turtle_art_options INTERFACE)
target_compile_features(turtle_art_options INTERFACE cxx_std_20)
target_compile_options(turtle_art_options INTERFACE
  # $<...> is a generator expression: evaluated per compiler at build time.
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
