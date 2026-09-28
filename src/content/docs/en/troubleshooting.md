---
title: "Troubleshoot setup and lip sync"
description: "Resolve missing models, silent or motionless characters, rejected audio, slow startup, and delayed live input in LAM Audio2Expression for Unreal Engine."
sidebar: {"label":"Troubleshooting"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"}]
---

## The model cannot be found

Check that you downloaded the **model-included release ZIP**, not GitHub's “Source code” archive. The Model setting should be `/LAMAudio2Expression/Models/LAM_A2E`.

If it works in the editor but fails in a packaged build, add `/LAMAudio2Expression/Models` to Packaging → Additional Asset Directories to Cook. See [installation](/LAMAudio2Expression-UE/en/installation/).

## Audio plays but the face does not move

1. Confirm `Apply LAM ARKit Curves` is connected in the character's AnimBP.
2. Confirm Source Component references the component performing analysis and playback.
3. Check that Alpha is nonzero and Curve Profile has not disabled the required curves.
4. Check for matching morph targets such as `jawOpen`, or that your rig consumes the curves.

Custom bone-driven rigs are not converted automatically. See [expression curves](/LAMAudio2Expression-UE/en/expression-curves/).

## Analysis is rejected or an audio input cannot be used

The supported input is a standard mono / stereo SoundWave, 8–192 kHz, up to 300 seconds. SoundCue, MetaSound, Procedural SoundWave, and direct external WAV / MP3 input are unsupported. Loop-enabled SoundWaves are also rejected for playback.

Record the Error from async analysis `Failed` and playback `On Playback Failed`. Starting another analysis on the same component cancels the previous one. See [Blueprint connections](/LAMAudio2Expression-UE/en/blueprint/).

## The first analysis takes longer

Model loading and NNE instance initialization take time. Connect `Progress` to a loading indicator and start playback after `Completed`. Distinguish first-use timing from a fixed window after initialization. See [validation](/LAMAudio2Expression-UE/en/validation/).

## Microphone or live input fails or falls behind

Check Windows microphone permissions, the recording device, and `On Live State Changed`. Inspect initialization, result-latency P95, and dropped intervals with `Get Live Metrics`. Avoid queuing many offline analyses on the shared worker. Supply PCM in small chunks from the game thread.

Microphone audio is not monitored through speakers. Hearing no audio alone does not mean capture failed. See [live input](/LAMAudio2Expression-UE/en/live-input/).

## Advance dialogue on natural completion

Use `On Playback Finished`. `On Playback Ended` also fires for stops and replacements, so do not advance dialogue unconditionally from it. Identify the relevant playback using the event's Playback Id. See [playback controls](/LAMAudio2Expression-UE/en/playback/).

## Report a problem

Open a [plugin issue](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues) or a [demo issue](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/issues). Include the plugin version, UE version, Editor / Shipping configuration, CPU / GPU, reproduction steps, and Error.Code / Error.Message. State whether you are using a release archive or a source build.
