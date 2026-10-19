// What a picture is, shared by the page (app.js) and the worker that
// generates pictures (worker.js), so both describe pictures the same way.
//
// A picture is a plain object: its seed plus the knobs, e.g.
//   {seed: 7, steps: 15000, shape: 'hexagon', radius: 15, rows: 21,
//    edge: 'wrap', paint: 'turtle', colors: 4, history: 1, theme: 'light'}
// Plain objects can be saved as favorites and sent to the worker as is.

// Rows offered for golden rectangles and squares. Consecutive Fibonacci
// numbers have a ratio close to the golden ratio (34 / 21 = 1.619), so a
// golden rectangle is `rows` high and the next Fibonacci number wide. The
// last number is only ever a width (of 610 rows), so the rows menu leaves it
// out.
export const FIBONACCI = [5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987];

// Favorites saved before a knob existed lack it; they used its first value.
export const paintOf = (p) => p.paint ?? 'turtle';
export const shapeOf = (p) => p.shape ?? 'hexagon';
export const themeOf = (p) => p.theme ?? 'light';

// Returns the columns and rows of a golden rectangle or square picture.
export function sizeOf(p) {
  if (shapeOf(p) === 'square') return { width: p.rows, height: p.rows };
  const next = FIBONACCI[FIBONACCI.indexOf(p.rows) + 1];
  return { width: next, height: p.rows };
}

// Returns the C++ ArtSettings for picture `p` (see bindings.cc), using the
// enums of `art`, a loaded turtle_art module.
export function toArtSettings(art, p) {
  const hexagon = shapeOf(p) === 'hexagon';
  const { width, height } = hexagon ? { width: 0, height: 0 } : sizeOf(p);
  return { seed: p.seed, steps: p.steps,
           shape: hexagon ? art.GridShape.hexagon : art.GridShape.rectangle,
           radius: p.radius, width, height, edgeMode: art.EdgeMode[p.edge],
           paintMode: art.PaintMode[paintOf(p)], paintCount: p.colors,
           historyLength: p.history, hz: 0,
           theme: art.Theme[themeOf(p)] };
}

// Returns the turtle-art command line that draws picture `p`.
export function commandOf(p) {
  let grid = `--radius ${p.radius}`;
  if (shapeOf(p) !== 'hexagon') {
    const { width, height } = sizeOf(p);
    grid = `--width ${width} --height ${height}`;
  }
  return `turtle-art --seed ${p.seed} --steps ${p.steps} ${grid}` +
         ` --edge ${p.edge} --paint ${paintOf(p)} --colors ${p.colors}` +
         ` --history ${p.history} --theme ${themeOf(p)}`;
}
