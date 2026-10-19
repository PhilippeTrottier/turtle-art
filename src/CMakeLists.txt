# The program's logic, as a library so the tests can link to it.
add_library(turtle_art_lib
  art.cc
  color.cc
  direction.cc
  grid.cc
  grid_stats.cc
  hex.cc
  random_rules.cc
  rule_table.cc
  svg.cc
  turtle.cc)

# PUBLIC: targets linking turtle_art_lib also get this include path, so they
# can write #include "direction.h".
target_include_directories(turtle_art_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(turtle_art_lib PUBLIC turtle_art_options)

# The command-line program: a thin layer over turtle_art_lib. Not built for
# the web, which has its own front end (web/).
if(NOT EMSCRIPTEN)
  find_package(CLI11 CONFIG REQUIRED)
  add_executable(turtle-art main.cc)
  target_link_libraries(turtle-art PRIVATE turtle_art_lib CLI11::CLI11)
endif()
