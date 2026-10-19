// Generates pictures on a background thread (a Web Worker), so that slow
// ones (many steps) never freeze the page. It has its own copy of the C++
// module and does one thing:
//
//   page -> worker: {run, picture}  (see pictures.js for `picture`)
//   worker -> page: {run, picture, colors, entropy}
//
// where colors and entropy are what art.generate() returns (bindings.cc),
// and `run` is passed through so the page can drop answers it no longer
// wants. Requests are answered one at a time, in the order they arrive.

import createTurtleArt from './turtle_art.js';
import { toArtSettings } from './pictures.js';

// Start loading now; requests that arrive meanwhile wait for it.
const ready = createTurtleArt();

onmessage = async ({ data: { run, picture } }) => {
  const art = await ready;
  const { colors, entropy } = art.generate(toArtSettings(art, picture));
  // Listing colors.buffer as "transferable" hands the bytes to the page
  // instead of copying them.
  postMessage({ run, picture, colors, entropy }, [colors.buffer]);
};
