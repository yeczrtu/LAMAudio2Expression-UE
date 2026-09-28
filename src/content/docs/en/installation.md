---
title: "Install the Unreal Engine plugin"
description: "Install LAM Audio2Expression v0.2.0 in UE 5.8.2. Set up the model-included ZIP, enable dependencies, configure cooking, and connect your first Blueprint."
sidebar: {"label":"Install the plugin"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / README.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"}]
---

Use this guide to add audio-driven lip sync to your own Unreal Engine project. To try the result first, use the [standalone Windows demo](/LAMAudio2Expression-UE/en/demo/).

## Requirements

- Windows x64 and **Unreal Engine 5.8.2**.
- A character with ARKit 52 morph targets, or a rig that consumes expression curves.
- A standard SoundWave imported into Unreal Engine.

The release includes precompiled Editor Development and Game Development / Shipping data, plus C++ source. Other UE versions require a [source rebuild](/LAMAudio2Expression-UE/en/development/).

## Install the model-included ZIP

1. Download the [v0.2.0 plugin ZIP](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip).
2. Close the UE editor and extract the entire archive.
3. Place `LAMAudio2Expression` at `<Project>/Plugins/LAMAudio2Expression`. When upgrading, move the old plugin folder elsewhere before replacing it.
4. Enable **LAM Audio2Expression** in UE and restart. The required NNE ORT and AudioCapture plugins are enabled as dependencies.
5. In Project Settings → LAM Audio2Expression, confirm that Model is `/LAMAudio2Expression/Models/LAM_A2E`.

:::caution[Keep the supplied build data]
Do not remove `Source` or `Intermediate/Build`. These folders contain data required for Game builds. GitHub's “Source code” archive is not a substitute for the model-included release.
:::

## Play your first clip

Add a `LAMAudio2ExpressionComponent` to an Actor. Pass it and a SoundWave to `Analyze SoundWave Async`. Connect the Clip returned by `Completed` to `Play Expression Clip`.

In the Animation Blueprint, place `Apply LAM ARKit Curves` between the existing pose and Output Pose. Set Source Component to the same component. Continue with the [Blueprint guide](/LAMAudio2Expression-UE/en/blueprint/) for the complete connection flow.

The first analysis waits for the roughly 384 MiB model to load and inference to initialize. Runtime use needs no Python, external inference server, or additional model download.

## Package your project

Add `/LAMAudio2Expression/Models` to Project Settings → Packaging → **Additional Asset Directories to Cook**. Check this first if the model works in the editor but cannot be found in a packaged application. See [models and packaging](/LAMAudio2Expression-UE/en/models-and-packaging/).

The plugin alone does not include a face, speech samples, or demo map. Use the [demo project](/LAMAudio2Expression-UE/en/demo/) for a preconfigured character.
