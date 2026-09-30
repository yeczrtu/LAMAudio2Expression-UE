---
title: "Models and Windows packaging"
description: "Configure Unreal Engine cooking for the LAM Audio2Expression model. Understand CPU and DirectML data, model provenance, release checksums, and redistribution notices."
sidebar: {"label":"Models & packaging"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/RELEASE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/RELEASE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

## Model location

The model-included ZIP contains a neural model asset of roughly 384 MiB. Its default reference is `/LAMAudio2Expression/Models/LAM_A2E`. UE uses the ONNX stored in the NNE asset and cooked CPU / DirectML data.

Models are not stored in the normal Git history. For a source checkout, follow [development setup](/LAMAudio2Expression-UE/en/development/) to retrieve and convert a pinned checkpoint, or use the published model-included package.

<!-- guide:model-settings:start -->
<figure class="guide-figure" id="figure-model-settings" data-guide="model-settings">
<div class="guide-shot" style="--shot-ratio:700/195;--shot-width:134%;--shot-left:-31.14286%;--shot-top:-99.48718%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/model-settings.jpg" width="938" height="494" loading="lazy" decoding="async" alt="LAM Audio2Expression settings showing LAM_A2E, Prefer GPU enabled and Cache MiB set to 64." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:57.85714%;--y:17.4359%;--w:40.14286%;--h:34.87179%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:58.42857%;--y:52.82051%;--w:18.85714%;--h:40.51282%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Select the model in Project Settings</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/model-settings.jpg">Open full-size image</a></p>
<ol>
<li>Select /LAMAudio2Expression/Models/LAM_A2E in Model.</li>
<li>The capture shows Prefer GPU enabled and Cache MiB 64.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:model-settings:end -->

<!-- guide:plugin-content:start -->
<figure class="guide-figure" id="figure-plugin-content" data-guide="plugin-content">
<div class="guide-shot" style="--shot-ratio:222/155;--shot-width:100%;--shot-left:0%;--shot-top:-188.3871%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/plugin-content.jpg" width="222" height="746" loading="lazy" decoding="async" alt="Content Browser Settings with Show Plugin Content enabled." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.6036%;--y:70.32258%;--w:91.89189%;--h:18.70968%"><b>1</b></span>
</div>
<figcaption>
<p><strong>Show plugin assets in the Content Browser</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/plugin-content.jpg">Open full-size image</a></p>
<ol>
<li>Open Settings at the top right of the Content Browser and enable Show Plugin Content.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:plugin-content:end -->

## Cooking checklist

1. Confirm the Model setting in Project Settings → LAM Audio2Expression.
2. Add `/LAMAudio2Expression/Models` to Packaging → Additional Asset Directories to Cook.
3. When packaging the demo, add `/Game/LAMFaceDemo/Maps/LAM_FaceDemo` to Maps to Cook.
4. Keep the plugin's `Source` and `Intermediate/Build` directories.
5. Copy or extract the complete output to another short path and test the packaged application.

Distribute the entire folder, including cooked data, required DLLs, and licenses, rather than the executable alone. The published Windows demo also includes a VC++ runtime installer.

<!-- guide:cook-settings:start -->
<figure class="guide-figure" id="figure-cook-settings" data-guide="cook-settings">
<div class="guide-shot" style="--shot-ratio:702/68;--shot-width:133.61823%;--shot-left:-31.05413%;--shot-top:-438.23529%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/cook-settings.jpg" width="938" height="494" loading="lazy" decoding="async" alt="Packaging with /LAMAudio2Expression/Models listed under Additional Asset Directories to Cook." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.5641%;--y:5.88235%;--w:93.73219%;--h:38.23529%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:57.69231%;--y:48.52941%;--w:30.76923%;--h:39.70588%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Include the model folder when cooking</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/cook-settings.jpg">Open full-size image</a></p>
<ol>
<li>Open Additional Asset Directories to Cook under Packaging.</li>
<li>Add the model folder /LAMAudio2Expression/Models.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:cook-settings:end -->

## GPU and CPU

DirectML is preferred, with CPU fallback when unavailable. Show model loading and initialization as part of your application's loading state. The tested fallback path covers an unavailable GPU model; it is not a reproduction of physical GPU failure. See [validation](/LAMAudio2Expression-UE/en/validation/).

## Versions and checksums

The [model manifest](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/model-manifest.json) records provenance, pinned revisions, shapes, and SHA-256 hashes. Releases attach `release-manifest.json` and `SHA256SUMS.txt`.

Record plugin, UE, and model versions together with hashes. Prefer a new release version over silently replacing a published ZIP.

## Distribution notices

MIT for original code and Apache-2.0 for upstream-derived code and model weights have different scopes. Preserve `LICENSE`, `THIRD_PARTY_NOTICES.md`, `Licenses`, model provenance, and conversion notes. Demo audio, visual presentation, and character assets have separate terms. See [licenses and credits](/LAMAudio2Expression-UE/en/licenses/).

## Cook saved clips (v0.3.0)

Save Plugin 1f06ac8 [baked clips](/LAMAudio2Expression-UE/en/baked-clips/) after generation and include both clips and source SoundWaves in the cook. Include soft-reference-only assets explicitly, for example through Asset Manager. Saved-clip playback does not load the model, but existing model packaging settings remain unchanged. This feature is included in the v0.3.0 ZIPs.
