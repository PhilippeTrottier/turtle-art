# The web explorer: turtle_art_lib compiled to WebAssembly with Emscripten,
# plus a static page. Built only by the `wasm` preset; the output folder
# build/wasm/web/ is the whole site (see README).

add_executable(turtle_art_web bindings.cc)
target_link_libraries(turtle_art_web PRIVATE turtle_art_lib)
target_link_options(turtle_art_web PRIVATE
  -lembind                    # Link Embind, used by bindings.cc.
  -sMODULARIZE=1              # Wrap the module in a factory function...
  -sEXPORT_ES6=1              # ...exported as a JavaScript module...
  -sEXPORT_NAME=createTurtleArt  # ...named createTurtleArt.
  -sALLOW_MEMORY_GROWTH=1     # Let big grids grow the WebAssembly memory.
  -sENVIRONMENT=web,worker)   # Browsers, on the page and in Web Workers.
# Emscripten writes turtle_art.js (the glue) and turtle_art.wasm.
set_target_properties(turtle_art_web PROPERTIES OUTPUT_NAME turtle_art
                                                SUFFIX ".js")

# Copy the page next to them. configure_file re-runs when a source changes.
configure_file(index.html index.html COPYONLY)
configure_file(app.js app.js COPYONLY)
configure_file(pictures.js pictures.js COPYONLY)
configure_file(worker.js worker.js COPYONLY)
