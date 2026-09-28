---
title: "Playback controls and events"
description: "Control synchronized audio and expressions in Blueprint: pause, seek, volume, submix routing, 3D audio, game pause behavior, and playback completion events."
sidebar: {"label":"Playback controls"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
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

## Audio routing

An unset **Output Submix** inherits the source audio / SoundClass / engine routing. Setting it replaces the main output. **Additional Submix Sends** adds parallel sends. Levels range from 0 to 1; the last entry wins for duplicate destinations.

`Set Output Submix`, `Set Submix Send`, and `Remove Submix Send` work during playback and update the component's defaults for the next play. They do not modify the SoundWave or analyzed Clip. Different components can play the same Clip through different routes. Parallel paths that reach the same parent submix are summed.

**Inherit SoundWave Sends** is enabled by default. Unset **Sound Class Override** and **Concurrency Settings** inherit from the source audio.

## 3D playback and game pause

Playback Mode defaults to Two Dimensional. Three Dimensional supports Attachment, Socket, Transform, and Attenuation Settings. An unset Attachment uses the owner's root. Unregistered components, another World, or a missing socket produce an error.

**Play When Game Paused** defaults to false. Enable it to continue playback during game pause. Also enable **Tick Even When Paused** on the SkeletalMeshComponent if its AnimBP must continue updating.

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

## Play a baked clip (published source)

Plugin 1f06ac8 accepts [saved clips](/LAMAudio2Expression-UE/en/baked-clips/) in the same playback nodes. Load the clip and audio first, and call Prime Sound early for streaming audio. This removes analysis waits, not audio output latency. The feature is not included in the v0.2.0 ZIPs.
