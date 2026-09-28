---
title: "Audio-driven facial animation in Unreal Engine"
description: "Generate ARKit 52 facial curves from audio with LAM Audio2Expression for Unreal Engine. Install the plugin, try the Windows demo, and connect lip sync in Blueprint."
sidebar: {"label":"Overview"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / README.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"},{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"}]
---

<p class="lead">Turn a SoundWave into synchronized facial animation. LAM Audio2Expression is an Unreal Engine runtime plugin that runs inference locally on your PC.</p>

<div class="signal-flow" aria-label="Audio to expression pipeline">
  <div><span>01 INPUT</span><strong>Audio</strong><small>SoundWave / microphone / PCM</small></div>
  <div><span>02 INFERENCE</span><strong>Local inference</strong><small>DirectML / CPU</small></div>
  <div><span>03 EXPRESSION</span><strong>ARKit 52</strong><small>Blueprint / AnimGraph</small></div>
</div>

[Install the plugin](/LAMAudio2Expression-UE/en/installation/) · [Try the Windows demo](/LAMAudio2Expression-UE/en/demo/) · [Connect Blueprint nodes](/LAMAudio2Expression-UE/en/blueprint/)

## Downloads

<div class="download-grid">
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip"><strong>Plugin</strong><span>For your Unreal project<br/>Model included · ~376 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-Win64-Demo.zip"><strong>Windows demo</strong><span>No UE editor required<br/>Standalone ZIP · ~968 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Project-Model.zip"><strong>UE project</strong><span>Edit the character and audio<br/>Model included · ~401 MiB</span></a>
</div>

All downloads above are **v0.2.0**. GitHub's automatically generated “Source code” archives do not include the model; the demo source archive also omits the plugin submodule. Checksums are available in the [plugin release](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/tag/v0.2.0) and [demo release](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/tag/v0.2.0).

## Six Japanese speech samples

![The v0.2.0 face demo generating expressions from Japanese speech, captured five seconds into the published recording.](/LAMAudio2Expression-UE/images/demo-preview.png)

<p class="media-credit">Six speech samples express anger, disgust, fear, happiness, sadness, and surprise. Video, speech, and demo presentation: CC BY-SA 4.0. Character: hinzka / VRoid. Speech: JVNV / litagin. <a href="/LAMAudio2Expression-UE/en/licenses/">Credits and terms</a></p>

The original video host is currently unavailable. Follow the [Windows demo guide](/LAMAudio2Expression-UE/en/demo/) to try it yourself.

## Requirements and capabilities

| Area | Supported |
| --- | --- |
| Engine / OS | Unreal Engine 5.8.2 / Windows x64 |
| Character | ARKit 52 morph targets, or a rig driven by animation curves |
| Offline analysis | Standard SoundWave, mono / stereo, 8–192 kHz, up to 5 minutes |
| Playback | Synchronized audio and expressions, pause, resume, seek, volume, submixes, 2D / 3D |
| Live input | Microphone or external PCM; inference interval about 33.3–1000 ms |
| Inference | DirectML preferred, with CPU fallback |

Python and an external inference server are not required at runtime. SoundCue, MetaSound, Procedural SoundWave, and direct loading of external WAV / MP3 files are outside the supported input path. See [validation](/LAMAudio2Expression-UE/en/validation/) for test coverage and measurement conditions.

## Documentation versions

The basic workflow targets the published v0.2.0 release. The Blueprint-based demo UI and analysis flow are changes in the published source and are **not included in the v0.2.0 ZIPs**. The [demo guide](/LAMAudio2Expression-UE/en/demo/) and [development guide](/LAMAudio2Expression-UE/en/development/) identify that distinction.

This site uses pinned public source snapshots. Each page links to the exact documents used.
