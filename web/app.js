// The turtle-art explorer: browse thousands of seeds, view them large and
// keep favorites. The art itself comes from the C++ library, compiled to
// WebAssembly (see bindings.cc); this file is only the page around it.

import createTurtleArt from './turtle_art.js';

const art = await createTurtleArt();
const PALETTE = art.palette();
const SQRT3 = Math.sqrt(3);
const FAVORITES_KEY = 'turtle-art-favorites';
const DEFAULTS = {
  edge: 'wrap', colors: 4, history: 3, radius: 25, steps: 6000, start: 1,
  minEntropy: 0.5, thumb: 160,
};
const BATCH = 60;          // Pictures added each time the end comes into view.
const FRAME_BUDGET = 12;   // Milliseconds of generating per animation frame.

const $ = (id) => document.getElementById(id);

// ---- Settings: the controls, mirrored in the URL's #fragment -------------

const controls = ['edge', 'colors', 'history', 'radius', 'steps', 'start',
                  'minEntropy', 'thumb'];

function readControls() {
  const s = {};
  for (const name of controls) {
    const value = $(name).value;
    s[name] = name === 'edge' ? value : Number(value);
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
      s[name] = name === 'edge' ? params.get(name) : Number(params.get(name));
    }
  }
  return s;
}

// A picture is fully described by its seed plus these knobs.
function pictureOf(s, seed) {
  return { seed, steps: s.steps, radius: s.radius, edge: s.edge,
           colors: s.colors, history: s.history };
}

function toArtSettings(p) {
  return { seed: p.seed, steps: p.steps, radius: p.radius,
           edgeMode: art.EdgeMode[p.edge], paintCount: p.colors,
           historyLength: p.history };
}

function commandOf(p) {
  return `turtle-art --seed ${p.seed} --steps ${p.steps} --radius ${p.radius}` +
         ` --edge ${p.edge} --colors ${p.colors} --history ${p.history}`;
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

// Tile centers for a radius, in the same order as Grid::Tiles() in C++
// (rows r from -radius to radius, q from low to high), in units of the
// hexagon size. Same formula as CenterOf() in svg.cc.
const centersCache = new Map();
function centersFor(radius) {
  if (!centersCache.has(radius)) {
    const centers = [];
    for (let r = -radius; r <= radius; r++) {
      for (let q = -radius; q <= radius; q++) {
        if ((Math.abs(q) + Math.abs(r) + Math.abs(q + r)) / 2 <= radius) {
          centers.push([SQRT3 * (q + r / 2), 1.5 * r]);
        }
      }
    }
    centersCache.set(radius, centers);
  }
  return centersCache.get(radius);
}

// Draws `colors` (one Color value per tile) onto `canvas`, `width` CSS
// pixels wide. Tiles of one color go into one path, so each color is a
// single fill call.
function draw(canvas, radius, colors, width) {
  const halfW = SQRT3 * (radius + 0.5);
  const halfH = 1.5 * radius + 1;
  const scale = (width * devicePixelRatio) / (2 * halfW);
  canvas.width = Math.round(2 * halfW * scale);
  canvas.height = Math.round(2 * halfH * scale);
  const ctx = canvas.getContext('2d');
  ctx.setTransform(scale, 0, 0, scale, halfW * scale, halfH * scale);
  const paths = PALETTE.map(() => new Path2D());
  const centers = centersFor(radius);
  // Slightly oversized hexagons hide the seams anti-aliasing leaves.
  const s = 1.04;
  const dx = (SQRT3 / 2) * s;
  centers.forEach(([x, y], i) => {
    const path = paths[colors[i]];
    path.moveTo(x + dx, y - s / 2);
    path.lineTo(x + dx, y + s / 2);
    path.lineTo(x, y + s);
    path.lineTo(x - dx, y + s / 2);
    path.lineTo(x - dx, y - s / 2);
    path.lineTo(x, y - s);
    path.closePath();
  });
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
  draw(canvas, picture.radius, result.colors, settings.thumb);
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

function applyControls() {
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
  const width = Math.min(innerWidth - 24, (innerHeight - 110) * 1.15);
  draw($('big'), item.picture.radius, item.colors, width);
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

document.addEventListener('keydown', (e) => {
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'SELECT') return;
  if ($('viewer').hidden) {
    if (e.key === 'g') toggleFavoritesView();
    if (e.key === 'r') randomStart();
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

writeControls(settings);
applyControls();
