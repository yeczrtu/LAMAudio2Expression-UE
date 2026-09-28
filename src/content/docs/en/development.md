---
title: "Build from source and prepare a release"
description: "Check out the documented LAM Audio2Expression source snapshot, generate the UE model, run tests, package Windows builds, and assemble release ZIPs."
sidebar: {"label":"Development & releases"}
appliesTo: "Published source snapshot · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/DEVELOPMENT.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/DEVELOPMENT.md"},{"label":"Demo / Docs/RELEASE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/RELEASE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"}]
---

This page targets the **published source snapshot**. For immediate use, install the [model-included v0.2.0 release](/LAMAudio2Expression-UE/en/installation/). The Blueprint face demo described here is newer than the v0.2.0 ZIPs.

## Development environment

Install UE 5.8.2, Visual Studio 2022 with C++ development tools, Python 3.10, and Git. The examples assume UE is installed at `D:\Unreal\UE_5.8`; replace that with your engine path.

## Reproduce the documented source version

```powershell
git clone --recurse-submodules https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo.git
cd LAMAudio2Expression-UE-Demo
git checkout 275a683a530254451efae8409e6ec2d2f57af6bb
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

## Assemble release archives

1. Generate the model and verify hashes against `Docs/model-manifest.json`.
2. Build the Win64 plugin with `RunUAT.bat BuildPlugin`. Use a new, short output path: this command empties its output directory.
3. Create a disposable demo-project copy. Generate release examples using `Tools/prepare_release_examples.py`, copy the validated model, and produce a Shipping build with BuildCookRun.
4. Assemble the distribution. Replace the angle-bracket placeholders with actual paths:

```powershell
python Tools/assemble_release.py --plugin <BuildPlugin-output> --project <disposable-project-directory> --shipping <Shipping-archive>/Windows --output <new-output-directory>
```

The new output directory receives plugin, editable-project, and Windows-demo ZIPs, plus a manifest and checksums. Extract each archive to another short path and verify the model, face demo, and playback controls before publishing matching version tags in both repositories. The linked release source below provides the detailed rules for the disposable project copy.
