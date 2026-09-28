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

## Edit the UE project

Extract the entire [model-included UE project](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Project-Model.zip). Open `LAMDemo.uproject` in UE 5.8.2, load `/Game/LAMFaceDemo/Maps/LAM_FaceDemo`, and press Play.

![The face demo showing a character and speech selection controls in Unreal Engine](/LAMAudio2Expression-UE/images/face-demo.png)

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
