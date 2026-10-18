# GTest is installed by vcpkg from vcpkg.json; CONFIG means "use the CMake
# package files the library ships", REQUIRED stops configuring if missing.
find_package(GTest CONFIG REQUIRED)

add_executable(turtle_art_tests
  color_test.cc
  direction_test.cc
  grid_test.cc
  hex_test.cc
  random_rules_test.cc
  rule_table_test.cc
  turtle_test.cc)

# GTest::gtest_main provides main(), which runs every TEST in the executable.
target_link_libraries(turtle_art_tests PRIVATE turtle_art_lib GTest::gtest_main)

# Registers each TEST with CTest, so `ctest` can list and run them one by one.
include(GoogleTest)
gtest_discover_tests(turtle_art_tests)
