import assert from 'node:assert/strict';
import { existsSync, readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { parseHTML, DOMParser } from 'linkedom';

const root = new URL('../dist/', import.meta.url);
const base = '/LAMAudio2Expression-UE/';
const origin = 'https://yeczrtu.github.io';
const slugs = ['', 'lip-sync-comparison', 'installation', 'demo', 'blueprint', 'baked-clips', 'expression-curves', 'playback', 'live-input',
  'models-and-packaging', 'development', 'architecture', 'validation', 'troubleshooting', 'licenses'];
const expected = slugs.flatMap(slug => ['', 'en/'].map(locale => `${base}${locale}${slug ? slug + '/' : ''}`));
const documents = new Map();
const titles = new Set();
const descriptions = new Set();
let links = 0;
const guideManifest = JSON.parse(readFileSync(new URL('../guide-images.json', import.meta.url), 'utf8'));
const contentSources = JSON.parse(readFileSync(new URL('../content-sources.json', import.meta.url), 'utf8'));
const guideById = new Map(guideManifest.guides.map(g => [g.id, g]));
let guideCount = 0;

function localFile(pathname) {
  assert(pathname.startsWith(base), `URL escapes project base: ${pathname}`);
  let path = decodeURIComponent(pathname.slice(base.length));
  if (!path || path.endsWith('/')) path += 'index.html';
  return new URL(path, root);
}
function documentAt(pathname) {
  if (!documents.has(pathname)) documents.set(pathname, parseHTML(readFileSync(localFile(pathname), 'utf8')).document);
  return documents.get(pathname);
}

for (const path of expected) {
  const doc = documentAt(path);
  const en = path.startsWith(base + 'en/');
  const lang = en ? 'en' : 'ja';
  const url = origin + path;
  const pairSlug = path.slice(base.length).replace(/^en\//, '');
  // Check download targets against the reviewed release, not a second hard-coded version.
  for (const a of doc.querySelectorAll('a[href*="/releases/download/"]')) {
    const target = new URL(a.getAttribute('href'));
    if (target.hostname !== 'github.com' || !target.pathname.startsWith('/yeczrtu/')) continue;
    assert.equal(target.pathname.split('/releases/download/')[1].split('/')[0], contentSources.release, `Stale release download: ${path}`);
  }
  if (pairSlug === '') {
    const downloads = [...doc.querySelectorAll('.download-grid a')];
    assert.equal(downloads.length, 3);
    for (const a of downloads) assert(a.href.includes(`/releases/download/${contentSources.release}/`), `Homepage release: ${path}`);
  }
  assert.equal(doc.documentElement.lang, lang, `Wrong language: ${path}`);
  assert.equal(doc.querySelectorAll('h1').length, 1, `Expected one H1: ${path}`);
  assert.equal(doc.querySelectorAll('link[rel="canonical"]').length, 1, `Duplicate canonical: ${path}`);
  assert.equal(doc.querySelector('link[rel="canonical"]')?.getAttribute('href'), url, `Canonical: ${path}`);
  assert.equal(doc.querySelector('meta[property="og:url"]')?.getAttribute('content'), url);
  assert.equal(doc.querySelectorAll('meta[property="og:locale"]').length, 1);
  assert.equal(doc.querySelectorAll('meta[name="twitter:card"]').length, 1);
  assert.equal(doc.querySelectorAll('link[rel="sitemap"]').length, 1);
  assert(!doc.querySelector('meta[name="robots"]')?.getAttribute('content')?.includes('noindex'), `Accidental noindex: ${path}`);
  const title = doc.title;
  const description = doc.querySelector('meta[name="description"]')?.getAttribute('content');
  assert(title && !titles.has(title), `Missing / duplicate title: ${path}`);
  assert(description && !descriptions.has(description), `Missing / duplicate description: ${path}`);
  titles.add(title); descriptions.add(description);
  assert.equal(doc.querySelector('link[hreflang="ja"]')?.getAttribute('href'), origin + base + pairSlug);
  assert.equal(doc.querySelector('link[hreflang="en"]')?.getAttribute('href'), origin + base + 'en/' + pairSlug);
  assert.equal(doc.querySelector('link[hreflang="x-default"]')?.getAttribute('href'), origin + base + pairSlug);
  const json = doc.querySelector('script[type="application/ld+json"]');
  assert(json, `Missing structured data: ${path}`);
  const graph = JSON.parse(json.textContent)['@graph'];
  assert.equal(graph.find(item => item['@type'] === 'TechArticle')?.url, url);
  const breadcrumb = graph.find(item => item['@type'] === 'BreadcrumbList');
  assert.equal(breadcrumb?.itemListElement.at(-1)?.item, url);
  assert(doc.querySelector('nav.breadcrumbs'));
  assert(doc.querySelector('.source-note a[href*="/blob/"]'), `Missing pinned source: ${path}`);
  for (const button of doc.querySelectorAll('.expressive-code button')) {
    assert.equal(button.getAttribute('title'), en ? 'Copy to clipboard' : 'コードをコピー', `Code control language: ${path}`);
  }
  assert(!doc.querySelector('main')?.textContent.includes('§'), `Authoring marker: ${path}`);
  for (const image of doc.querySelectorAll('img')) assert(image.getAttribute('alt')?.trim(), `Image missing alt: ${path}`);
  const expectedGuides = guideManifest.guides.filter(g => g.placements.some(p => p.slug + '/' === pairSlug)).map(g => g.id).sort();
  const figures = [...doc.querySelectorAll('figure[data-guide]')];
  assert.deepEqual(figures.map(f => f.dataset.guide).sort(), expectedGuides, `Guide placements: ${path}`);
  for (const figure of figures) {
    const g = guideById.get(figure.dataset.guide);
    const img = figure.querySelector('img');
    assert.equal(img?.getAttribute('src'), base + g.file);
    assert.equal(img?.getAttribute('alt'), g.alt[lang]);
    assert.equal(Number(img?.getAttribute('width')), g.width);
    assert.equal(Number(img?.getAttribute('height')), g.height);
    assert.equal(img?.getAttribute('loading'), 'lazy');
    assert.equal(figure.querySelector('.guide-original')?.getAttribute('href'), base + g.file);
    assert.equal(figure.querySelectorAll('.guide-callout').length, g.annotations.length);
    assert.deepEqual([...figure.querySelectorAll('figcaption li')].map(li => li.textContent), g.annotations.map(a => a[lang]));
    assert.deepEqual([...figure.querySelectorAll('.guide-callout b')].map(b => b.textContent), g.annotations.map((_,i) => String(i+1)));
    if (g.availability === 'release-0.3.0') assert.match(figure.querySelector('.guide-provenance').textContent, /v0\.3\.0/);
    guideCount++;
  }
  for (const tag of doc.querySelectorAll('[href], [src], [poster]')) {
    for (const attr of ['href', 'src', 'poster']) {
      const value = tag.getAttribute(attr);
      if (!value || /^(data:|mailto:|javascript:)/.test(value)) continue;
      assert(!value.includes('~/'), `Unresolved authoring path: ${value}`);
      const target = new URL(value, url);
      if (target.origin !== origin) continue;
      assert(existsSync(localFile(target.pathname)), `Broken local resource on ${path}: ${value}`);
      if (target.hash && (target.pathname.endsWith('/') || target.pathname.endsWith('.html'))) {
        const targetDoc = documentAt(target.pathname);
        assert(targetDoc.getElementById(decodeURIComponent(target.hash.slice(1))), `Broken anchor on ${path}: ${value}`);
      }
      links++;
    }
  }
}
const index = new DOMParser().parseFromString(readFileSync(new URL('sitemap-index.xml', root), 'utf8'), 'text/xml');
// Starlight keeps the URL fragment when switching language. Paired
// documents must resolve the other language's section anchors as well.
for (const slug of ['lip-sync-comparison', 'baked-clips']) for (const locale of ['', 'en/']) {
  const doc = documentAt(`${base}${locale}${slug}/`);
  const pair = documentAt(`${base}${locale ? '' : 'en/'}${slug}/`);
  for (const heading of doc.querySelectorAll('.sl-markdown-content h2, .sl-markdown-content h3')) {
    assert(pair.getElementById(heading.id), `Missing translated section anchor: ${heading.id}`);
  }
}
const sitemapUrls = [];
for (const element of index.querySelectorAll('loc')) {
  const mapUrl = new URL(element.textContent);
  assert.equal(mapUrl.origin, origin);
  const map = new DOMParser().parseFromString(readFileSync(localFile(mapUrl.pathname), 'utf8'), 'text/xml');
  for (const location of map.querySelectorAll('url > loc')) sitemapUrls.push(location.textContent);
}
assert.deepEqual(sitemapUrls.sort(), expected.map(path => origin + path).sort(), `Sitemap must contain exactly ${expected.length} canonical documents`);
const notFound = parseHTML(readFileSync(new URL('404.html', root), 'utf8')).document;
assert(notFound.querySelector('meta[name="robots"]')?.getAttribute('content')?.includes('noindex'));
assert(existsSync(new URL('pagefind/pagefind.js', root)), 'Missing search bundle');
const searchEntry = JSON.parse(readFileSync(new URL('pagefind/pagefind-entry.json', root), 'utf8'));
for (const language of ['ja', 'en']) assert.equal(searchEntry.languages[language]?.page_count, slugs.length, `Search index: ${language}`);
for (const g of guideManifest.guides) {
  const bytes = readFileSync(new URL(g.file, root));
  assert.equal(createHash('sha256').update(bytes).digest('hex'), g.sha256, `Raw image changed: ${g.id}`);
  const [x,y,w,h] = g.crop;
  assert(x >= 0 && y >= 0 && w > 0 && h > 0 && x+w <= g.width && y+h <= g.height, `Crop outside raw image: ${g.id}`);
  for (const {rect:[ax,ay,aw,ah]} of g.annotations) assert(ax >= x && ay >= y && aw > 0 && ah > 0 && ax+aw <= x+w && ay+ah <= y+h, `Callout outside crop: ${g.id}`);
}
console.log(`Verified ${expected.length} pages, ${links} local links/assets, reciprocal language links, metadata, JSON-LD, sitemap, 404, and two ${slugs.length}-page search indexes.`);
console.log(`Verified ${guideCount} bilingual figures and ${guideManifest.guides.length} original image hashes, crops, callouts, captions, alt text and full-size links.`);
