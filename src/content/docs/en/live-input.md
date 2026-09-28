---
title: "Generate live expressions from microphone or PCM"
description: "Feed microphone or external PCM audio into Unreal Engine facial animation. Configure inference intervals, presentation delay, live states, and P95 metrics."
sidebar: {"label":"Microphone & live input"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"}]
---

Generate expressions continuously from a microphone or PCM stream instead of analyzing an entire SoundWave first. Live inference and offline analysis share one dedicated worker.

## Microphone input

`Start Microphone(Settings, DeviceIndex=-1)` selects the default recording device. Windows microphone permission is required. Call `Stop Microphone` to finish.

The plugin does not monitor captured audio through speakers. Startup requires model loading and initialization. You can analyze a SoundWave beforehand to load the model asset.

## Supply external PCM

```text
Start PCM Stream(Settings)
  → Repeat on the game thread:
      Push PCM Audio(InterleavedPCM, SampleRate, Channels)
  → Stop Microphone
```

- Interleaved mono / stereo float samples in the range −1 to 1.
- Sample rates of 8–192 kHz, at most two seconds per call.
- Restart the stream when changing sample rate.
- Send small chunks regularly instead of batching large chunks.

## Inference interval and presentation delay

`Set Live Inference Interval` takes milliseconds. The range is about 33.3–1000 ms, rounded internally to 1–30 frames. The default is 10 frames, about 333.3 ms. A live change applies to the next inference job. `Get Live Inference Interval` returns the rounded request.

**Presentation Delay** is the requested minimum delay: default 0.75 seconds, configurable from 0.4 to 2 seconds. The effective value is the larger of that minimum and “interval + result-latency P95 + one frame,” capped at two seconds. During a session it only increases, avoiding backwards presentation time. A reduction is recalculated on the next start.

A shorter interval requests more frequent updates. Actual real-time behavior depends on hardware and competing work; the requested interval is not a guaranteed result delivery time.

## Inspect runtime state

`Get Live Metrics` returns Actual Interval, Effective Presentation Delay, Inference P95, Result Latency P95, Initialization, Dropped Intervals, Backend, and State. P95 uses the last 60 samples; zero during preparation means not yet measured. Result Latency includes worker queueing; Initialization is reported separately.

`On Live State Changed` reports Preparing, Running, Lagging, Failed, or Stopped. The pending input queue is capped at two seconds, with one inference job per session at a time. When behind, the system drops old pending work and catches up to the latest position. Without a result, it holds for 100 ms and then returns expressions over another 100 ms.

## Tested coverage

Published tests cover continuous PCM, interval changes during streaming, CPU / DirectML, and worker contention. **Physical microphone capture, disconnection, and long-running operation remain unverified.** Measure your target environment rather than treating functional-test P95 values as an operational guarantee. See [validation](/LAMAudio2Expression-UE/en/validation/).
