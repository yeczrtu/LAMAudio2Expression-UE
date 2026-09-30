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

<!-- guide:microphone-blueprint:start -->
<figure class="guide-figure" id="figure-microphone-blueprint" data-guide="microphone-blueprint">
<div class="guide-shot" style="--shot-ratio:652/470;--shot-width:314.11043%;--shot-left:-84.66258%;--shot-top:-44.04255%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/microphone-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint calling Start Microphone from M and Stop Microphone from N, with Device Index -1." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:48.61963%;--y:2.97872%;--w:49.07975%;--h:38.93617%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:48.61963%;--y:66.59574%;--w:49.07975%;--h:25.53191%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Start and stop microphone input</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/microphone-blueprint.jpg">Open full-size image</a></p>
<ol>
<li>Device Index -1 on Start Microphone selects the default device.</li>
<li>Call Stop Microphone on the same LAM to finish.</li>
</ol>
<p>This capture illustrates wiring only; no microphone recording or runtime test was performed. Enable Actor input when using keyboard events.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation shared with v0.2.0.</p>
</figcaption>
</figure>
<!-- guide:microphone-blueprint:end -->

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

<!-- guide:pcm-blueprint:start -->
<figure class="guide-figure" id="figure-pcm-blueprint" data-guide="pcm-blueprint">
<div class="guide-shot" style="--shot-ratio:480/558;--shot-width:426.66667%;--shot-left:-132.91667%;--shot-top:-29.39068%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/pcm-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint with Start PCMStream, custom OnPCMBlockReceived feeding Push PCMAudio, and Stop Microphone." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:44.79167%;--y:1.6129%;--w:46.04167%;--h:18.99642%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.5%;--y:37.81362%;--w:94.79167%;--h:27.41935%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:44.79167%;--y:80.28674%;--w:46.04167%;--h:15.41219%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Submit external PCM in small blocks</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/pcm-blueprint.jpg">Open full-size image</a></p>
<ol>
<li>Call Start PCMStream first.</li>
<li>Have your PCM producer invoke the event repeatedly on the game thread. This example uses a float array at 16,000 Hz, mono.</li>
<li>Use Stop Microphone to end this stream as well.</li>
</ol>
<p>OnPCMBlockReceived is an illustrative custom event and is not called automatically. Match Sample Rate and Channels to the actual data.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation shared with v0.2.0.</p>
</figcaption>
</figure>
<!-- guide:pcm-blueprint:end -->

## Inference interval and presentation delay

`Set Live Inference Interval` takes milliseconds. The range is about 33.3–1000 ms, rounded internally to 1–30 frames. The default is 10 frames, about 333.3 ms. A live change applies to the next inference job. `Get Live Inference Interval` returns the rounded request.

**Presentation Delay** is the requested minimum delay: default 0.75 seconds, configurable from 0.4 to 2 seconds. The effective value is the larger of that minimum and “interval + result-latency P95 + one frame,” capped at two seconds. During a session it only increases, avoiding backwards presentation time. A reduction is recalculated on the next start.

A shorter interval requests more frequent updates. Actual real-time behavior depends on hardware and competing work; the requested interval is not a guaranteed result delivery time.

## Inspect runtime state

`Get Live Metrics` returns Actual Interval, Effective Presentation Delay, Inference P95, Result Latency P95, Initialization, Dropped Intervals, Backend, and State. P95 uses the last 60 samples; zero during preparation means not yet measured. Result Latency includes worker queueing; Initialization is reported separately.

`On Live State Changed` reports Preparing, Running, Lagging, Failed, or Stopped. The pending input queue is capped at two seconds, with one inference job per session at a time. When behind, the system drops old pending work and catches up to the latest position. Without a result, it holds for 100 ms and then returns expressions over another 100 ms.

<!-- guide:live-metrics:start -->
<figure class="guide-figure" id="figure-live-metrics" data-guide="live-metrics">
<div class="guide-shot" style="--shot-ratio:1069/432;--shot-width:191.58092%;--shot-left:-32.27315%;--shot-top:-52.31481%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/live-metrics.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint setting 333.333333 ms with Set Live Inference Interval and connecting Get Live Metrics to Break LAMLive Metrics." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:28.99906%;--y:3.24074%;--w:30.12161%;--h:39.58333%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:28.99906%;--y:63.42593%;--w:69.97194%;--h:28.24074%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Set the inference interval and read metrics</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/live-metrics.jpg">Open full-size image</a></p>
<ol>
<li>Pass the requested interval in Milliseconds. 333.333333 is a configured value, not a timing measurement.</li>
<li>Break the Get Live Metrics result to read State and Actual Interval. Expand the downward arrow for the other fields.</li>
</ol>
<p>Connect the outputs to UI or logging to evaluate the values. Those consumers are omitted here. Enable Actor input if using the I key.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation shared with v0.2.0.</p>
</figcaption>
</figure>
<!-- guide:live-metrics:end -->

## Tested coverage

Published tests cover continuous PCM, interval changes during streaming, CPU / DirectML, and worker contention. **Physical microphone capture, disconnection, and long-running operation remain unverified.** Measure your target environment rather than treating functional-test P95 values as an operational guarantee. See [validation](/LAMAudio2Expression-UE/en/validation/).
