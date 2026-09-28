---
title: "Audio lip-sync compared: LAM, Audio2Face and MetaHuman"
description: "Compare LAM, Audio2Face, MetaHuman Animator, OVRLipsync and SG Com for Unreal Engine lip sync. Understand ARKit 52, emotion control, live input, and alternatives for Unity and the web."
sidebar: {"label":"Lip-sync comparison"}
appliesTo: "Reviewed September 28, 2026 · Published specifications"
sourceSummary: "Official documentation, repositories and research papers linked in this article were reviewed on September 28, 2026. LAM coverage uses this site's pinned public snapshots and v0.2.0. We have not benchmarked competing products against each other."
sources:
  - label: "LAM Plugin / README · 3a04219"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"
  - label: "LAM Demo / Validation record · 275a683"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VALIDATION.md"
  - label: "NVIDIA Audio2Face-3D / Official tools and models"
    url: "https://github.com/NVIDIA/Audio2Face-3D"
  - label: "Epic Games / Audio Driven Animation"
    url: "https://dev.epicgames.com/documentation/metahuman/audio-driven-animation"
  - label: "Meta / Oculus Lipsync Guide"
    url: "https://developers.meta.com/horizon/documentation/unreal/audio-ovrlipsync/"
  - label: "Speech Graphics / SG Com 5.0"
    url: "https://docs.speech-graphics.com/en/sg-com/5.0/what-is-sg-com"
---

Audio-driven animation ranges from estimating mouth shapes to generating an entire facial performance with emotion. Start with **your character, the performance you need, and the deployment environment** to narrow the choices before integration.

This guide uses public sources reviewed on **September 28, 2026**. Its recommendations are judgments based on specifications, not a controlled benchmark or quality ranking. Linked product versions, the LAM release and later public source snapshots are distinguished throughout.

<span id="用途から選ぶ" class="comparison-anchor" aria-hidden="true"></span>

## Choose by use case

<div class="comparison-table" role="region" aria-label="Lip-sync choices by use case" tabindex="0">

