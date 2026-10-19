// The turtle-art explorer: browse thousands of seeds, view them large and
// keep favorites. The art itself comes from the C++ library, compiled to
// WebAssembly (see bindings.cc); this file is only the page around it.

import createTurtleArt from './turtle_art.js';

const art = await createTurtleArt();
const PALETTE = art.palette();
const FAVORITES_KEY = 'turtle-art-favorites';
const DEFAULTS = {
  shape: 'hexagon', radius: 15, rows: 21, edge: 'wrap', paint: 'turtle',
  colors: 4, history: 1, steps: 15000, start: 1, minEntropy: 0.6, thumb: 160,
};
// Rows offered for golden rectangles and squares. Consecutive Fibonacci
// numbers have a ratio close to the golden ratio (34 / 21 = 1.619), so a
// golden rectangle is `rows` high and the next Fibonacci number wide.
const FIBONACCI = [5, 8, 13, 21, 34, 55, 89, 144, 233];
const BATCH = 60;          // Pictures added each time the end comes into view.
const FRAME_BUDGET = 12;   // Milliseconds of generating per animation frame.

const $ = (id) => document.getElementById(id);

// ---- Settings: the controls, mirrored in the URL's #fragment -------------

const controls = ['shape', 'radius', 'rows', 'edge', 'paint', 'colors',
                  'history', 'steps', 'start', 'minEntropy', 'thumb'];
const textControls = ['shape', 'edge', 'paint'];  // The rest are numbers.

function readControls() {
  const s = {};
  for (const name of controls) {
    const value = $(name).value;
    s[name] = textControls.includes(name) ? value : Number(value);
  }
  return s;
}

function writeControls(s) {
  for (const name of controls) $(name).value = s[name];
}

function settingsFromHash() {
  const params = new URLSearchParams(location.hash.slice(1));
  const s = { ...DEFAULTS };
  for (const name of controls) {
    if (params.has(name)) {
      const value = params.get(name);
      s[name] = textControls.includes(name) ? value : Number(value);
    }
  }
  return s;
}

// A picture is fully described by its seed plus these knobs.
function pictureOf(s, seed) {
  return { seed, steps: s.steps, shape: s.shape, radius: s.radius,
           rows: s.rows, edge: s.edge, paint: s.paint, colors: s.colors,
           history: s.history };
}

// Favorites saved before a knob existed lack it; they used its first value.
const paintOf = (p) => p.paint ?? 'turtle';
const shapeOf = (p) => p.shape ?? 'hexagon';

// Returns the columns and rows of a golden rectangle or square picture.
function sizeOf(p) {
  if (shapeOf(p) === 'square') return { width: p.rows, height: p.rows };
  const next = FIBONACCI[FIBONACCI.indexOf(p.rows) + 1];
  return { width: next, height: p.rows };
}

function toArtSettings(p) {
  const hexagon = shapeOf(p) === 'hexagon';
  const { width, height } = hexagon ? { width: 0, height: 0 } : sizeOf(p);
  return { seed: p.seed, steps: p.steps,
           shape: hexagon ? art.GridShape.hexagon : art.GridShape.rectangle,
           radius: p.radius, width, height, edgeMode: art.EdgeMode[p.edge],
           paintMode: art.PaintMode[paintOf(p)], paintCount: p.colors,
           historyLength: p.history, hz: 0 };
}

function commandOf(p) {
  let grid = `--radius ${p.radius}`;
  if (shapeOf(p) !== 'hexagon') {
    const { width, height } = sizeOf(p);
    grid = `--width ${width} --height ${height}`;
  }
  return `turtle-art --seed ${p.seed} --steps ${p.steps} ${grid}` +
         ` --edge ${p.edge} --paint ${paintOf(p)} --colors ${p.colors}` +
         ` --history ${p.history}`;
}

const keyOf = (p) => commandOf(p);

// ---- Favorites, kept in this browser's localStorage ----------------------

