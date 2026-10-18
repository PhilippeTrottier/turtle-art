# The program's logic, as a library so the tests can link to it.
add_library(turtle_art_lib
  direction.cc
  grid.cc
  hex.cc)

# PUBLIC: targets linking turtle_art_lib also get this include path, so they
# can write #include "direction.h".
target_include_directories(turtle_art_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(turtle_art_lib PUBLIC turtle_art_options)
