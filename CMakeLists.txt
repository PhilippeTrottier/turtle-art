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

# Run clang-tidy (configured in .clang-tidy) on every file as it compiles.
# Visual Studio ships clang-tidy next to MSVC, so also look there: the
# compiler lives in VC/Tools/MSVC/<version>/bin/Hostx64/x64.
option(TURTLE_ART_CLANG_TIDY "Run clang-tidy during the build" ON)
if(TURTLE_ART_CLANG_TIDY)
  cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH compiler_dir)
  find_program(CLANG_TIDY_EXE clang-tidy
    HINTS "${compiler_dir}/../../../../../Llvm/x64/bin")
  if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy: ${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    if(MSVC)
      # clang-tidy ignores the /EHsc in MSVC's compile command, then
      # rejects any try/catch (CLI11 has some). Passing /EHsc again fixes it.
      list(APPEND CMAKE_CXX_CLANG_TIDY "--extra-arg=/EHsc")
    endif()
  else()
    message(WARNING "clang-tidy not found; building without it. "
                    "Install it or configure with -DTURTLE_ART_CLANG_TIDY=OFF.")
  endif()
endif()

add_subdirectory(src)

# EMSCRIPTEN is set by Emscripten's toolchain file (the `wasm` preset). That
# build makes the web explorer; the tests and command-line program are built
# natively by the other presets.
if(EMSCRIPTEN)
  add_subdirectory(web)
else()
  enable_testing()
  add_subdirectory(tests)
endif()
