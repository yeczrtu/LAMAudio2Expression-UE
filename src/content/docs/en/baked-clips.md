---
title: "Bake SoundWave lip sync for UE5 playback"
description: "Bake a UE5 SoundWave into saved ARKit 52 curves and play lip sync in Blueprint without runtime analysis. Learn v0.3.0 generation, regeneration, preloading, cooking, and remaining latency limits."
sidebar: {"label":"Bake SoundWave clips"}
appliesTo: "v0.3.0 · UE 5.8.2 / Win64"
sourceSummary: "Reviewed on 2026-09-30 against v0.3.0 release metadata. Baked clips ship in v0.3.0, built from Plugin 1f06ac8 and Demo f3b6f13. Earlier source-test results are labeled separately."
sources:
  - label: "Plugin / Docs/BAKED_CLIPS.md · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"
  - label: "Plugin / LAMBakedExpressionClip.cpp · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2Expression/Private/LAMBakedExpressionClip.cpp"
  - label: "Plugin / LAMBakeSubsystem.h · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2ExpressionEditor/Public/LAMBakeSubsystem.h"
  - label: "Demo / Docs/DEVELOPMENT.md · f3b6f13"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/DEVELOPMENT.md"
  - label: "Demo / baked-clips-results.json · f3b6f13"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/Validation/baked-clips-results.json"
---

For recorded dialogue in **Unreal Engine 5 (UE5)**, analyze a SoundWave once in the editor and save its ARKit 52 facial curves as a Clip asset. Play the loaded clip with the existing Play Expression Clip node to skip model loading, analysis decoding, and inference during playback. This removes the analysis wait, while asset loading and audio output can still introduce latency.

:::note[Included in v0.3.0]
Install the [model-included v0.3.0 plugin](/LAMAudio2Expression-UE/en/installation/) or UE project, enable the plugin, and restart UE 5.8.2. A source build is not required for the supported release configuration. These ZIPs use **Plugin 1f06ac8 / Demo f3b6f13**. [Release manifest](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json)
:::

<span id="使い分けと対応範囲" class="comparison-anchor" aria-hidden="true"></span>

## Choose a workflow and check support

| Use case | Workflow |
| --- | --- |
| Replay dialogue known in advance | Saved clips described here |
| Analyze a SoundWave during gameplay | Existing [Analyze SoundWave Async](/LAMAudio2Expression-UE/en/blueprint/) |
| Process microphone or external PCM continuously | [Live input](/LAMAudio2Expression-UE/en/live-input/) |

Initial support is **UE 5.8.2 / Win64**, with standard SoundWave assets, mono / stereo, 8–192 kHz, up to 300 seconds, non-looping playback at 1× speed. Import external WAV / MP3 files into UE first. Existing dynamic analysis and live input remain available.

Clips store float32 ARKit 52 curves at 30 fps. The curve array alone uses **about 366 KiB per minute**, excluding audio and metadata. Visemes are not baked. When using the published source's Viseme conversion, you can change the method, templates, input correction, and strengths during playback; its CPU cost remains.

<span id="エディタでclipを生成する" class="comparison-anchor" aria-hidden="true"></span>

## Generate clips in the editor

1. Select one or more standard **SoundWave** assets in the Content Browser.
2. Right-click and choose **Generate LAM Expression Clip**.
3. Configure **Style / Smooth / Suppress Silent Mouth / Symmetrize / Auto Blink / Blink Seed**.
4. Click **Generate** and wait for completion. Errors appear in the window and Output Log.
5. Use **Save All** to write the generated assets to disk.

Style is an index from 0 to 11, not a set of 12 emotion labels. See [expression curves](/LAMAudio2Expression-UE/en/expression-curves/) for the settings. Generation completing does not save the asset to disk.

The output is `<SoundWaveName>_LAMClip` in the source audio's folder. An existing output clip for the same sound is updated. A name occupied by another asset type or another source sound receives a suffix such as `_1` or `_2`. **Duplicate or rename the existing clip first to retain a different settings variant.**

