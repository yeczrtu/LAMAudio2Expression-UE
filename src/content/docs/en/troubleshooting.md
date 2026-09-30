---
title: "Troubleshoot setup and lip sync"
description: "Resolve missing models, silent or motionless characters, rejected audio, slow startup, and delayed live input in LAM Audio2Expression for Unreal Engine."
sidebar: {"label":"Troubleshooting"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/RELEASE_INSTALL.md"},{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / README.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/README.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

## The model cannot be found

Check that you downloaded the **model-included release ZIP**, not GitHub's “Source code” archive. The Model setting should be `/LAMAudio2Expression/Models/LAM_A2E`.

If it works in the editor but fails in a packaged build, add `/LAMAudio2Expression/Models` to Packaging → Additional Asset Directories to Cook. See [installation](/LAMAudio2Expression-UE/en/installation/).

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

## Audio plays but the face does not move

1. Confirm `Apply LAM ARKit Curves` is connected in the character's AnimBP.
2. Confirm Source Component references the component performing analysis and playback.
3. Check that Alpha is nonzero and Curve Profile has not disabled the required curves.
4. Check for matching morph targets such as `jawOpen`, or that your rig consumes the curves.

Custom bone-driven rigs are not converted automatically. See [expression curves](/LAMAudio2Expression-UE/en/expression-curves/).

<!-- guide:animgraph:start -->
<figure class="guide-figure" id="figure-animgraph" data-guide="animgraph">
<div class="guide-shot" style="--shot-ratio:753/219;--shot-width:271.97875%;--shot-left:-73.70518%;--shot-top:-177.62557%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/animgraph.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Actual AnimGraph connecting Local Space Ref Pose, Apply LAM ARKit Curves and Output Pose." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.39044%;--y:8.21918%;--w:24.9668%;--h:34.7032%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:36.78619%;--y:6.84932%;--w:28.15405%;--h:65.2968%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:68.6587%;--y:7.76256%;--w:29.08367%;--h:85.84475%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Apply ARKit expression curves to the input pose</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/animgraph.jpg">Open full-size image</a></p>
<ol>
<li>Ref Pose is a placeholder in this example. Supply your existing animation pose.</li>
<li>Assign the analysis/playback LAM to Source Component. If empty, the owning Actor is searched.</li>
<li>Connect the output to Output Pose.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:animgraph:end -->

## Analysis is rejected or an audio input cannot be used

The supported input is a standard mono / stereo SoundWave, 8–192 kHz, up to 300 seconds. SoundCue, MetaSound, Procedural SoundWave, and direct external WAV / MP3 input are unsupported. Loop-enabled SoundWaves are also rejected for playback.

Record the Error from async analysis `Failed` and playback `On Playback Failed`. Starting another analysis on the same component cancels the previous one. See [Blueprint connections](/LAMAudio2Expression-UE/en/blueprint/).

## The first analysis takes longer

Model loading and NNE instance initialization take time. Connect `Progress` to a loading indicator and start playback after `Completed`. Distinguish first-use timing from a fixed window after initialization. See [validation](/LAMAudio2Expression-UE/en/validation/).

## Microphone or live input fails or falls behind

Check Windows microphone permissions, the recording device, and `On Live State Changed`. Inspect initialization, result-latency P95, and dropped intervals with `Get Live Metrics`. Avoid queuing many offline analyses on the shared worker. Supply PCM in small chunks from the game thread.

Microphone audio is not monitored through speakers. Hearing no audio alone does not mean capture failed. See [live input](/LAMAudio2Expression-UE/en/live-input/).

## Advance dialogue on natural completion

Use `On Playback Finished`. `On Playback Ended` also fires for stops and replacements, so do not advance dialogue unconditionally from it. Identify the relevant playback using the event's Playback Id. See [playback controls](/LAMAudio2Expression-UE/en/playback/).

## Report a problem

Open a [plugin issue](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues) or a [demo issue](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/issues). Include the plugin version, UE version, Editor / Shipping configuration, CPU / GPU, reproduction steps, and Error.Code / Error.Message. State whether you are using a release archive or a source build.

## Baked-clip issues (v0.3.0)

If the generation menu is missing, check that v0.3.0 is installed and enabled, then restart the editor. See [baked-clip troubleshooting](/LAMAudio2Expression-UE/en/baked-clips/#troubleshooting) for `InvalidBakedClip`, saving generated assets, regeneration, and missing cooked assets.

<!-- guide:bake-regenerate:start -->
<figure class="guide-figure" id="figure-bake-regenerate" data-guide="bake-regenerate">
<div class="guide-shot" style="--shot-ratio:468/241;--shot-width:333.11966%;--shot-left:-51.28205%;--shot-top:-139.41909%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-regenerate.jpg" width="1559" height="971" loading="lazy" decoding="async" alt="speech_stream_LAMClip in the Content Browser with Regenerate in its context menu." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:35.89744%;--y:39.41909%;--w:59.40171%;--h:12.44813%"><b>1</b></span>
</div>
<figcaption>
<p><strong>Regenerate a saved clip</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-regenerate.jpg">Open full-size image</a></p>
<ol>
<li>Right-click the clip and choose Regenerate to use its stored settings.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-regenerate:end -->

## Viseme curves do not move the mouth

Check Profile Target Names against the mesh's actual morph names. FiveVowelRules outputs zero for the nine consonant slots; use TemplateFit for a compatible 15-slot rig. Avoid applying full ARKit mouth curves and Viseme curves to the same mouth. For missing input channels in TemplateFit use FitWeight=0, not Scale=0. See [Viseme setup and limitations](/LAMAudio2Expression-UE/en/expression-curves/#visemes).
