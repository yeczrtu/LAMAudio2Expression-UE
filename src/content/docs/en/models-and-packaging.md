---
title: "Models and Windows packaging"
description: "Configure Unreal Engine cooking for the LAM Audio2Expression model. Understand CPU and DirectML data, model provenance, release checksums, and redistribution notices."
sidebar: {"label":"Models & packaging"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/RELEASE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/RELEASE.md"}]
---

## Model location

The model-included ZIP contains a neural model asset of roughly 384 MiB. Its default reference is `/LAMAudio2Expression/Models/LAM_A2E`. UE uses the ONNX stored in the NNE asset and cooked CPU / DirectML data.

Models are not stored in the normal Git history. For a source checkout, follow [development setup](/LAMAudio2Expression-UE/en/development/) to retrieve and convert a pinned checkpoint, or use the published model-included package.

## Cooking checklist

1. Confirm the Model setting in Project Settings → LAM Audio2Expression.
2. Add `/LAMAudio2Expression/Models` to Packaging → Additional Asset Directories to Cook.
3. When packaging the demo, add `/Game/LAMFaceDemo/Maps/LAM_FaceDemo` to Maps to Cook.
4. Keep the plugin's `Source` and `Intermediate/Build` directories.
5. Copy or extract the complete output to another short path and test the packaged application.

Distribute the entire folder, including cooked data, required DLLs, and licenses, rather than the executable alone. The published Windows demo also includes a VC++ runtime installer.

## GPU and CPU

DirectML is preferred, with CPU fallback when unavailable. Show model loading and initialization as part of your application's loading state. The tested fallback path covers an unavailable GPU model; it is not a reproduction of physical GPU failure. See [validation](/LAMAudio2Expression-UE/en/validation/).

## Versions and checksums

The [model manifest](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/model-manifest.json) records provenance, pinned revisions, shapes, and SHA-256 hashes. Releases attach `release-manifest.json` and `SHA256SUMS.txt`.

Record plugin, UE, and model versions together with hashes. Prefer a new release version over silently replacing a published ZIP.

## Distribution notices

MIT for original code and Apache-2.0 for upstream-derived code and model weights have different scopes. Preserve `LICENSE`, `THIRD_PARTY_NOTICES.md`, `Licenses`, model provenance, and conversion notes. Demo audio, visual presentation, and character assets have separate terms. See [licenses and credits](/LAMAudio2Expression-UE/en/licenses/).