function loadFavorites() {
  try {
    return JSON.parse(localStorage.getItem(FAVORITES_KEY)) ?? [];
  } catch {
    return [];
  }
}

let favorites = loadFavorites();

function isFavorite(p) {
  return favorites.some((f) => keyOf(f) === keyOf(p));
}

function toggleFavorite(p) {
  favorites = isFavorite(p)
    ? favorites.filter((f) => keyOf(f) !== keyOf(p))
    : [...favorites, p];
  try {
    localStorage.setItem(FAVORITES_KEY, JSON.stringify(favorites));
  } catch {
    // Storage blocked (private window): favorites last for this visit only.
  }
  $('favoritesButton').textContent = `★ Favorites (${favorites.length})`;
}

// ---- Drawing --------------------------------------------------------------

// Where a picture's tiles go, from the C++ layout (see bindings.cc): tile
// centers in Grid::Tiles() order, one tile's outline, and the picture's
// bounds. Cached, since every seed with the same grid shares it.
const layoutCache = new Map();
function layoutFor(p) {
  const s = toArtSettings({ ...p, seed: 0 });
  const key = `${shapeOf(p)} ${s.radius} ${s.width} ${s.height}`;
  if (!layoutCache.has(key)) layoutCache.set(key, art.layout(s));
  return layoutCache.get(key);
}

// Draws `colors` (one Color value per tile) onto `canvas`, `width` CSS
// pixels wide. Tiles of one color go into one path, so each color is a
// single fill call.
function draw(canvas, layout, colors, width) {
  const { centers, outline, bounds } = layout;
  const scale = (width * devicePixelRatio) / bounds.width;
  canvas.width = Math.round(bounds.width * scale);
  canvas.height = Math.round(bounds.height * scale);
  const ctx = canvas.getContext('2d');
  ctx.setTransform(scale, 0, 0, scale, -bounds.x * scale, -bounds.y * scale);
  const paths = PALETTE.map(() => new Path2D());
  // Slightly oversized tiles hide the seams anti-aliasing leaves.
  const grow = 1.04;
  for (let i = 0; i < colors.length; i++) {
    const path = paths[colors[i]];
    const x = centers[2 * i];
    const y = centers[2 * i + 1];
    path.moveTo(x + outline[0] * grow, y + outline[1] * grow);
    for (let c = 2; c < outline.length; c += 2) {
      path.lineTo(x + outline[c] * grow, y + outline[c + 1] * grow);
    }
    path.closePath();
  }
  paths.forEach((path, color) => {
    ctx.fillStyle = PALETTE[color];
    ctx.fill(path);
  });
}

// ---- The grid of thumbnails ----------------------------------------------

let settings = settingsFromHash();
let showingFavorites = false;
let items = [];        // {picture, colors, entropy, card} in display order.
let nextSeed = 0;
let scanned = 0;
let target = 0;
let pumping = false;
let runId = 0;         // Bumped on reset, so a stale loop stops.

function addItem(picture, result) {
  const card = document.createElement('div');
  card.className = 'card';
  const canvas = document.createElement('canvas');
  draw(canvas, layoutFor(picture), result.colors, settings.thumb);
  const index = items.length;
  canvas.addEventListener('click', () => openViewer(index));
  const meta = document.createElement('div');
  meta.className = 'meta';
  const label = document.createElement('span');
  label.textContent = `#${picture.seed} · ${result.entropy.toFixed(2)}`;
  label.title = `seed · interest (color entropy)\n${commandOf(picture)}`;
  const star = document.createElement('button');
  star.className = 'star';
  const updateStar = () => {
    const on = isFavorite(picture);
    star.textContent = on ? '★' : '☆';
    star.classList.toggle('on', on);
  };
  star.addEventListener('click', () => { toggleFavorite(picture); updateStar(); });
  updateStar();
  meta.append(label, star);
  card.append(canvas, meta);
  $('grid').append(card);
  items.push({ picture, colors: result.colors, entropy: result.entropy,
               card, updateStar });
}

