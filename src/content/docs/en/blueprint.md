---
title: "Connect audio and facial animation in Blueprint"
description: "Wire Analyze SoundWave Async to Play Expression Clip and Apply LAM ARKit Curves. A practical Blueprint and AnimGraph guide for audio-driven Unreal Engine lip sync."
sidebar: {"label":"Blueprint connections"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

Connect audio analysis and playback as separate steps. Your Actor needs a `LAMAudio2ExpressionComponent`, and the displayed mesh needs an Animation Blueprint.

## Analyze a SoundWave

```text
BeginPlay or another event
  → Analyze SoundWave Async(Component, SoundWave, Settings)
      Completed(Clip) → Play Expression Clip(Clip, StartTime=0)
      Progress       → Update loading UI
      Failed         → Display Error
      Cancelled      → Close loading UI
```

Supported inputs are standard mono / stereo SoundWaves, 8–192 kHz, up to 300 seconds. External WAV / MP3 files cannot be passed directly: import them into UE as SoundWaves first.

Pass the Clip output of `Completed` to the playback node. Keep the Clip in a Blueprint variable if other Actors will reuse it. Use `Cancel Analysis` to cancel analysis. Starting another analysis on the same component cancels its previous analysis.

## Apply expressions in the AnimGraph

```text
Existing pose → Apply LAM ARKit Curves → Output Pose
```

1. Open your character's Animation Blueprint.
2. Connect `Apply LAM ARKit Curves` between the existing pose and Output Pose.
3. Set **Source Component** to the LAM component that performs analysis and playback.
4. Set **Alpha** between 0 and 1. Use a [Curve Profile](/LAMAudio2Expression-UE/en/expression-curves/) if your curve names differ.

If Source Component is empty, the node searches the SkeletalMesh's owning Actor. Specify it explicitly when that Actor has multiple LAM components.

A mesh can consume standard curve names such as `jawOpen` through matching morph targets. For a bone-driven rig, route the curves into your existing rig or Control Rig. The plugin does not infer a custom bone arrangement automatically.

## Playback and state

`Pause`, `Resume`, `Stop`, and `Seek` control audio and expressions together. Pause holds the expression; stop returns to the input pose over 100 ms. See [playback controls](/LAMAudio2Expression-UE/en/playback/).

- `Get Current Expression Frame` returns time, 52 values, Validity, and the applied Weight.
- `Get ARKit Curve Value` returns a named estimated curve value. Stop-fade Weight is separate.

## Connected examples

The distributed project includes `/Game/Examples/BP_LAMPlayback` for async analysis and playback, and `/Game/Examples/ABP_LAMCurves` for the AnimGraph. The latter uses a test skeleton; copy its node arrangement into your own AnimBP.

In the public Blueprint face-demo source, start at `02_Analyze_And_Play` in `BP_FaceDemo`. That demo implementation is newer than the v0.2.0 ZIPs. See the [demo version notes](/LAMAudio2Expression-UE/en/demo/).

## Use a saved clip (published source)

For recorded audio, [bake a SoundWave clip](/LAMAudio2Expression-UE/en/baked-clips/) in the editor and save the asset. Pass the loaded clip directly to a playback node to skip runtime analysis. This feature is in Plugin 1f06ac8 and is not included in the v0.2.0 ZIPs.
