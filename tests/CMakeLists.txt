# GTest is installed by vcpkg from vcpkg.json; CONFIG means "use the CMake
# package files the library ships", REQUIRED stops configuring if missing.
find_package(GTest CONFIG REQUIRED)

add_executable(turtle_art_tests
  art_test.cc
  color_test.cc
  grid_stats_test.cc
  grid_test.cc
  layout_test.cc
  palette_test.cc
  png_test.cc
  random_rules_test.cc
  rule_table_test.cc
  svg_test.cc
  tile_test.cc
  turtle_test.cc)

# GTest::gtest_main provides main(), which runs every TEST in the executable.
# lodepng too: the PNG tests decode what they encode.
find_package(lodepng CONFIG REQUIRED)
target_link_libraries(turtle_art_tests PRIVATE turtle_art_lib lodepng
                                               GTest::gtest_main)

# Registers each TEST with CTest, so `ctest` can list and run them one by one.
include(GoogleTest)
gtest_discover_tests(turtle_art_tests)
