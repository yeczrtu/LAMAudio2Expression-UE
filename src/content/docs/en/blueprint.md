---
title: "Connect audio and facial animation in Blueprint"
description: "Wire Analyze SoundWave Async to Play Expression Clip and Apply LAM ARKit Curves. A practical Blueprint and AnimGraph guide for audio-driven Unreal Engine lip sync."
sidebar: {"label":"Blueprint connections"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
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

<!-- guide:blueprint-analysis:start -->
<figure class="guide-figure" id="figure-blueprint-analysis" data-guide="blueprint-analysis">
<div class="guide-shot" style="--shot-ratio:1051/318;--shot-width:194.86204%;--shot-left:-33.30162%;--shot-top:-88.99371%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/blueprint-analysis.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint connecting BeginPlay to Analyze SoundWave Async, then Completed and Clip to Play Expression Clip." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:32.35014%;--y:5.66038%;--w:28.92483%;--h:90.56604%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:68.03045%;--y:5.66038%;--w:30.63749%;--h:58.1761%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Play from the Completed output of asynchronous analysis</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/blueprint-analysis.jpg">Open full-size image</a></p>
<ol>
<li>Pass the LAM component and a SoundWave to the analysis node.</li>
<li>Connect the Completed execution pin and Clip output to the playback node.</li>
</ol>
<p>This is a minimal wiring example. Handle Failed, Cancelled and Progress in your application.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:blueprint-analysis:end -->

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

## Playback and state

`Pause`, `Resume`, `Stop`, and `Seek` control audio and expressions together. Pause holds the expression; stop returns to the input pose over 100 ms. See [playback controls](/LAMAudio2Expression-UE/en/playback/).

- `Get Current Expression Frame` returns time, 52 values, Validity, and the applied Weight.
- `Get ARKit Curve Value` returns a named estimated curve value. Stop-fade Weight is separate.

## Connected examples

The distributed project includes `/Game/Examples/BP_LAMPlayback` for async analysis and playback, and `/Game/Examples/ABP_LAMCurves` for the AnimGraph. The latter uses a test skeleton; copy its node arrangement into your own AnimBP.

In the public Blueprint face-demo source, start at `02_Analyze_And_Play` in `BP_FaceDemo`. That implementation is included in the v0.3.0 ZIPs. See the [demo version notes](/LAMAudio2Expression-UE/en/demo/).

## Use a saved clip (v0.3.0)

For recorded audio, [bake a SoundWave clip](/LAMAudio2Expression-UE/en/baked-clips/) in the editor and save the asset. Pass the loaded clip directly to a playback node to skip runtime analysis. This feature is included in the v0.3.0 ZIPs.

In latest source Plugin `1860d0e`, **Lipsync** also finds these nodes. For the v0.3.0 ZIP use **LAM** or the displayed node name. For a vowel / Oculus rig, replace the mouth application with [Apply LAM Viseme Curves](/LAMAudio2Expression-UE/en/expression-curves/#visemes).
