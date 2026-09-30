---
title: "Run the Windows demo or open the UE project"
description: "Use the v0.3.0 Windows demo and UE project: six Japanese samples, playback controls, the included Blueprint face demo, and Viseme examples."
sidebar: {"label":"Try the demo"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / README.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/README.md"},{"label":"Demo / Docs/FACE_DEMO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/FACE_DEMO.md"},{"label":"Demo / Docs/VIDEO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/VIDEO.md"}]
---

## Run the Windows application

1. Download the [model-included Windows demo](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-Win64-Demo.zip).
2. Extract **the entire archive** to a short path such as `C:\LAMDemo` and launch `LAMDemo.exe`.
3. Choose a speech button on the left, or press **1–6**. Expressions and audio play after analysis completes.

The model, plugin, character, and six speech samples are included. You do not need the UE editor or Python. If the VC++ runtime is missing, run the included `Engine/Extras/Redist/en-us/vc_redist.x64.exe`.

## Controls

| Key | Action |
| --- | --- |
| 1–6 | Select a speech sample |
| Space | Pause / resume |
| R | Replay from the beginning |
| V / − / ＋ | Mute / lower volume / raise volume |
| O / F | Switch output submix / fade out and stop |
| M / I | Start or stop microphone / cycle 100, 333, 1000 ms inference intervals |
| Alt + F4 | Exit |

Microphone input needs Windows microphone permission. The plugin does not monitor the captured audio through speakers. Long-running use with a physical microphone remains unverified.

<!-- guide:demo-controls:start -->
<figure class="guide-figure" id="figure-demo-controls" data-guide="demo-controls">
<div class="guide-shot" style="--shot-ratio:888/500;--shot-width:100%;--shot-left:0%;--shot-top:0%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/face-demo.png" width="888" height="500" loading="lazy" decoding="async" alt="Existing actual face-demo screen showing voice selection, Space and R playback controls, volume and live status." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.7027%;--y:18.2%;--w:21.05856%;--h:38.6%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.59009%;--y:59.2%;--w:21.28378%;--h:14.4%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:26.68919%;--y:3.8%;--w:21.84685%;--h:36.6%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Controls in the published demo screenshot</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/face-demo.png">Open full-size image</a></p>
<ol>
<li>Select audio using buttons 1–6. Their labels classify the speech samples.</li>
<li>Space pauses/resumes; R replays from the beginning.</li>
<li>Inspect volume, output and live status. The values belong to the existing capture, not a new measurement.</li>
</ol>
<p>Character: hinzka / VRoid and pixiv. Speech: JVNV / litagin. See the licenses page for usage conditions.</p>
<p class="guide-provenance">Existing public image (capture date unknown), reused unchanged from Demo 275a683. <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/face-demo.png">Image source</a> · <a href="/LAMAudio2Expression-UE/en/licenses/">Credits and usage conditions</a></p>
</figcaption>
</figure>
<!-- guide:demo-controls:end -->

## Edit the UE project

Extract the entire [model-included UE project](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Project-Model.zip). Open `LAMDemo.uproject` in UE 5.8.2, load `/Game/LAMFaceDemo/Maps/LAM_FaceDemo`, and press Play.

See the [credits and terms](/LAMAudio2Expression-UE/en/licenses/) for the screenshot and demo assets.

## Release demo versus Blueprint source demo

:::note[v0.3.0 includes the Blueprint demo]
The v0.3.0 ZIPs use Demo `f3b6f13` and include the Blueprint UI and analysis flow below. The older v0.2.0 recording predates this migration. Latest public Demo `9dee71d` only advances the plugin reference. See the [release manifest](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json).
:::

Start with `02_Analyze_And_Play` in `BP_FaceDemo`: `SelectSample` → cancel previous work → `Analyze SoundWave Async` → `Completed` → `Play Expression Clip`.

| Blueprint | Responsibility |
| --- | --- |
| BP_FaceDemo | Analysis, playback, input, state events |
| BP_FaceDemoHUD | Canvas-based UI and click handling |
| BP_FaceDemoGameMode | Selects the HUD |
| ABP_Face52 | Applies expressions with Apply LAM ARKit Curves |

These assets live under `Content/LAMFaceDemo`. To move the demo to another project, install the plugin and use Content Browser's Migrate command. Keep the asset attribution and licenses. You do not need the demo assets when integrating your own character.

## Viseme and baked-clip examples

v0.3.0 also includes five-vowel / Oculus template examples and baked-clip examples. Normal face-demo startup still uses ARKit 52. For a vowel rig, inspect `/Game/LAMVisemeExamples/ABP_LAMVisemes` and its `Fcl_MTH_A/I/U/E/O` targets. OpenFaceFX and TalkingHead examples use `ABP_OpenFaceFX` and `ABP_TalkingHead` in that folder. The test script switches the mesh and AnimBP only when the relevant test flag is supplied; these are not extra keyboard modes in the normal HUD.

See [Viseme setup](/LAMAudio2Expression-UE/en/expression-curves/#visemes), [saved-clip examples](/LAMAudio2Expression-UE/en/baked-clips/#examples-and-validation-commands), and the [v0.3.0 validation record](/LAMAudio2Expression-UE/en/validation/#release-validation).