function updateStatus() {
  $('status').textContent = showingFavorites
    ? `${items.length} favorites`
    : `seeds ${settings.start}–${nextSeed - 1}: showing ${items.length} of ${scanned}`;
}

// Generates pictures for a few milliseconds per frame until `target` are
// shown, so the page stays responsive.
function pump(id) {
  if (id !== runId) return;
  const t0 = performance.now();
  while (items.length < target && nextSeed <= 0xffffffff &&
         performance.now() - t0 < FRAME_BUDGET) {
    const picture = pictureOf(settings, nextSeed++);
    scanned++;
    const result = art.generate(toArtSettings(picture));
    if (result.entropy >= settings.minEntropy) addItem(picture, result);
  }
  updateStatus();
  if (items.length < target && nextSeed <= 0xffffffff) {
    requestAnimationFrame(() => pump(id));
  } else {
    pumping = false;
  }
}

function requestMore() {
  if (showingFavorites) return;
  target = items.length + BATCH;
  if (!pumping) {
    pumping = true;
    requestAnimationFrame(() => pump(runId));
  }
}

function reset() {
  runId++;
  pumping = false;
  items = [];
  $('grid').replaceChildren();
  document.documentElement.style.setProperty('--thumb', `${settings.thumb}px`);
  $('copyFavorites').hidden = !showingFavorites;
  $('empty').hidden = !(showingFavorites && favorites.length === 0);
  $('favoritesButton').classList.toggle('active', showingFavorites);
  $('favoritesButton').textContent = `★ Favorites (${favorites.length})`;
  if (showingFavorites) {
    for (const picture of favorites) {
      addItem(picture, art.generate(toArtSettings(picture)));
    }
    updateStatus();
  } else {
    nextSeed = settings.start;
    scanned = 0;
    requestMore();
  }
}

// Shows the size control that fits the shape: radius for hexagons, rows
// for the shapes made of square tiles.
function showSizeControl() {
  const hexagon = $('shape').value === 'hexagon';
  $('radiusLabel').hidden = !hexagon;
  $('rowsLabel').hidden = hexagon;
}

function applyControls() {
  showSizeControl();
  settings = readControls();
  $('minEntropyValue').textContent = settings.minEntropy.toFixed(2);
  const params = new URLSearchParams();
  for (const name of controls) params.set(name, settings[name]);
  history.replaceState(null, '', `#${params}`);
  reset();
}

// Loads more when the end of the grid scrolls into view.
new IntersectionObserver((entries) => {
  if (entries.some((e) => e.isIntersecting)) requestMore();
}, { rootMargin: '600px' }).observe($('sentinel'));

for (const name of controls) $(name).addEventListener('change', applyControls);
$('minEntropy').addEventListener('input', () => {
  $('minEntropyValue').textContent = Number($('minEntropy').value).toFixed(2);
});
$('favoritesButton').addEventListener('click', toggleFavoritesView);
$('random').addEventListener('click', randomStart);

// Jumps to a random place among the 2^32 seeds.
function randomStart() {
  $('start').value = Math.floor(Math.random() * 0x100000000);
  applyControls();
}
$('copyFavorites').addEventListener('click', () => {
  navigator.clipboard.writeText(favorites.map(commandOf).join('\n'));
});

function toggleFavoritesView() {
  showingFavorites = !showingFavorites;
  reset();
}

// ---- The large viewer ------------------------------------------------------

let current = -1;

function openViewer(index) {
  current = index;
  $('viewer').hidden = false;
  showCurrent();
}

