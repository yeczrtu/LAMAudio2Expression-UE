---
title: "Bake SoundWave analysis into saved expression clips"
description: "Generate and save ARKit 52 curves from an Unreal Engine SoundWave, then play them in Blueprint without waiting for analysis. Covers regeneration, preloading, cooking, and validation."
sidebar: {"label":"Bake SoundWave clips"}
appliesTo: "Published source 1f06ac8 / Demo f3b6f13 · UE 5.8.2 / Win64 · Not in v0.2.0"
sourceSummary: "Public sources reviewed on 2026-09-28. Baking behavior is based on Plugin 1f06ac8; examples and validation records use Demo f3b6f13. This feature is not included in the v0.2.0 release ZIPs."
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

Analyze recorded dialogue once in the editor and save its ARKit 52 facial curves as an asset. Pass the saved clip to the existing `Play Expression Clip` node in your game to skip model loading, analysis decoding, and inference during playback.

:::note[Available in the published source]
This guide targets **Plugin 1f06ac8 / Demo f3b6f13**. The feature is **not included in the v0.2.0 release ZIPs** as of 2026-09-28. Build the UE 5.8.2 Editor target from source containing this commit, then restart the editor. See [development](/LAMAudio2Expression-UE/en/development/) for model setup.
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

<span id="音声やモデルを変更したら再生成する" class="comparison-anchor" aria-hidden="true"></span>

## Regenerate after changing audio or models

Right-click a clip and choose **Regenerate** to reuse that clip's recorded analysis settings. To change the settings, use **Generate LAM Expression Clip** on the source sound instead.

Clips record the sound update GUID, input PCM SHA-1, model GUID, model path, and processing/format versions. After reimporting audio, changing its processing or compression settings, or updating the model, run UE's standard **Validate Assets**, regenerate, and save. Editor validation may load the model. Playback does not automatically regenerate stale clips.

If audio or the model changes during analysis, the result is discarded. Loading also validates the format version, sound reference, duration, frame and curve counts, and value validity. Corrupt or unsupported data triggers `On Playback Failed` with **InvalidBakedClip** in `Error.Code` and a reason. It does not automatically fall back to dynamic analysis.

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
| Generation menu is missing | Unavailable in the v0.2.0 ZIP. Build the supported source's Editor target, restart, and select a SoundWave |
| Generation cannot start | Exit PIE, wait for another batch to finish, and check model settings and Output Log |
| Clip disappears after restarting | Use Save All after generation |
| Audio changes but expressions do not | Validate Assets → Regenerate → Save All. Generate from the source sound to change settings |
| Playback takes time to start | Check clip loading, audio preloading, storage, and audio buffers separately from analysis waits |
| `InvalidBakedClip` | Read the reason, then regenerate, save, and recook with a supported version |
| Failure occurs only in a packaged build | Check cooking of both Clip and SoundWave and completion of soft-reference loading |

See [troubleshooting](/LAMAudio2Expression-UE/en/troubleshooting/) for general audio, expression, and model issues.
