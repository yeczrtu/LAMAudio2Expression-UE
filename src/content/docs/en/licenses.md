---
title: "Licenses and credits"
description: "Source attribution for LAM Audio2Expression code and model weights, JVNV speech, demo video, and the hinzka / VRoid character, based on the published project notices."
sidebar: {"label":"Licenses & credits"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / THIRD_PARTY_NOTICES.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/THIRD_PARTY_NOTICES.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"},{"label":"Demo / Resources/Demo/README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Resources/Demo/README.md"}]
---

This page organizes the project's published attribution records. Code, model weights, and demo assets retain their respective licenses and notices.

## License scope

| Material | Published attribution |
| --- | --- |
| Original plugin integration code | MIT |
| LAM-derived code and model weights | Apache-2.0 |
| Speech, audio-synchronized demo presentation, and recording | CC BY-SA 4.0 |
| Character and textures | hinzka's published permission and applicable VRoid terms |
| Unreal Engine | Epic Games terms |

[MIT text](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/LICENSE) · [Apache-2.0 text](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Licenses/Apache-2.0.txt) · [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

## Upstream code and model

The project references [LAM Audio2Expression](https://github.com/aigc3d/LAM_Audio2Expression) and a [pinned model card](https://huggingface.co/3DAIGC/LAM_audio2exp/blob/0fe5f4dbb283ec7d9c01688681e6e4b6ac314858/README.md). The model is converted to ONNX and used as an Unreal Engine NNE asset. This does not relicense it under MIT.

Retain LICENSE, THIRD_PARTY_NOTICES, license texts, provenance, and conversion notes when redistributing. Model manifests in the repositories record pinned revisions and hashes.

## Speech and video

Original work: **JVNV: A Corpus of Japanese Emotional Speech with Verbal Content and Nonverbal Expressions**. Creators: **Detai Xin, Junfeng Jiang, Shinnosuke Takamichi, Yuki Saito, Akiko Aizawa, and Hiroshi Saruwatari**. [Official corpus](https://sites.google.com/site/shinnosuketakamichi/research-topics/jvnv_corpus)

The selected speech comes from **litagin**'s [jvnv_corpus_v1_no_nv](https://huggingface.co/datasets/litagin/jvnv_corpus_v1_no_nv), pinned at `0ca4908ee5b9610bc3b739e1e67dff6120df0675`. This preserves the prior removal of nonverbal intervals and their transcript text.

The six F1 samples are anger_regular_31, disgust_regular_38, fear_regular_23, happy_regular_38, sad_regular_10, and surprise_regular_11. The demo imports them as SoundWaves, encodes runtime playback with Bink Audio, and generates synchronized facial animation. Attribution does not imply endorsement by the original creators.

The preview is a frame extracted at five seconds from the published recording; the face-demo image republishes an existing screenshot without modification. This site displays still images because the original video host is unavailable. Retain attribution, license links, and modification notices when reusing the material.

## Character and textures

**hinzka — [52blendshapes-for-VRoid-face](https://github.com/hinzka/52blendshapes-for-VRoid-face)**, based on VRoid Studio / pixiv. The demo's provenance record relies on the use, modification, and redistribution permission in the author's [pinned README](https://github.com/hinzka/52blendshapes-for-VRoid-face/blob/756f5abab7d2295ad5b5dbc2cd86972c388c48d2/README.md), updated September 18, 2026.

The older VRM metadata still contains `Redistribution_Prohibited`. The demo's provenance record explicitly notes that discrepancy and relies on the newer published README permission. The character is not relicensed as MIT or CC0.

The demo migrates an FBX-converted Face52 mesh, assigns extracted textures, and uses masked, lit UE materials in place of MToon. Consult the [applicable VRoid terms](https://vroid.pixiv.help/hc/ja/articles/4405813333657) and the original attribution linked below.
