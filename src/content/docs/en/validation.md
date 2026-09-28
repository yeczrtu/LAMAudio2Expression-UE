---
title: "Validation results and supported scope"
description: "Published numerical parity, Blueprint, Shipping, and live PCM tests for LAM Audio2Expression on UE 5.8.2, Windows x64, and RTX 3070, with explicit coverage limits."
sidebar: {"label":"Validation results"}
appliesTo: "Published source snapshot · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/VALIDATION.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VALIDATION.md"}]
---

This page summarizes public results from **September 24–25, 2026**. The environment was UE 5.8.2, Windows x64, Visual Studio 2022 / MSVC 14.44, Core i7-12700, RTX 3070, and 64 GB RAM. Release tests, later source changes, and earlier measurements are identified separately.

## Numerical model parity

Maximum absolute error against PyTorch, using a fixed window, FP32, opset17, and 3,328 values before post-processing:

| Input | Python ONNX Runtime | UE CPU | UE DirectML |
| --- | ---: | ---: | ---: |
| noise / style 0 | 7.2122e-6 | 7.2122e-6 | 1.7136e-6 |
| silence / style 0 | 1.7509e-7 | 1.70e-7 | 6.3e-8 |
| noise / style 11 | 4.2617e-6 | 4.232e-6 | 1.088e-6 |

All were below 1e-3. Tensor parity is distinct from a subjective assessment of lip-sync quality.

## Version 0.2 functional tests

| Test suite | Published result |
| --- | --- |
| UE Automation | 5 passed: decoder, boundaries / curves, PIE teardown / cancellation, live timing, model parity |
| Additional playback / live tests | 5 passed in each of Editor, Development, and Shipping |
| Existing packaged regressions | 10 passed in each of Development and Shipping |
| Shipping face demo | All 6 JVNV speech samples passed |

Tests cover volume, submixes, concurrency, game pause, completion events, live interval changes, and recovery under CPU contention. Inspect the [Shipping test results](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/Shipping-playback-controls-0.2.json) and [Automation record](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/automation-0.2.json).

## Blueprint face demo

In the **published source revision after the v0.2.0 ZIPs**, three Blueprints compiled without errors or warnings. All six samples passed in Editor Standalone, Development, and Shipping, covering HUD clicks through analysis, playback, and AnimGraph jawOpen output. See the [18 execution records](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/blueprint-demo-results.json).

Dependency checks found no demo C++ module references across 44 assets. Physical keyboard and microphone input were not part of this automated verification.

## Interpreting performance numbers

The earlier Shipping / DirectML / NullRHI measurement analyzed five minutes of audio in 3.489 seconds for inference plus post-processing, or 8.220 seconds including model loading and decoding. A separate CPU-forced process measured 74.965 and 78.519 seconds respectively. Process startup was excluded.

After initialization, fixed windows in a same-process test took about 50–57 ms on CPU and 6.7 ms on DirectML. Do not extrapolate short-window timing directly to long analysis. Peak Working Set for the entire Shipping process was about 1,748–1,775 MiB, including UE, the model, and NNE.

Version 0.2 separates initialization from P95 and includes worker queueing in Result Latency. These definitions differ from earlier P95 results. NullRHI functional tests are not performance guarantees for a rendered application.

## Remaining unverified areas

- Physical microphone capture, disconnects, overflow, and long-running live sessions.
- Many simultaneous characters, sustained cache pressure, and memory-leak testing.
- Physical GPU loss, driver faults, and corrupted cooked chunks.
- Synchronization including physical audio output, real socket tracking, and perceived attenuation / spatialization.
- Full-utterance subjective quality and use on other characters.

See [development and releases](/LAMAudio2Expression-UE/en/development/) to rerun the published checks.