function showCurrent() {
  const item = items[current];
  const layout = layoutFor(item.picture);
  const aspect = layout.bounds.width / layout.bounds.height;
  const width = Math.min(innerWidth - 24, (innerHeight - 110) * aspect);
  draw($('big'), layout, item.colors, width);
  $('big').style.width = `${width}px`;
  $('info').textContent = `#${item.picture.seed} · interest ` +
    `${item.entropy.toFixed(2)} · ${current + 1}/${items.length}`;
  $('fav').textContent = isFavorite(item.picture) ? '★' : '☆';
  $('command').textContent = commandOf(item.picture);
  // Keep more pictures coming when browsing near the end.
  if (current >= items.length - 5) requestMore();
}

function step(delta) {
  const index = current + delta;
  if (index >= 0 && index < items.length) {
    current = index;
    showCurrent();
    items[current].card.scrollIntoView({ block: 'nearest' });
  }
}

function closeViewer() {
  $('viewer').hidden = true;
}

function favoriteCurrent() {
  const item = items[current];
  toggleFavorite(item.picture);
  item.updateStar();
  showCurrent();
}

function downloadCurrent() {
  const p = items[current].picture;
  const blob = new Blob([art.svg(toArtSettings(p))], { type: 'image/svg+xml' });
  const link = document.createElement('a');
  link.href = URL.createObjectURL(blob);
  link.download = `turtle-art-${p.seed}.svg`;
  link.click();
  URL.revokeObjectURL(link.href);
}

function copyCurrent() {
  navigator.clipboard.writeText(commandOf(items[current].picture));
}

$('prev').addEventListener('click', () => step(-1));
$('next').addEventListener('click', () => step(1));
$('fav').addEventListener('click', favoriteCurrent);
$('download').addEventListener('click', downloadCurrent);
$('copy').addEventListener('click', copyCurrent);
$('close').addEventListener('click', closeViewer);
$('viewer').addEventListener('click', (e) => {
  if (e.target === $('viewer')) closeViewer();
});

// ---- Slideshow: a new animated picture every 30 seconds ------------------
//
// The animation itself comes from C++ (an SVG that animates itself with
// SMIL, see ToAnimatedSvg): the page only picks seeds and shows their SVGs.
// 300 steps at 10 steps per second take 30 seconds, so each picture has
// just finished drawing when the next one starts, from a clear grid.

const SLIDESHOW = {
  shape: 'hexagon', radius: 9, rows: 21, edge: 'wrap', paint: 'turtle',
  colors: 4, history: 2, steps: 300,
};
const SLIDESHOW_HZ = 10;
const SLIDESHOW_MIN_INTEREST = 0.95;
// The picture's share of the screen height. Copies of it surround it, as
// many rings as it takes to fill the screen edge to edge (2 on most
// screens, 3 on ultrawide ones).
const SLIDESHOW_SCALE = 0.6;

let slideTimer = null;
let slidePicture = null;
let slideUrl = null;

// Tries random seeds until one is interesting enough. Most are not, but
// each try takes well under a millisecond at this size.
function interestingSlidePicture() {
  let picture;
  for (let tries = 0; tries < 10000; tries++) {
    picture = { ...SLIDESHOW, seed: Math.floor(Math.random() * 0x100000000) };
    if (art.generate(toArtSettings(picture)).entropy > SLIDESHOW_MIN_INTEREST) {
      break;
    }
  }
  return picture;
}

function updateSlideFavorite() {
  $('slideFav').textContent = isFavorite(slidePicture) ? '★' : '☆';
}

// Sizes the slide frame to the picture's shape, SLIDESHOW_SCALE of the
// screen height (or less, to fit the width).
// Returns the frame's height in pixels.
function sizeSlideFrame() {
  const { bounds } = layoutFor(slidePicture);
  const height = Math.min((innerHeight - 90) * SLIDESHOW_SCALE,
                          ((innerWidth - 24) * SLIDESHOW_SCALE * bounds.height) / bounds.width);
  $('slideFrame').style.height = `${height}px`;
  $('slideFrame').style.width = `${(height * bounds.width) / bounds.height}px`;
  return height;
}