Batches process one sound at a time, record failures, and continue to the next item. Generation is unavailable during PIE. **Cancel** or closing the window stops the remaining work. An inference call already running finishes, but its result is discarded. Completed clips and the previous data of clips whose update failed are retained.

<!-- guide:bake-generate-menu:start -->
<figure class="guide-figure" id="figure-bake-generate-menu" data-guide="bake-generate-menu">
<div class="guide-shot" style="--shot-ratio:340/84;--shot-width:458.52941%;--shot-left:-116.17647%;--shot-top:-194.04762%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-generate-menu.jpg" width="1559" height="971" loading="lazy" decoding="async" alt="Generate LAM Expression Clip in the SoundWave context menu." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:5.58824%;--y:41.66667%;--w:91.47059%;--h:38.09524%"><b>1</b></span>
</div>
<figcaption>
<p><strong>Start generation from the SoundWave context menu</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-generate-menu.jpg">Open full-size image</a></p>
<ol>
<li>Select a SoundWave in the Content Browser and choose this command.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-generate-menu:end -->

<!-- guide:bake-settings:start -->
<figure class="guide-figure" id="figure-bake-settings" data-guide="bake-settings">
<div class="guide-shot" style="--shot-ratio:356/170;--shot-width:147.75281%;--shot-left:-2.24719%;--shot-top:-51.17647%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg" width="526" height="580" loading="lazy" decoding="async" alt="Generation settings: Style 0, Smooth and Suppress Silent Mouth enabled, Symmetrize and Auto Blink disabled, Blink Seed 1234." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:2.94118%;--w:93.53933%;--h:13.52941%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:17.64706%;--w:93.53933%;--h:46.47059%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:64.70588%;--w:93.53933%;--h:31.17647%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Analysis settings stored with the clip</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg">Open full-size image</a></p>
<ol>
<li>Style is a speaker-style index from 0 to 11, not an emotion name.</li>
<li>This example enables Smooth and Suppress Silent Mouth and disables Symmetrize.</li>
<li>Auto Blink is disabled. Blink Seed 1234 is the seed used for automatic blinking.</li>
</ol>
<p>After setting the options, click Generate at the bottom of the dialog. This generation dialog is included in v0.3.0.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-settings:end -->

<!-- guide:bake-complete:start -->
<figure class="guide-figure" id="figure-bake-complete" data-guide="bake-complete">
<div class="guide-shot" style="--shot-ratio:512/76;--shot-width:102.73438%;--shot-left:-1.36719%;--shot-top:-351.31579%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-complete.jpg" width="526" height="580" loading="lazy" decoding="async" alt="Actual completion message: Finished. 1 clips, 0 failures. Save generated assets to keep them." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:1.75781%;--y:48.68421%;--w:95.70313%;--h:40.78947%"><b>1</b></span>
</div>
<figcaption>
<p><strong>Check generation completion</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-complete.jpg">Open full-size image</a></p>
<ol>
<li>This captured operation completed one clip with no failures. Saving is still required.</li>
</ol>
<p>This illustrates generation of one audio asset, not a new benchmark or comprehensive validation result.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-complete:end -->

<!-- guide:bake-save:start -->
<figure class="guide-figure" id="figure-bake-save" data-guide="bake-save">
<div class="guide-shot" style="--shot-ratio:646/532;--shot-width:100%;--shot-left:0%;--shot-top:0%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-save.jpg" width="646" height="532" loading="lazy" decoding="async" alt="Save Content dialog with speech_stream_LAMClip checked and the Save Selected button visible." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.47678%;--y:18.04511%;--w:95.51084%;--h:5.45113%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:64.08669%;--y:91.54135%;--w:20.27864%;--h:5.45113%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Save the generated clip to disk</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-save.jpg">Open full-size image</a></p>
<ol>
<li>Click Save All and check that the generated speech_stream_LAMClip is selected.</li>
<li>Confirm with Save Selected.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-save:end -->

