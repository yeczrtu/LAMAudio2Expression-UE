# LAM Audio2Expression documentation site

Public site: **https://yeczrtu.github.io/LAMAudio2Expression-UE/**

Japanese is served at the project root; English is under `/en/`.
This orphan branch, `codex/docs-site`, contains only the documentation site.
It is never merged into the plugin's `main` branch or the demo's `master` branch.

## Local development

Use Node.js **24.21.0** (see `.nvmrc`) in a separate checkout:

```powershell
git clone --single-branch --branch codex/docs-site https://github.com/yeczrtu/LAMAudio2Expression-UE.git LAMAudio2Expression-docs
cd LAMAudio2Expression-docs
npm ci
npm run dev
```

Open `http://127.0.0.1:4321/LAMAudio2Expression-UE/`.
Search is generated during a production build, so test it with `npm run preview` after building.

```powershell
npm run check
npm run build
npm run preview
```

The build verifier checks all 30 documents, internal links and fragments, assets,
unique metadata, self-canonicals, reciprocal language alternates, structured data,
the sitemap, 404 exclusion, and both 15-page search indexes.

## Editing documents

Edit Japanese Markdown in `src/content/docs/` and the corresponding English file
in `src/content/docs/en/` together. Keep the same slug in both languages.
Each page requires `title`, `description`, `appliesTo`, and pinned `sources`.
Articles may add `sourceSummary` to describe newer source snapshots or external evidence
instead of the default LAM release note. Link official sources next to claims and
state the review date and product version. Edit both translations together.
The comparison and baked-clips pages keep alternate-language heading anchors so switching
languages from a section stays at the corresponding section. Preserve these
anchors when editing headings and add paired anchors for new translated sections.
Update source links and version notes when reviewing a new release or commit.
Do not automatically import unpublished files or assume a source feature is in a release ZIP.

For another page, update the sidebar and expected routes in the verifier as well.
Internal URLs must include `/LAMAudio2Expression-UE/` and the English prefix where appropriate.
Metadata, search assets, and language switching must retain the project base path.

## Publishing and rollback

Push reviewed changes to `codex/docs-site`. The `Deploy documentation` workflow
runs `npm ci`, type/content checks, the static build and verifier, then deploys
the built artifact. GitHub Pages uses **GitHub Actions** as its source.
The `github-pages` environment permits deployment only from `codex/docs-site`.
No workflow file is required in `main`.

Review the workflow under the repository's Actions tab; the manual dispatch
button is intentionally not required for this non-default branch.
A failed build does not replace the previously deployed version.
To roll back, revert the relevant site commit and push the revert to this branch.

## SEO and ownership

Sitemap: https://yeczrtu.github.io/LAMAudio2Expression-UE/sitemap-index.xml

Each document has static HTML, a self-canonical, Japanese/English alternates,
Open Graph metadata, and article/breadcrumb structured data. The 404 is noindex
and excluded from the sitemap. There is no analytics or custom domain.

Only `https://yeczrtu.github.io/robots.txt` controls crawling for this host.
A `robots.txt` inside this project's subdirectory would not control crawlers,
so none is emitted. The repository does not own the host-root configuration.
If adding this site to Search Console, verify the project URL-prefix property
using the owner's account and submit the sitemap above. Search Console registration
and search-engine indexing are external to deployment verification.

## Sources and media

See `content-sources.json` for the pinned public snapshots and release URLs.
The initial site covers v0.2.0 and the September 25 public Blueprint demo changes.
The `baked-clips/` guide additionally covers Plugin `1f06ac8` and Demo `f3b6f13`,
published September 28, 2026. It is explicitly outside the v0.2.0 release ZIPs.
Keep its source-only availability note, pinned sources, and recorded validation
conditions together when updating it. Model packaging settings remain unchanged.
The site does not cover unpublished Android investigation or provide a full Viseme guide.
The bilingual `lip-sync-comparison/` article additionally summarizes official
third-party documentation and research reviewed on September 28, 2026. Its
recommendations are based on published specifications, not new competitive
benchmarks. Keep vendor latency claims distinct from measured LAM inference times.

Media is reused from the published demo, with attribution on both license pages
and beneath the preview. ZIP binaries remain at their existing GitHub URLs.
The original video attachment returned HTTP 404 on 2026-09-28, both anonymously
and in the browser. The site shows the published still image and runnable-demo
instructions instead of a broken player. Restore an embedded player only after
confirming a working public URL for the same recording and updating the credit text.
See `THIRD_PARTY_NOTICES.md`; the MIT license does not relicense third-party media.

The operation guides share 20 English-UI captures made on September 30, 2026
with UE 5.8.2, Plugin `1860d0e` and Demo `9dee71d`, plus the existing public demo
image. See [SCREENSHOTS.md](SCREENSHOTS.md) for the isolated capture workflow,
numbered HTML/CSS annotations, source-only labels and bilingual update checks.
`guide-images.json` records the original hashes, crops, captions and placements.
