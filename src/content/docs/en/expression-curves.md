---
title: "ARKit 52 curves, Curve Profiles, and Viseme conversion"
description: "Configure ARKit 52 curves and v0.3.0 five-vowel / Oculus-compatible Visemes in Unreal Engine. Covers profile names, TemplateFit, mouth ownership, and limits."
sidebar: {"label":"Expression curves"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Demo / Docs/ARCHITECTURE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/ARCHITECTURE.md"},{"label":"Plugin / Docs/VISEMES.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md"}]
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

<!-- guide:curve-profile:start -->
<figure class="guide-figure" id="figure-curve-profile" data-guide="curve-profile">
<div class="guide-shot" style="--shot-ratio:975/199;--shot-width:210.05128%;--shot-left:-1.53846%;--shot-top:-95.47739%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/curve-profile.jpg" width="2048" height="1111" loading="lazy" decoding="async" alt="Curve Profile with Source Name and Target Name jawOpen, Enabled checked, Scale 0.8 and Offset 0.0." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.97436%;--y:28.1407%;--w:94.35897%;--h:27.13568%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.97436%;--y:56.28141%;--w:94.35897%;--h:38.69347%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Example jawOpen adjustment with a Curve Profile</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/curve-profile.jpg">Open full-size image</a></p>
<ol>
<li>Source Name is the LAM curve; Target Name is the name on the character.</li>
<li>This illustrative rule uses Enabled, Scale 0.8 and Offset 0.0.</li>
</ol>
<p>0.8 is an illustrative adjustment, not a universal recommended value. Assign the created profile to the AnimGraph node.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:curve-profile:end -->

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

<!-- guide:bake-settings:start -->
<figure class="guide-figure" id="figure-bake-settings" data-guide="bake-settings">
<div class="guide-shot" style="--shot-ratio:356/170;--shot-width:147.75281%;--shot-left:-2.24719%;--shot-top:-51.17647%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg" width="526" height="580" loading="lazy" decoding="async" alt="Generation settings: Style 0, Smooth and Suppress Silent Mouth enabled, Symmetrize and Auto Blink disabled, Blink Seed 1234." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:2.94118%;--w:93.53933%;--h:13.52941%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:17.64706%;--w:93.53933%;--h:46.47059%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:64.70588%;--w:93.53933%;--h:31.17647%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Analysis settings stored with the clip</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg">Open full-size image</a></p>
<ol>
<li>Style is a speaker-style index from 0 to 11, not an emotion name.</li>
<li>This example enables Smooth and Suppress Silent Mouth and disables Symmetrize.</li>
<li>Auto Blink is disabled. Blink Seed 1234 is the seed used for automatic blinking.</li>
</ol>
<p>After setting the options, click Generate at the bottom of the dialog. This generation dialog is included in v0.3.0.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:bake-settings:end -->

## From inference to the final look

Analysis advances a fixed window of about 2.13 seconds in one-second steps, producing curves at 30 fps. Post-processing runs across the resulting sequence. This does not reproduce the upstream demo's random output exactly.

You can drive Control Rig or a bone-based setup with these curves, but the plugin does not automatically retarget arbitrary rigs. See [Blueprint connections](/LAMAudio2Expression-UE/en/blueprint/) and [validation coverage](/LAMAudio2Expression-UE/en/validation/).

<span id="visemes" class="comparison-anchor" aria-hidden="true"></span>

## Five-vowel and Oculus-compatible Visemes

**Included in v0.3.0.** `Apply LAM Viseme Curves` converts the interpolated ARKit 52 frame into five vowel weights or 15 Oculus-compatible slots. It works with dynamic SoundWave analysis, saved clips, and live input without another model, the Oculus SDK, or reanalysis. This is mouth-shape conversion, not phoneme recognition. [Public specification, reviewed 2026-09-30](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md)

1. Prepare a mesh with a neutral mouth and the required morph targets. The plugin does not automatically create arbitrary character morphs.
2. Enable **Show Plugin Content**, duplicate a profile from `/LAMAudio2Expression/Profiles/` into your project, and set **Target Names** to the mesh's actual morph names. `None` means no write.
3. Connect `Existing Pose → Apply LAM Viseme Curves → Output Pose` in AnimGraph. Set **Profile** and **Source Component** to the actor's LAM component. An empty Source Component searches the owning actor; assign it explicitly when several components exist.
4. Test pause, seek, stop, neutral mouth, and your own speech material. Missing morph names produce curves but no mesh deformation.

| Profile | Conversion / intended morphs |
| --- | --- |
| `DA_JapaneseFive` | FiveVowelRules; default names `A/I/U/E/O` |
| `DA_OculusReference` / `DA_OculusSDK` | FiveVowelRules with 15 output names; the nine consonant slots remain zero |
| `DA_OculusOpenFaceFX` | TemplateFit; OpenFaceFX recipe, SDK names such as `ih/oh/ou` |
| `DA_OculusTalkingHead` | TemplateFit; TalkingHead recipe, prefixed names such as `viseme_I/viseme_O/viseme_U` |

Output names and conversion mode are independent settings. For a five-vowel-only mesh, keep **FiveVowelRules**. With no Profile assigned, the node uses the default five-vowel settings and `A/I/U/E/O` names. In FiveVowelRules, adjust neutral **Input Corrections**, then **Activation**, **Width / Roundness / OpenSplit**, and finally **Vowel Gains**. These are shape-based approximations; they do not guarantee phonetic accuracy across characters.

### Combining upper-face and mouth animation

Use `DA_UpperFaceOnly` on the ARKit node if LAM should also drive eyes and brows:

```text
Existing Pose
  → Apply LAM ARKit Curves (DA_UpperFaceOnly)
  → Apply LAM Viseme Curves (character Profile)
  → Output Pose
```

UpperFaceOnly disables writing `jaw*`, `mouth*`, and `tongueOut`; it **does not clear mouth curves already present in the input pose**. Avoid simultaneously driving the same mouth through a full ARKit node or Set Morph Target. Pausing holds the frame, seeking follows the new time, and stopping uses the component's 100 ms fade back to the input pose. Conversion adds no temporal smoothing.

### TemplateFit settings and limits

**ConversionMode=TemplateFit** fits 14 non-neutral weights to the selected forward recipe, with nonnegative weights whose sum is at most 1. **sil** is the residual `1 − sum`, not an audio-silence detector. Set its Target Name to None if no neutral morph is needed.

For an unavailable input channel such as `tongueOut`, set **Input Corrections → FitWeight=0**. Scale=0 instead tells the fitter that the observed value is zero. Use **Viseme Gains** for the 14 output gains; sil's gain is ignored. Vowel Gains and the opening / width / roundness thresholds belong to FiveVowelRules. Validate the profile with **Validate Assets** after changes.

Both template matrices have rank 11, so 14 weights cannot always be recovered uniquely. TalkingHead's CH and RR recipes are identical and share weight equally before gains. Expressions such as smiles also affect mouth shape. TemplateFit approximates shapes; it does not reproduce Oculus audio inference or reliably identify spoken consonants. See the [pinned recipes and full constraints](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md).

### Read converted weights in Blueprint

```text
Get Current Expression Frame
  → Convert ARKit To Visemes (Frame, Profile)
  → Get Vowel Weights / Get Viseme Weight
```

The fixed 15-slot order is `sil, PP, FF, TH, DD, kk, CH, SS, nn, RR, aa, E, ih, oh, ou`. Check **bValid** before custom application; **Values** do not include **Weight**, so apply Alpha × Weight once. Invalid frames return 15 zeros, including sil. External ARKit input must use the order returned by `Get ARKit Curve Names`, with 52 finite values and valid time / weight metadata.

See [demo assets](/LAMAudio2Expression-UE/en/demo/) and [release validation](/LAMAudio2Expression-UE/en/validation/#release-validation). The plugin bundles the MIT-licensed recipe data and notices; it does not require the OpenFaceFX, TalkingHead, Blender, or Oculus runtimes. [Credits](/LAMAudio2Expression-UE/en/licenses/#viseme-templates)