<span id="blueprintで先読みして再生する" class="comparison-anchor" aria-hidden="true"></span>

## Preload and play in Blueprint

The generated asset is a `ULAMBakedExpressionClip`, derived from `ULAMExpressionClip`. Assign it directly to existing Clip input pins. You do not need to call `Analyze SoundWave Async` before playback.

```text
Loading screen / BeginPlay
  → Load the saved Clip and its source SoundWave
  → Prime Sound (source SoundWave)

Later playback event
  → Play Expression Clip (saved Clip)

AnimGraph
  Existing pose → Apply LAM ARKit Curves → Output Pose
```

Connect the actor's LAM component to **Source Component** in the AnimBP. The existing [Blueprint setup](/LAMAudio2Expression-UE/en/blueprint/) and [Curve Profile](/LAMAudio2Expression-UE/en/expression-curves/) still apply. A Viseme-based setup uses the corresponding `Apply LAM Viseme Curves` node.

A hard reference to the clip also makes its source SoundWave a loading dependency. With a **Soft Object Reference**, wait for `Async Load Asset` to complete and retain the loaded clip in a variable or another reference. Call `Prime Sound` early for streaming audio and consider the sound's **Retain On Load** setting.

:::caution[Preloading and analysis waits are different]
`Prime Sound` requests asynchronous preloading; it is not a load-completion notification. Call it early, such as on a loading screen, rather than immediately before playback. **Zero analysis wait** means playback of a loaded clip does not wait for analysis. Audio decoding, storage, audio output buffers, and rendering can still introduce latency.
:::

`Play Expression Clip With Settings`, Pause / Resume / Seek / Stop, volume, fades, 2D / 3D, submixes, and playback events use the [existing playback controls](/LAMAudio2Expression-UE/en/playback/). Playback shares the saved curve data and interpolates 52 values per frame; it does not copy the entire array on every play.

<!-- guide:baked-blueprint:start -->
<figure class="guide-figure" id="figure-baked-blueprint" data-guide="baked-blueprint">
<div class="guide-shot" style="--shot-ratio:674/516;--shot-width:303.85757%;--shot-left:-79.97033%;--shot-top:-36.24031%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/baked-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint priming speech_stream on BeginPlay and playing its saved clip from Space Bar with Play Expression Clip." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:49.85163%;--y:2.90698%;--w:36.20178%;--h:24.03101%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.70326%;--y:48.83721%;--w:48.07122%;--h:40.89147%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Prime audio and play a saved clip</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/baked-blueprint.jpg">Open full-size image</a></p>
<ol>
<li>Pass the original SoundWave to Prime Sound and request priming early.</li>
<li>Set Clip on Play Expression Clip to the saved speech_stream_LAMClip.</li>
</ol>
<p>Asset names are truncated by the field width. Prime Sound does not signal completion. An Actor using keyboard events must be configured to receive input.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:baked-blueprint:end -->

<span id="音声やモデルを変更したら再生成する" class="comparison-anchor" aria-hidden="true"></span>

## Regenerate after changing audio or models

Right-click a clip and choose **Regenerate** to reuse that clip's recorded analysis settings. To change the settings, use **Generate LAM Expression Clip** on the source sound instead.

Clips record the sound update GUID, input PCM SHA-1, model GUID, model path, and processing/format versions. After reimporting audio, changing its processing or compression settings, or updating the model, run UE's standard **Validate Assets**, regenerate, and save. Editor validation may load the model. Playback does not automatically regenerate stale clips.

If audio or the model changes during analysis, the result is discarded. Loading also validates the format version, sound reference, duration, frame and curve counts, and value validity. Corrupt or unsupported data triggers `On Playback Failed` with **InvalidBakedClip** in `Error.Code` and a reason. It does not automatically fall back to dynamic analysis.

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

<span id="cookとパッケージ化" class="comparison-anchor" aria-hidden="true"></span>