// Shows the animated SVG at `url`, plus copies of it around, so a turtle
// leaving one side can be followed into the next copy, and the art fills
// the screen. All images share one URL, so the browser plays them in step.
// CSS translate percentages are of the image's own size, hence offset /
// picture size.
function showSlide(url) {
  const { bounds } = layoutFor(slidePicture);
  const settings = toArtSettings(slidePicture);
  // The screen's size in picture units, for as many rings as fill it.
  const unitsPerPixel = bounds.height / sizeSlideFrame();
  const rings = art.wrapRingsToFill(settings, innerWidth * unitsPerPixel,
                                    innerHeight * unitsPerPixel);
  const copies = art.wrapCopies(settings, rings);
  const images = [];
  for (let i = 0; i < copies.length; i += 2) {
    const copy = new Image();
    copy.alt = '';
    copy.src = url;
    copy.style.transform = `translate(${(copies[i] / bounds.width) * 100}%, ` +
                           `${(copies[i + 1] / bounds.height) * 100}%)`;
    images.push(copy);
  }
  const picture = new Image();
  picture.alt = 'Animated turtle-art picture';
  picture.src = url;
  images.push(picture);  // Last, so it is drawn above its copies.
  $('slideFrame').replaceChildren(...images);
}

function nextSlide() {
  clearTimeout(slideTimer);
  slidePicture = interestingSlidePicture();
  const svg = art.svg({ ...toArtSettings(slidePicture), hz: SLIDESHOW_HZ });
  // New images start their animation from the beginning: a clear grid.
  const old = slideUrl;
  slideUrl = URL.createObjectURL(new Blob([svg], { type: 'image/svg+xml' }));
  showSlide(slideUrl);
  if (old) URL.revokeObjectURL(old);
  $('slideInfo').textContent = `#${slidePicture.seed}`;
  $('slideCommand').textContent = `${commandOf(slidePicture)} --hz ${SLIDESHOW_HZ}`;
  updateSlideFavorite();
  slideTimer = setTimeout(nextSlide, (slidePicture.steps / SLIDESHOW_HZ) * 1000);
}

function startSlideshow() {
  $('slideshow').hidden = false;
  nextSlide();
}

function stopSlideshow() {
  clearTimeout(slideTimer);
  $('slideshow').hidden = true;
}

function favoriteSlide() {
  toggleFavorite(slidePicture);
  updateSlideFavorite();
}

$('slideshowButton').addEventListener('click', startSlideshow);
// A new size may need more rings. The images reuse the same URL, so the
// animation carries on where it was.
window.addEventListener('resize', () => {
  if (!$('slideshow').hidden) showSlide(slideUrl);
});
$('slideFav').addEventListener('click', favoriteSlide);
$('slideNext').addEventListener('click', nextSlide);
$('slideClose').addEventListener('click', stopSlideshow);

document.addEventListener('keydown', (e) => {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'SELECT') return;
  if (!$('slideshow').hidden) {
    const slideActions = { Escape: stopSlideshow, f: favoriteSlide, n: nextSlide };
    if (slideActions[e.key]) {
      e.preventDefault();
      slideActions[e.key]();
    }
    return;
  }
  if ($('viewer').hidden) {
    if (e.key === 'g') toggleFavoritesView();
    if (e.key === 'r') randomStart();
    if (e.key === 's') startSlideshow();
    // Enter opens the first picture, to browse with the keyboard alone.
    if (e.key === 'Enter' && items.length > 0) openViewer(0);
    return;
  }
  const actions = {
    ArrowLeft: () => step(-1), ArrowRight: () => step(1),
    f: favoriteCurrent, d: downloadCurrent, c: copyCurrent, Escape: closeViewer,
  };
  if (actions[e.key]) {
    e.preventDefault();
    actions[e.key]();
  }
});

// ---- Start -------------------------------------------------------------------

// All but the last, which has no next Fibonacci number for the width.
for (const rows of FIBONACCI.slice(0, -1)) $('rows').add(new Option(rows, rows));
writeControls(settings);
applyControls();
