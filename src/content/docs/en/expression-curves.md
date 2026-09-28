---
title: "ARKit 52 curves and Curve Profiles"
description: "Configure ARKit 52 facial curves in Unreal Engine. Map morph target names, adjust scale and masks, and understand smoothing, silence suppression, and blinking."
sidebar: {"label":"Expression curves"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Demo / Docs/ARCHITECTURE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/ARCHITECTURE.md"}]
---

LAM Audio2Expression outputs 52 curves in the standard upstream order. Match them to your character's morph target names, or consume their values in an existing rig.

## Fit the output to a character

The **Alpha** input of `Apply LAM ARKit Curves` ranges from 0 to 1 and controls expression strength relative to the input pose. If the mouth does not move, first check that the mesh has a morph target matching `jawOpen`.

Create a **LAMCurveProfile** Data Asset and assign it to Curve Profile on the node to configure each curve:

| Setting | Purpose |
| --- | --- |
| Name mapping | Match a character-specific morph target name |
| Disable | Exclude a curve from application |
| Scale | Adjust the strength of a movement |
| Offset | Add a constant to the estimated value |

Curves absent from the profile remain enabled under their standard names. Adjusted values are clamped to 0–1.

```text
result = lerp(inputCurve,
              clamp(estimatedValue * scale + offset, 0, 1),
              Alpha * Weight)
```

Disabled curves and bone poses are left unchanged. Weight includes effects such as the stop fade. Stopping returns to the input pose over 100 ms; pausing holds the current expression.

## Analysis settings

| Setting | Default and meaning |
| --- | --- |
| Style | 0; an upstream speaker-style index from 0 to 11 |
| Smooth | Enabled; five-frame Savitzky–Golay smoothing and boundary blending |
| Suppress Silent Mouth | Enabled; suppresses mouth motion when RMS stays below 0.001 for seven frames |
| Symmetrize | Disabled; left/right symmetrization |
| Auto Blink | Disabled; procedural blinking, not inferred from speech |
| Blink Seed | Random seed for reproducible procedural blinking |

Style selects an upstream model style; it is not an emotion selector. Verify the connection with defaults first, then adjust profile scales and masks for your character.

## From inference to the final look

Analysis advances a fixed window of about 2.13 seconds in one-second steps, producing curves at 30 fps. Post-processing runs across the resulting sequence. This does not reproduce the upstream demo's random output exactly.

You can drive Control Rig or a bone-based setup with these curves, but the plugin does not automatically retarget arbitrary rigs. See [Blueprint connections](/LAMAudio2Expression-UE/en/blueprint/) and [validation coverage](/LAMAudio2Expression-UE/en/validation/).