## Cook and package

1. Save the generated clips and their source SoundWaves.
2. Reference clips from Blueprint or another asset so they are included in the cook.
3. For management through soft references alone, include assets explicitly through Asset Manager or **Additional Asset Directories to Cook**.
4. Check playback in cooked Development / Shipping builds.

**Model packaging settings are unchanged.** Saved-clip playback does not load the model, but this feature does not provide model stripping to reduce distribution size. Generation still requires the model, as do dynamic analysis and live input. Keep the existing [model and packaging](/LAMAudio2Expression-UE/en/models-and-packaging/) configuration.

<span id="editor-utilityで一括処理する" class="comparison-anchor" aria-hidden="true"></span>

## Batch processing with Editor Utilities

Obtain the `LAMBakeSubsystem` editor subsystem from an Editor Utility Blueprint or Python.

| API | Purpose |
| --- | --- |
| `GenerateClips(Sounds, Settings)` | Generate clips for SoundWaves with the supplied settings |
| `RegenerateClips(Clips)` | Regenerate using the recorded settings |
| `IsBusy` / `GetProgress` / `GetStatus` | Inspect progress |
| `GeneratedClips` / `Errors` | Retrieve completed clips and errors |
| `Cancel` | Cancel unfinished work |

A `true` return from a start function means the request was accepted, not that generation succeeded or assets were saved. A busy subsystem, PIE, or an empty selection prevents starting. Allow the editor to tick and check status from a timer or asynchronous callback. **Waiting in a synchronous Python loop prevents processing from progressing.** Check `Errors` and explicitly save generated assets after completion. [API declaration](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2ExpressionEditor/Public/LAMBakeSubsystem.h)

<span id="サンプルと検証手順" class="comparison-anchor" aria-hidden="true"></span>

## Examples and validation commands

Build the Editor target of Demo f3b6f13, then run the following from its project root. Use a test checkout: these tests generate and save `/Game/Audio/*_LAMClip`, `/Game/Examples/BP_LAMBakedPlayback`, and a test map.

```powershell
./Tools/test_baked_clips.ps1 -Engine D:\Unreal\UE_5.8
./Tools/package.ps1 -Configuration Development -IncludeBaked
./Tools/package.ps1 -Configuration Shipping -IncludeBaked
./Tools/test_playback_controls.ps1 -Configuration Development -IncludeBaked
./Tools/test_playback_controls.ps1 -Configuration Shipping -IncludeBaked
```

Place the generated `BP_LAMBakedPlayback` in a level to preload on BeginPlay and play with **Space**. `Tools/build_baked_examples.py` uses the previously generated `/Game/Audio/speech_stream_LAMClip` and does not overwrite an existing Blueprint of the same name. Connect your own character mesh and AnimBP. Existing dynamic-analysis examples remain available.

The published 2026-09-28 record reports 12 successful automation tests and 18 playback cases across Editor / Development / Shipping. See [baked-clip validation](/LAMAudio2Expression-UE/en/validation/#baked-clips) for numerical comparisons, measurements, and untested areas. These UE tests were not rerun when writing this guide.

<span id="困ったとき" class="comparison-anchor" aria-hidden="true"></span>

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Generation menu is missing | Install and enable v0.3.0, restart the editor, and select a SoundWave |
| Generation cannot start | Exit PIE, wait for another batch to finish, and check model settings and Output Log |
| Clip disappears after restarting | Use Save All after generation |
| Audio changes but expressions do not | Validate Assets → Regenerate → Save All. Generate from the source sound to change settings |
| Playback takes time to start | Check clip loading, audio preloading, storage, and audio buffers separately from analysis waits |
| `InvalidBakedClip` | Read the reason, then regenerate, save, and recook with a supported version |
| Failure occurs only in a packaged build | Check cooking of both Clip and SoundWave and completion of soft-reference loading |

See [troubleshooting](/LAMAudio2Expression-UE/en/troubleshooting/) for general audio, expression, and model issues.
