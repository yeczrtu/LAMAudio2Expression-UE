---
title: "Inference, synchronization, and AnimGraph architecture"
description: "Understand LAM Audio2Expression modules, dedicated inference worker, audio-clock synchronization, expression snapshots, cache ownership, and live processing."
sidebar: {"label":"Architecture"}
appliesTo: "Published source snapshot · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/ARCHITECTURE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/ARCHITECTURE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

## Processing flow

```text
SoundWave → Decode → 16 kHz PCM → Windowed inference → Post-process → Clip
                                                                    ↓
Audio playback clock → Current expression snapshot → AnimGraph → ARKit curves
```

`LAMAudio2Expression` is the Runtime module. `LAMAudio2ExpressionEditor` is UncookedOnly and provides AnimGraph editor nodes, model configuration, example generation, and PIE tests. No engine modification is required.

| Implementation | Responsibility |
| --- | --- |
| LAMAnalyzeAsync | Async loading, analysis jobs, BP notifications, PCM hashes, LRU cache |
| LAMDecoder | Cooked SoundWave decoding and streaming-chunk retention |
| LAMCore | Resampling, window extraction, NNE inference, CPU fallback, post-processing |
| LAMAudio2ExpressionComponent | Playback control, synchronization clock, expression snapshots |
| AnimNode_LAMARKit | Snapshot acquisition and curve blending |
| LAMLive | Microphone / PCM, continuous resampling, live inference, presentation delay |
| LAMTypes | 52 curve names, settings, Clip, Profile, interpolation |

## Threads and lifetime

Model UObject loading and NNE model creation run on the game thread. Decoding, resampling, NNE model-instance creation, inference, and post-processing use one dedicated worker. Blueprint progress and completion notifications return to the game thread.

An in-flight NNE call is not interrupted. Cancellation flags suppress subsequent work and result notifications. EndPlay and async-action destruction also invalidate jobs.

The AnimNode copies curves and the Profile in PreUpdate. Evaluate_AnyThread uses only that snapshot and SourcePose; it performs no inference or UObject lookup.

## Analysis windows and time

Offline analysis advances by 16,000 samples. Each 34,133-sample window produces 64 frames; frames 34–63 are retained. Samples before the start and after the end are zero-padded. The final frame count uses integer arithmetic: `(samples * 30 + 15999) / 16000`.

Playback uses audio time, allowing at most 1/30 second of interpolation prediction until the next callback. When seeking recreates internal audio playback, notifications from the old playback are ignored. Physical output-device latency is not compensated.

## Cache and ownership

The analysis cache defaults to 64 MiB. Its key includes the 16 kHz PCM SHA-1, model GUID, style, post-processing settings, and processing version. A playback Clip owns its result array independently, so cache eviction does not invalidate it. Model memory and Clips retained by callers are outside the cache budget.

## Live processing

The pending input queue is capped at two seconds, with separate historical context for inference. Old-generation results are discarded, and delayed processing catches up to the latest position. Version 0.2 supports adjustable live intervals; see [live input](/LAMAudio2Expression-UE/en/live-input/). Audio routing, concurrency, and event behavior are documented in [playback controls](/LAMAudio2Expression-UE/en/playback/).

## Saved-clip path (published source)

Plugin 1f06ac8 adds `ULAMBakedExpressionClip`, an asset containing analyzed ARKit 52 curves. Playback shares its array and interpolates 52 values, skipping model loading, analysis decoding, inference, and a full-array copy per play. Viseme conversion remains a runtime operation. This path is separate from the v0.2.0 analysis cache above. See [generation and load validation](/LAMAudio2Expression-UE/en/baked-clips/).