| Goal or constraint | Candidates to compare first | What matters |
| --- | --- | --- |
| A generic ARKit character inside UE | [LAM](#lam-audio2expression) | Local CPU / DirectML execution and Blueprint integration |
| A conversational character with emotion controls | [Audio2Face-3D](#audio2face-3d), [SG Com](#sg-com) | Emotion inputs, rig setup and GPU / CPU requirements |
| Recorded dialogue for a MetaHuman | [MetaHuman Animator](#metahuman-animator) offline audio | Performance adjustment and Sequencer production |
| Lightweight mouth animation, Unity or VRM | uLipSync, SALSA | Decide whether you need full-face output or phoneme distinction |
| Web avatars and synthesized speech | TalkingHead / HeadAudio, Azure Speech Viseme | Compare audio analysis with using timing from TTS |
| Changing the mouth in a face image or finished video | Wav2Lip, MuseTalk | Video output serves a different purpose from UE facial curves |

</div>

See [other candidates](#other-candidates) for Unity, web and production tools, and [research and video generation](#research-and-video-generation) for video methods.

<span id="比較する前に" class="comparison-anchor" aria-hidden="true"></span>

## Before comparing

- **ARKit 52 versus visemes:** ARKit coefficients describe facial deformations; visemes represent mouth shapes associated with speech sounds. Coefficient counts alone do not measure accurate lip closure or natural motion.
- **Mouth versus full face:** Good lip sync does not automatically include brows, gaze or head acting. Procedural blinking also differs from expressions inferred from audio.
- **Preprocessing versus live input:** A method that can use the complete recording has different constraints from one receiving audio incrementally.
- **Compute time versus perceived latency:** Include input buffering, queues, rendering and audio output, not just model inference. Real-time processing does not mean zero delay.

<span id="主要手法の比較" class="comparison-anchor" aria-hidden="true"></span>

## Main methods compared

Scroll the tables horizontally when needed. Product names link to explanations and sources below.

<span id="出力と感情表現" class="comparison-anchor" aria-hidden="true"></span>

### Output and emotion

<div class="comparison-table" role="region" aria-label="Main methods: output and emotion" tabindex="0">

| Method | Output and coverage | Emotion and performance | Input and processing |
| --- | --- | --- | --- |
| [LAM](#lam-audio2expression) | ARKit 52 at 30 fps; mouth, eye and brow curves | 12 speaker styles; no dedicated emotion input | SoundWave preprocessing; live microphone / PCM |
| [Audio2Face-3D](#audio2face-3d) | Full face; ARKit-compatible curves in UE. Output format depends on the integration | Audio2Emotion integration and emotion inputs | Recordings and streams; SDK / service configuration matters |
| [MetaHuman: offline](#metahuman-animator) | MetaHuman face, head motion and blinks | Mood detection, overrides and intensity | Generates animation from a SoundWave |
| [MetaHuman: real-time audio](#metahuman-animator) | Facial animation through Live Link | Different controls from offline; the audio solver does not generate head motion | Audio Live Link source |
| [OVRLipsync](#ovrlipsync) | 15 visemes covering silence, vowels and consonants | Laughter detection; full-face emotional acting needs additional work | Microphone / file; live or precomputed |
| [SG Com](#sg-com) | Mouth, full face, blinks, gaze and head motion | Audio-driven emotional expression | Audio stream processing and synchronized playback |

</div>

<span id="実行環境と導入条件" class="comparison-anchor" aria-hidden="true"></span>

### Runtime and integration

<div class="comparison-table" role="region" aria-label="Main methods: runtime and integration" tabindex="0">

| Method | Environment and deployment | Integration and terms | Maintenance and coverage |
| --- | --- | --- | --- |
| [LAM UE integration](#lam-audio2expression) | UE 5.8.2, Windows x64; local CPU / DirectML | Original integration: MIT; upstream code and model: Apache-2.0 | v0.2.0 and pinned public source. Long physical-microphone sessions unverified |
| [Audio2Face-3D](#audio2face-3d) | Windows / Linux SDK, CUDA and TensorRT; local / cloud configurations | MIT SDK; separate facial-model and Audio2Emotion terms. Rig adaptation required | Check SDK, model and UE plugin versions separately |
| [MetaHuman Animator](#metahuman-animator) | Unreal Engine and MetaHuman; UE processing / Live Link | Uses an assembled MetaHuman and its rig; Epic's terms apply | Offline audio requires UE 5.6 or later. Check the requirements for your UE version |
| [OVRLipsync](#ovrlipsync) | UE / Unity plugins; CPU audio analysis | Map 15 visemes and review the SDK terms | End-of-life; Movement SDK migration path for Quest |
| [SG Com](#sg-com) | CPU SDK; Windows / Linux and other platforms; UE integration | Commercial license and character setup. Connectivity depends on licensing mode | This article references 5.0 documentation; check target OS and SDK version |

</div>

### LAM Audio2Expression

This UE integration resamples audio to 16 kHz and generates ARKit 52 curves at 30 fps. **The 12 Style values are speaker styles, not 12 emotions.** There is no dedicated input for happiness or anger. Runtime inference needs neither Python nor an external server, making it a candidate for local animation of a generic ARKit rig. [Published specification](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md)

You still need to match curve names and adjust scales or masks for the face. It does not automatically retarget arbitrary rigs. Start with [installation](/LAMAudio2Expression-UE/en/installation/), then [Blueprint connections](/LAMAudio2Expression-UE/en/blueprint/) and [expression curves](/LAMAudio2Expression-UE/en/expression-curves/).

The default live inference interval is about 333.3 ms, with a requested presentation delay of 0.75 seconds. Fast fixed-window inference numbers do not directly describe conversational latency. [Live settings and limitations](/LAMAudio2Expression-UE/en/live-input/)

### Audio2Face-3D

Include today's SDK, models and UE integrations when evaluating Audio2Face, alongside the older Omniverse application. Facial models accept emotion inputs and can work with Audio2Emotion; connecting outputs to a rig depends on the integration. [Official component list](https://github.com/NVIDIA/Audio2Face-3D), [facial model inputs and outputs](https://huggingface.co/nvidia/Audio2Face-3D-v3.0)

The ACE UE plugin 2.5 animation node adds ARKit-compatible curves. The target face needs matching poses and curve setup. [UE character setup](https://docs.nvidia.com/ace/ace-unreal-plugin/2.5/ace-unreal-plugin-animation.html)

The public SDK uses CUDA / TensorRT and an NVIDIA GPU, unlike LAM's CPU / DirectML path. The SDK is MIT; the v3.0 facial model uses the NVIDIA Open Model License. **Audio2Emotion v2.2 restricts use to Audio2Face and cannot be treated as a standalone emotion estimator for LAM.** [SDK requirements](https://github.com/NVIDIA/Audio2Face-3D-SDK), [Audio2Emotion terms](https://huggingface.co/nvidia/Audio2Emotion-v2.2)

### MetaHuman Animator

For recorded dialogue, offline audio processing offers mood detection, overrides, intensity, head movement and blinks before animation export. It uses the MetaHuman facial rig rather than a generic ARKit 52 output interface. [Offline audio workflow](https://dev.epicgames.com/documentation/metahuman/audio-driven-animation)

Live audio drives the character through a MetaHuman Audio Live Link Source. The Realtime Audio Solver does not generate head motion and does not expose the same full set of offline performance controls. Selecting that solver inside a Performance asset also does not turn the input into a live stream. [Live Link animation](https://dev.epicgames.com/documentation/metahuman/realtime-animation-using-live-link), [solver differences](https://dev.epicgames.com/documentation/metahuman/audio-driven-animation)

### OVRLipsync

This mouth-focused approach provides 15 visemes and laughter detection. It analyzes consonant and vowel mouth shapes from audio, making it relevant to avatars with a small set of mouth shapes.

**Meta's official notice, updated April 17, 2026, says the plugin will receive no further updates or support.** Meta points Quest developers to audio-based face tracking in Movement SDK. This does not imply a drop-in replacement for every existing Windows, Unity or UE integration. [Meta specification and end-of-life notice](https://developers.meta.com/horizon/documentation/unreal/audio-ovrlipsync/)

### SG Com

SG Com is a commercial CPU-based candidate for generating full-face animation, head motion and gaze from an audio stream. It offers UE integration and requires character setup and licensing. **The 50 ms in its 5.0 documentation is a vendor-stated input-to-output processing delay**, not a measurement under the same conditions as this site's LAM results. [SG Com 5.0 overview](https://docs.speech-graphics.com/en/sg-com/5.0/what-is-sg-com), [CPU resource usage](https://docs.speech-graphics.com/en/sg-com/5.0/sg-com-compute-resource-usage)

Check the [supported platforms](https://docs.speech-graphics.com/en/sg-com/5.0/sg-com-platform-support). Consider audio processing location and license authentication separately: cloud-based licensing needs connectivity, so fully offline deployment also depends on the contract and licensing mode.

<span id="その他の候補" class="comparison-anchor" aria-hidden="true"></span>

## Other candidates

These summaries also use official sources reviewed on September 28, 2026. Each product link is the source for its row.

<span id="unity2d台詞制作" class="comparison-anchor" aria-hidden="true"></span>

### Unity, 2D and dialogue production

<div class="comparison-table" role="region" aria-label="Unity and production alternatives" tabindex="0">

| Candidate | Approach and use | Check before adopting |
| --- | --- | --- |
| [uLipSync](https://github.com/hecomi/uLipSync) | MFCC-based mouth-shape estimation in Unity; live, prebaked and VRM workflows | Voice-specific profile calibration. MIT. Suitable for five-vowel setups but not limited to vowels |
| [SALSA LipSync Suite](https://crazyminnowstudio.com/docs/salsa-lip-sync/modules/recommendations/) | Unity mouth animation based on changes in audio amplitude | Does not identify specific phonemes. Commercial. Consider for stylized motion and quick integration |
| [Rhubarb Lip Sync](https://github.com/DanielSWolf/rhubarb-lip-sync/blob/master/README.adoc) | Audio files to 6–9 timed mouth shapes for 2D production | Phonetic recognition is available for non-English audio. Offline processing serves a different purpose from live full-face generation |
| [FaceFX](https://www.facefx.com/) | Generate dialogue animation, then edit phonemes and curves | Commercial production tools for processing and refining large amounts of recorded dialogue |
| [iClone AccuLIPS](https://www.reallusion.com/iclone/lipsync-animation.html) | Audio / script alignment, co-articulation and word-level editing | Commercial authoring tool with an English-centered dictionary. Non-English workflows exist; evaluate Japanese quality separately |
| [SGX](https://docs.speech-graphics.com/en/sgx/4.4/what-is-sgx) | Full-face and head animation, batch generation and editing | Commercial, with character setup. This article references 4.4 documentation; distinguish it from the SG Com live SDK |

</div>

<span id="web音声合成" class="comparison-anchor" aria-hidden="true"></span>

### Web and speech synthesis

<div class="comparison-table" role="region" aria-label="Web and speech synthesis alternatives" tabindex="0">

| Candidate | Approach and use | Check before adopting |
| --- | --- | --- |
| [Azure Speech Viseme](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/how-to-speech-synthesis-viseme) | Visemes produced during TTS; blendshape output has 55 values at 60 fps | Cloud speech terms and language support for each output type. This is not arbitrary recorded-audio analysis |
| [TalkingHead](https://github.com/met4citizen/TalkingHead) | Browser 3D avatars using text, timing and audio for mouth animation | Check TTS integration and languages. MIT. Expression controls are distinct from inferring emotion from audio |
| [HeadAudio](https://github.com/met4citizen/HeadAudio) | Audio-driven viseme estimation inside the browser | MIT, no analysis server. Its author documents detection and voice-activity limitations at low signal-to-noise ratios |

</div>

If your TTS provides pronunciation timing, consider using it directly. This avoids estimating pronunciation again from generated audio, while brows, gaze and emotional acting still need their own design.

<span id="研究モデルと動画生成" class="comparison-anchor" aria-hidden="true"></span>

## Research and video generation

<div class="comparison-table" role="region" aria-label="Research models and video generation" tabindex="0">

| Method | Output and research focus | Difference from LAM and usage terms |
| --- | --- | --- |
| [EmoTalk](https://github.com/psyai-net/EmoTalk_release) | Facial animation separating emotion from speech content | A candidate for emotion research. Public implementation: CC BY-NC 4.0; commercial licensing requires contact |
| [EMOTE](https://arxiv.org/abs/2306.08990) | 3D animation with separate control of speech content and emotion | FLAME-based representation. Requires ARKit rig adaptation and a review of code / model terms |
| [FaceFormer](https://github.com/EvelynFan/FaceFormer) | Transformer-based speech-to-3D-mesh generation | Does not directly output ARKit 52. Training data, model and target rig must be compatible |
| [Wav2Lip](https://github.com/Rudrabha/Wav2Lip) | Synchronizes mouth regions in face videos to audio | Does not directly drive UE Morph Targets. Public research version has noncommercial terms |
| [MuseTalk](https://github.com/TMElyralab/MuseTalk) | Generates mouth regions in face images / videos | Video output. Code and models are described as commercially usable; dependent models have separate terms |

</div>

Research implementations, pretrained weights and datasets can have different terms. Check the specific linked versions before adopting them. This site does not redistribute models or other products' code.

<span id="品質評価の読み方" class="comparison-anchor" aria-hidden="true"></span>

## How to read quality evidence

<span id="公開比較研究が示す範囲" class="comparison-anchor" aria-hidden="true"></span>

### What a published comparison covers

A June 2026 [study of speech-driven animation in UE](https://arxiv.org/html/2606.10753v1) compares MetaHuman, Audio2Face, and ARKit-trained FaceDiffuser and ProbTalk3D-X. In two experiments with 12 and 8 audio clips, MetaHuman received the highest mean lip-sync, realism and expressiveness ratings.

**LAM was not included.** The study used high-quality human characters such as MetaHumans; its results do not establish a ranking for Japanese speech or stylized rigs.

<span id="lamの既存測定との区別" class="comparison-anchor" aria-hidden="true"></span>

### Distinguish LAM's existing measurements

Published LAM records show warm fixed-window inference at about 6.7 ms on DirectML and 50–57 ms on CPU. These are model processing times, not delays from audio capture to visible expression. The [validation page](/LAMAudio2Expression-UE/en/validation/) records the UE, Windows and hardware conditions, NullRHI measurements, and unverified physical-microphone coverage. Do not rank these alongside SG Com's 50 ms as if they were the same metric.

<span id="同じ音声で確かめる項目" class="comparison-anchor" aria-hidden="true"></span>

### Evaluate with the same audio

1. **Lip closure:** Check Japanese “pa, ba, ma” sounds for appropriate closure.
2. **Fast speech and silence:** Examine transitions, word endings and unwanted mouth motion during silence.
3. **Emotion:** Check whether the same line can take different specified emotions and whether intent is conveyed beyond the mouth.
4. **Synchronization:** Measure audio–mouth offset and the time from input onset to visible expression.
5. **Concurrent load:** Measure CPU / GPU and memory usage while rendering, including stability with multiple characters.

Keep the input audio, playback level, camera, output rig, settings and hardware consistent, and document unavoidable rig differences. The [six Japanese demo samples](/LAMAudio2Expression-UE/en/demo/) provide a starting point, but existing functional tests do not establish a quality difference between methods.
