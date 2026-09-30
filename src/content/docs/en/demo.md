---
title: "Run the Windows demo or open the UE project"
description: "Try six Japanese speech samples in the LAM Audio2Expression Windows demo. Learn the controls and the differences between v0.2.0 and the Blueprint source demo."
sidebar: {"label":"Try the demo"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"}]
---

## Run the Windows application

1. Download the [model-included Windows demo](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-Win64-Demo.zip).
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

Extract the entire [model-included UE project](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Project-Model.zip). Open `LAMDemo.uproject` in UE 5.8.2, load `/Game/LAMFaceDemo/Maps/LAM_FaceDemo`, and press Play.

See the [credits and terms](/LAMAudio2Expression-UE/en/licenses/) for the screenshot and demo assets.

## Release demo versus Blueprint source demo

:::note[The v0.2.0 ZIPs and video]
The release archives and recording predate the migration of the demo logic to Blueprint. The following Blueprint structure describes public source commit `275a683`. Follow the [development guide](/LAMAudio2Expression-UE/en/development/) to obtain and build that source.
:::

Start with `02_Analyze_And_Play` in `BP_FaceDemo`: `SelectSample` → cancel previous work → `Analyze SoundWave Async` → `Completed` → `Play Expression Clip`.

| Blueprint | Responsibility |
| --- | --- |
| BP_FaceDemo | Analysis, playback, input, state events |
| BP_FaceDemoHUD | Canvas-based UI and click handling |
| BP_FaceDemoGameMode | Selects the HUD |
| ABP_Face52 | Applies expressions with Apply LAM ARKit Curves |

These assets live under `Content/LAMFaceDemo`. To move the demo to another project, install the plugin and use Content Browser's Migrate command. Keep the asset attribution and licenses. You do not need the demo assets when integrating your own character.
