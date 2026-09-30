# Operation-guide screenshots

The files in `public/images/guides/` are unmodified captures of Unreal Engine's English UI. The documentation crops the visible area and adds numbered boxes using HTML/CSS; the full-size link opens the original capture. Do not paint over settings, node labels, or execution results.

Capture baseline: UE 5.8.2 on Windows, plugin `1860d0e2b28ea120a804361d3c9f5c19622f05e4`, demo `9dee71de2b60058e3434478f4f78b080cee9ca93`. The capture project is a separate clone. Documentation-only sample Blueprints illustrate existing public API calls; they are not a new plugin release. A successful Blueprint compile does not establish runtime behavior or performance. Microphone capture and performance benchmarks are not run for these illustrations.

Each image's date, source commits, visible crop, numbered annotations, and bilingual captions are recorded in `guide-images.json`. Release-download links target v0.3.0, which includes baked clips and Viseme conversion. Raw captures and their commits are preserved. The captured source descriptor shows 0.2.0; the release packaging sets it to 0.3.0. The displayed version alone does not identify the source commit.

When updating a screenshot:

1. Prepare an isolated capture project from public commits. Keep the user's existing project, local settings, and edited assets untouched.
2. Capture the actual English UI. Use a small, readable viewport and avoid unrelated windows, personal paths, and tooltips over important controls.
3. Save the raw image, then edit the crop, numbered boxes, captions and placement in `guide-images.json`. Run `node scripts/sync-guide-figures.mjs` to update the committed Markdown and raw-image SHA-256 values. Keep the full-size link, nonempty localized alt text, intrinsic dimensions, and lazy loading. This is a manual authoring command, not an upstream content importer.
4. Update both translations and `guide-images.json`. Captions must distinguish example values from required settings and from measured results.
5. Run `npm run check` and `npm run build`. Inspect the figures at desktop and mobile widths in both themes; open original-image links. Check all 30 pages and both 15-page search indexes with the existing verification script.
6. Push only `codex/docs-site`, wait for Pages deployment, and confirm the public pages and image URLs.

Character images retain the credits and usage conditions described in `THIRD_PARTY_NOTICES.md` and the bilingual licenses pages. The existing demo image is reused without changing its contents or treating its displayed status as a new measurement.
