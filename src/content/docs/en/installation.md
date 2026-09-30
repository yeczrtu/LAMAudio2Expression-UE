---
title: "Install the Unreal Engine plugin"
description: "Install LAM Audio2Expression v0.3.0 in UE 5.8.2. Set up the model-included ZIP, enable dependencies, configure cooking, and connect your first Blueprint."
sidebar: {"label":"Install the plugin"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / README.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/README.md"}]
---

Use this guide to add audio-driven lip sync to your own Unreal Engine project. To try the result first, use the [standalone Windows demo](/LAMAudio2Expression-UE/en/demo/).

## Requirements

- Windows x64 and **Unreal Engine 5.8.2**.
- ARKit 52 morph targets or a curve-driven rig; five-vowel and Oculus-compatible rigs can use [Viseme conversion](/LAMAudio2Expression-UE/en/expression-curves/#visemes).
- A standard SoundWave imported into Unreal Engine.

The release includes precompiled Editor Development and Game Development / Shipping data, plus C++ source. Other UE versions require a [source rebuild](/LAMAudio2Expression-UE/en/development/).

## Install the model-included ZIP

1. Download the [v0.3.0 plugin ZIP](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Win64-Model.zip).
2. Close the UE editor and extract the entire archive.
3. Place `LAMAudio2Expression` at `<Project>/Plugins/LAMAudio2Expression`. When upgrading, move the old plugin folder elsewhere before replacing it.
4. Enable **LAM Audio2Expression** in UE and restart. The required NNE ORT and AudioCapture plugins are enabled as dependencies.
5. In Project Settings → LAM Audio2Expression, confirm that Model is `/LAMAudio2Expression/Models/LAM_A2E`.

:::caution[Keep the supplied build data]
Do not remove `Source` or `Intermediate/Build`. These folders contain data required for Game builds. GitHub's “Source code” archive is not a substitute for the model-included release.
:::

<!-- guide:plugin-enabled:start -->
<figure class="guide-figure" id="figure-plugin-enabled" data-guide="plugin-enabled">
<div class="guide-shot" style="--shot-ratio:675/169;--shot-width:138.81481%;--shot-left:-36.74074%;--shot-top:-32.54438%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/plugin-enabled.jpg" width="937" height="494" loading="lazy" decoding="async" alt="Plugins filtered by LAM, with LAM Audio2Expression enabled." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:1.77778%;--y:5.91716%;--w:83.25926%;--h:16.56805%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:4%;--y:59.1716%;--w:3.40741%;--h:18.93491%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Enable LAM Audio2Expression in Plugins</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/plugin-enabled.jpg">Open full-size image</a></p>
<ol>
<li>Enter LAM in the search field.</li>
<li>Enable the checkbox and restart UE if prompted.</li>
</ol>
<p>Version 0.2.0 is the captured public-source descriptor label. The v0.3.0 release ZIP updates it to 0.3.0. The capture commit is recorded below.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:plugin-enabled:end -->

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

## Play your first clip

Add a `LAMAudio2ExpressionComponent` to an Actor. Pass it and a SoundWave to `Analyze SoundWave Async`. Connect the Clip returned by `Completed` to `Play Expression Clip`.

In the Animation Blueprint, place `Apply LAM ARKit Curves` between the existing pose and Output Pose. Set Source Component to the same component. Continue with the [Blueprint guide](/LAMAudio2Expression-UE/en/blueprint/) for the complete connection flow.

The first analysis waits for the roughly 384 MiB model to load and inference to initialize. Runtime use needs no Python, external inference server, or additional model download.

<!-- guide:add-component:start -->
<figure class="guide-figure" id="figure-add-component" data-guide="add-component">
<div class="guide-shot" style="--shot-ratio:329/157;--shot-width:622.4924%;--shot-left:0%;--shot-top:-64.33121%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/add-component.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint Components panel showing Add and the added LAM component." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.34347%;--y:18.47134%;--w:21.8845%;--h:18.47134%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:8.20669%;--y:71.97452%;--w:86.32219%;--h:17.83439%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Add a LAM component to the Actor</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/add-component.jpg">Open full-size image</a></p>
<ol>
<li>Use Add to find and add LAM Audio2Expression Component.</li>
<li>This example names the added component LAM.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:add-component:end -->

## Package your project

Add `/LAMAudio2Expression/Models` to Project Settings → Packaging → **Additional Asset Directories to Cook**. Check this first if the model works in the editor but cannot be found in a packaged application. See [models and packaging](/LAMAudio2Expression-UE/en/models-and-packaging/).

The plugin alone does not include a face, speech samples, or demo map. Use the [demo project](/LAMAudio2Expression-UE/en/demo/) for a preconfigured character.

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
