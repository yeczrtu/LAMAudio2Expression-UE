// Run explicitly after editing guide-images.json; never imports upstream content.
import { readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
const root = new URL('../', import.meta.url);
const manifestFile = new URL('guide-images.json', root);
const manifest = JSON.parse(readFileSync(manifestFile, 'utf8'));
const base = '/LAMAudio2Expression-UE/';
const escape = value => String(value).replaceAll('&', '&amp;').replaceAll('<', '&lt;').replaceAll('>', '&gt;').replaceAll('"', '&quot;');
const percent = value => `${Number(value.toFixed(5))}%`;
const pages = new Map();
function figure(g, lang) {
  const ja = lang === 'ja';
  const [x,y,w,h] = g.crop;
  const url = base + g.file;
  const boxes = g.annotations.map((a,i) => {
    const [ax,ay,aw,ah] = a.rect;
    return `<span class="guide-callout" aria-hidden="true" style="--x:${percent((ax-x)/w*100)};--y:${percent((ay-y)/h*100)};--w:${percent(aw/w*100)};--h:${percent(ah/h*100)}"><b>${i+1}</b></span>`;
  }).join('\n');
  let provenance;
  if (g.availability === 'published-demo') {
    provenance = ja ? '既存の公開画像（撮影日不明）。Demo 275a683から無加工で再利用。' : 'Existing public image (capture date unknown), reused unchanged from Demo 275a683.';
    provenance += ` <a href="${g.sourceUrl}">${ja ? '画像の出典' : 'Image source'}</a> · <a href="${base}${ja ? '' : 'en/'}licenses/">${ja ? '素材のクレジット・利用条件' : 'Credits and usage conditions'}</a>`;
  } else {
    provenance = `${ja ? '撮影' : 'Captured'} ${manifest.capturedOn} · UE ${manifest.ue} · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/${manifest.pluginCommit}">Plugin ${manifest.pluginCommit.slice(0,7)}</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/${manifest.demoCommit}">Demo ${manifest.demoCommit.slice(0,7)}</a> · `;
    provenance += ja ? 'v0.3.0に含まれる操作。' : 'Operation included in v0.3.0.';
  }
  return `<!-- guide:${g.id}:start -->
<figure class="guide-figure" id="figure-${g.id}" data-guide="${g.id}">
<div class="guide-shot" style="--shot-ratio:${w}/${h};--shot-width:${percent(g.width/w*100)};--shot-left:${percent(-x/w*100)};--shot-top:${percent(-y/h*100)}">
<div class="guide-window">
<img src="${url}" width="${g.width}" height="${g.height}" loading="lazy" decoding="async" alt="${escape(g.alt[lang])}" />
</div>
${boxes}
</div>
<figcaption>
<p><strong>${escape(g.caption[lang])}</strong> · <a class="guide-original" href="${url}">${ja ? '原寸画像を開く' : 'Open full-size image'}</a></p>
<ol>
${g.annotations.map(a=>`<li>${escape(a[lang])}</li>`).join('\n')}
</ol>
${g.note ? `<p>${escape(g.note[lang])}</p>\n` : ''}<p class="guide-provenance">${provenance}</p>
</figcaption>
</figure>
<!-- guide:${g.id}:end -->`;
}
for (const g of manifest.guides) {
  g.sha256 = createHash('sha256').update(readFileSync(new URL('public/'+g.file, root))).digest('hex');
  for (const placement of g.placements) for (const lang of ['ja','en']) {
    const name = `src/content/docs/${lang === 'ja' ? '' : 'en/'}${placement.slug}.md`;
    if (!pages.has(name)) pages.set(name, readFileSync(new URL(name,root),'utf8').replace(/\r\n/g,'\n').replace(/\n*<!-- guide:[\s\S]*?<!-- guide:[^\n]*?:end -->\n*/g,'\n\n'));
    let text = pages.get(name);
    if (placement.slug === 'demo') text = text.replace(/^!\[[^\n]*\]\(\/LAMAudio2Expression-UE\/images\/face-demo.png\)\n*/m, '');
    const heading = `## ${placement.section[lang]}\n`;
    const start = text.indexOf(heading);
    if (start === -1) throw new Error(`Missing section: ${name}: ${heading}`);
    let end = text.indexOf('\n## ',start+heading.length);
    if (end === -1) end = text.length;
    // The bilingual anchor preceding the next heading must stay with that heading.
    const before = text.slice(0,end);
    const anchor = before.match(/\n<span id="[^\n]+class="comparison-anchor"[^\n]*<\/span>\s*$/);
    if (anchor) end = anchor.index;
    pages.set(name, text.slice(0,end).trimEnd()+'\n\n'+figure(g,lang)+'\n\n'+text.slice(end).trimStart());
  }
}
for (const [name, text] of pages) writeFileSync(new URL(name,root), text.trimEnd()+'\n');
writeFileSync(manifestFile, JSON.stringify(manifest,null,2)+'\n');
console.log(`Rendered ${manifest.guides.length} shared images in ${pages.size} translated operation guides.`);
