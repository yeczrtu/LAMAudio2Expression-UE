---
title: "Playback controls and events"
description: "Control synchronized audio and expressions in Blueprint: pause, seek, volume, submix routing, 3D audio, game pause behavior, and playback completion events."
sidebar: {"label":"Playback controls"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

`LAMAudio2ExpressionComponent` keeps audio and facial expressions synchronized. Set default Playback Settings and call `Play Expression Clip`, or pass settings per call with `Play Expression Clip With Settings`.

## Basic controls

| Operation | Behavior |
| --- | --- |
| Pause / Resume | Hold and resume audio, expressions, and fades |
| Stop | End playback and return to the input expression over 100 ms |
| Seek | Change position under the same Playback Id; preserve paused state |
| Fade Out And Stop | Fade volume linearly, then stop |
| Set Volume / Set Muted | Change live volume or mute; expressions and time continue while muted |

A seek after stopping starts new playback. Playback is at 1× speed; looping is unsupported. A SoundWave with looping enabled is rejected.

<!-- guide:playback-controls:start -->
<figure class="guide-figure" id="figure-playback-controls" data-guide="playback-controls">
<div class="guide-shot" style="--shot-ratio:564/548;--shot-width:363.12057%;--shot-left:-104.78723%;--shot-top:-31.20438%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-controls.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Blueprint calling Pause from P, Resume from R and Seek with Time Seconds 1.0 from S." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:2.37226%;--w:47.87234%;--h:19.34307%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:37.59124%;--w:47.87234%;--h:19.34307%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:72.81022%;--w:47.87234%;--h:24.63504%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Wiring Pause, Resume and Seek</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-controls.jpg">Open full-size image</a></p>
<ol>
<li>P calls Pause.</li>
<li>R calls Resume.</li>
<li>S calls Seek, moving to 1.0 seconds in this example.</li>
</ol>
<p>These keys belong to the documentation example. Configure the Actor to receive input and first play a clip on the same LAM component. These are not the demo key bindings.</p>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:playback-controls:end -->

## Audio routing

An unset **Output Submix** inherits the source audio / SoundClass / engine routing. Setting it replaces the main output. **Additional Submix Sends** adds parallel sends. Levels range from 0 to 1; the last entry wins for duplicate destinations.

`Set Output Submix`, `Set Submix Send`, and `Remove Submix Send` work during playback and update the component's defaults for the next play. They do not modify the SoundWave or analyzed Clip. Different components can play the same Clip through different routes. Parallel paths that reach the same parent submix are summed.

**Inherit SoundWave Sends** is enabled by default. Unset **Sound Class Override** and **Concurrency Settings** inherit from the source audio.

<!-- guide:playback-settings:start -->
<figure class="guide-figure" id="figure-playback-settings" data-guide="playback-settings">
<div class="guide-shot" style="--shot-ratio:438/265;--shot-width:467.57991%;--shot-left:-324.65753%;--shot-top:-147.92453%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-settings.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Playback Settings showing Output Submix, Additional Submix Sends, Sound Class Override, Volume 1.0 and Muted disabled." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:8.21918%;--y:17.73585%;--w:89.72603%;--h:36.98113%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:8.21918%;--y:78.11321%;--w:61.41553%;--h:20.37736%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Component audio routing and volume</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-settings.jpg">Open full-size image</a></p>
<ol>
<li>Configure Output Submix and additional sends. None inherits the source/engine routing.</li>
<li>The capture shows Volume 1.0 and Muted disabled.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:playback-settings:end -->

## 3D playback and game pause

Playback Mode defaults to Two Dimensional. Three Dimensional supports Attachment, Socket, Transform, and Attenuation Settings. An unset Attachment uses the owner's root. Unregistered components, another World, or a missing socket produce an error.

**Play When Game Paused** defaults to false. Enable it to continue playback during game pause. Also enable **Tick Even When Paused** on the SkeletalMeshComponent if its AnimBP must continue updating.

<!-- guide:playback-3d:start -->
<figure class="guide-figure" id="figure-playback-3d" data-guide="playback-3d">
<div class="guide-shot" style="--shot-ratio:463/175;--shot-width:442.33261%;--shot-left:-307.12743%;--shot-top:-372.57143%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-3d.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Two Dimensional and Three Dimensional options with Attachment, Socket and Attenuation Settings." />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:35.42117%;--y:4%;--w:29.80562%;--h:37.71429%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:7.77538%;--y:50.85714%;--w:84.88121%;--h:42.85714%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Playback Mode and spatial-audio settings</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-3d.jpg">Open full-size image</a></p>
<ol>
<li>Choose Three Dimensional in Playback Mode. The capture shows the open options.</li>
<li>Configure Transform and Attenuation Settings as needed. Close the menu to inspect Attachment and Socket.</li>
</ol>
<p class="guide-provenance">Captured 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · Operation included in v0.3.0.</p>
</figcaption>
</figure>
<!-- guide:playback-3d:end -->

## Choose the right completion event

| Event | Meaning / use |
| --- | --- |
| On Playback Started | Playback accepted; not repeated for seeks |
| On Playback Finished | Natural audio completion only; useful for advancing dialogue |
| On Playback Ended | Once per accepted playback; Reason is Completed, Stopped, Replaced, Interrupted, or Failed |
| On Playback Failed | Invalid input or playback rejection; inspect Error.Code / Error.Message |
| On Playback State Changed | Starting, Playing, Paused, FadingIn, FadingOut, Stopped, or Failed |

Event data includes a Playback Id that increases within the component, and the Clip. Use the event arguments to identify the playback, especially when a listener starts the next clip. `Get Playback Info` returns state, position, duration, and progress.

Preflight rejection emits Failed only and preserves existing playback. After playback has started, notifications are State Changed → Ended → Finished for natural completion, or Failed for a start failure. Notifications are suppressed during Actor destruction and PIE teardown. Natural completion refers to the source audio's end, not the end of reverb tails.

## Play a baked clip (v0.3.0)

Plugin 1f06ac8 accepts [saved clips](/LAMAudio2Expression-UE/en/baked-clips/) in the same playback nodes. Load the clip and audio first, and call Prime Sound early for streaming audio. This removes analysis waits, not audio output latency. The feature is included in the v0.3.0 ZIPs.
