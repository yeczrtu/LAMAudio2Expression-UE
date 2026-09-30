---
title: "Build from source and prepare a release"
description: "Check out the documented LAM Audio2Expression source snapshot, generate the UE model, run tests, package Windows builds, and assemble release ZIPs."
sidebar: {"label":"Development & releases"}
appliesTo: "v0.3.0 / Plugin 1860d0e / Demo 9dee71d · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/DEVELOPMENT.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/DEVELOPMENT.md"},{"label":"Demo / Docs/RELEASE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/RELEASE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

This page targets the **published source snapshot**. For immediate use, install the [model-included v0.3.0 release](/LAMAudio2Expression-UE/en/installation/). The Blueprint face demo, baked clips, and Viseme conversion are included in v0.3.0.

## Development environment

Install UE 5.8.2, Visual Studio 2022 with C++ development tools, Python 3.10, and Git. The examples assume UE is installed at `D:\Unreal\UE_5.8`; replace that with your engine path.

## Reproduce the documented source version

```powershell
git clone --recurse-submodules https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo.git
cd LAMAudio2Expression-UE-Demo
git checkout 9dee71de2b60058e3434478f4f78b080cee9ca93
git submodule update --init --recursive
./Tools/setup.ps1 -Engine D:\Unreal\UE_5.8
```

This checkout reproduces the referenced version. Create a working branch if you want to save changes. Existing clones also need their submodules initialized.

Setup uses a dedicated `.work/venv`. It retrieves the pinned model, verifies SHA-256, exports ONNX, compares numerical output, and generates the UE model and test assets. Python, PyTorch, and network access are development-time requirements; the distributed application's inference does not need them.

:::caution[Generated test assets]
Setup regenerates test data at `/Game/LAMDemo` and `/Game/Audio`. Back up edits there first. It does not regenerate the face demo itself.
:::

## Build and test

Run these commands from the project root:

```powershell
./Tools/test.ps1 -Engine D:\Unreal\UE_5.8
./Tools/package.ps1 -Configuration Development
./Tools/package.ps1 -Configuration Shipping
./Tools/smoke.ps1 -Configuration Development
./Tools/smoke.ps1 -Configuration Shipping
./Tools/test_playback_controls.ps1 -Configuration Shipping
./Tools/test_face_demo.ps1 -Configuration Shipping -OutputDirectory Artifacts/BPChecks-Shipping
```

Playback-control and face-demo tests also support Editor and Development configurations. The Shipping executable is `Artifacts/Shipping/Windows/LAMDemo.exe`. Distribute the entire Windows folder.

The checkout above pins current public Demo `9dee71d` and Plugin `1860d0e`. To reproduce the v0.3.0 release source instead, checkout Demo `f3b6f13e98d75d6e23933669adee79319ef3363f`, then run `git submodule update --init --recursive`; its plugin is `1f06ac858413090f00c3f5bb955e1d73653ef74e`. The later plugin adds Lipsync search keywords and README updates.

## Assemble release archives

1. Generate the model and verify hashes against `Docs/model-manifest.json`.
2. Build the Win64 plugin with `RunUAT.bat BuildPlugin`. Use a new, short output path: this command empties its output directory.
3. Create a disposable demo-project copy. Generate release examples using `Tools/prepare_release_examples.py`, copy the validated model, and produce a Shipping build with BuildCookRun.
4. Assemble the distribution. Replace the angle-bracket placeholders with actual paths:

```powershell
python Tools/assemble_release.py --version 0.3.0 --plugin <BuildPlugin-output> --project <disposable-project-directory> --shipping <Shipping-archive>/Windows --output <new-output-directory>
```

The source script still defaults to 0.2.0, so pass `--version 0.3.0` explicitly. The [published release manifest](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json) records packaging-only edits: `Version=3`, `VersionName=0.3.0`, BuildPlugin's `Installed=true`, and the README-FIRST release label. Apply and check those changes only in the disposable distribution copy, including the script's hard-coded README-FIRST text / URL. The release used unchanged C++ sources. Inspect every assembled archive and record its source commits, validation, and SHA-256; the version flag alone is not a complete reproduction of the release packaging.


The new output directory receives plugin, editable-project, and Windows-demo ZIPs, plus a manifest and checksums. Extract each archive to another short path and verify the model, face demo, and playback controls before publishing matching version tags in both repositories. The linked release source below provides the detailed rules for the disposable project copy.

## Develop and validate baked clips (v0.3.0)

This feature was added in Plugin 1f06ac8 / Demo f3b6f13. Build the Editor target before running dedicated checks for generation, regeneration, saving, and playback in another process. The new `-IncludeBaked` option includes the test map in Development / Shipping packages. See [commands and generated assets](/LAMAudio2Expression-UE/en/baked-clips/#examples-and-validation-commands). The v0.3.0 release ZIPs include this feature.

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
