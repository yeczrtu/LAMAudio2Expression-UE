---
title: "音声・表情の再生制御とイベント"
description: "LAM Audio2Expressionの再生、一時停止、シーク、音量、Submix、3D音声、終了イベントをBlueprintで制御する方法。"
sidebar: {"label":"再生制御"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

`LAMAudio2ExpressionComponent`は音声と表情の同期を担当します。既定のPlayback Settingsを設定して`Play Expression Clip`を呼ぶか、呼び出しごとに`Play Expression Clip With Settings`へ設定を渡します。

## 基本操作

| 操作 | 動作 |
| --- | --- |
| Pause / Resume | 音声、表情、フェードの状態を保持して一時停止・再開 |
| Stop | 再生を終了し、100 msで元の表情へ戻す |
| Seek | 同じPlayback Idで位置を変更。一時停止状態を維持 |
| Fade Out And Stop | 音量を線形に下げて停止 |
| Set Volume / Set Muted | 再生中の音量・ミュートを変更。ミュート中も表情と位置は進む |

停止後のSeekは新しい再生を開始します。速度は1倍、ループは対象外です。ループ指定のSoundWaveは拒否されます。

<!-- guide:playback-controls:start -->
<figure class="guide-figure" id="figure-playback-controls" data-guide="playback-controls">
<div class="guide-shot" style="--shot-ratio:564/548;--shot-width:363.12057%;--shot-left:-104.78723%;--shot-top:-31.20438%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-controls.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="PキーからPause、RキーからResume、SキーからTime Seconds 1.0のSeekを呼ぶBlueprint。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:2.37226%;--w:47.87234%;--h:19.34307%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:37.59124%;--w:47.87234%;--h:19.34307%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.8227%;--y:72.81022%;--w:47.87234%;--h:24.63504%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Pause・Resume・Seekの接続例</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-controls.jpg">原寸画像を開く</a></p>
<ol>
<li>PからPauseを呼び、一時停止します。</li>
<li>RからResumeを呼び、再開します。</li>
<li>SからSeekを呼び、1.0秒の位置へ移動する例です。</li>
</ol>
<p>撮影用Blueprintのキー割り当てです。Actorが入力を受け取る設定にし、同じLAMでClipを再生してから操作します。デモのキー割り当てとは異なります。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:playback-controls:end -->

## 音声の出力先

**Output Submix**が未指定なら元音声・SoundClass・エンジンの設定を継承し、指定すると主出力を置き換えます。**Additional Submix Sends**は並列の追加送信です。レベルは0〜1、同じ送信先を複数指定した場合は最後を採用します。

`Set Output Submix`・`Set Submix Send`・`Remove Submix Send`は再生中にも使え、次回の既定設定にも反映されます。SoundWaveと解析Clip自体は変更しません。同じClipを異なるコンポーネントで別の出力先へ再生できます。同じ親Submixへ複数経路が合流すると音量が加算されます。

**Inherit SoundWave Sends**は既定で有効です。**Sound Class Override**と**Concurrency Settings**は未指定なら元音声から継承します。

<!-- guide:playback-settings:start -->
<figure class="guide-figure" id="figure-playback-settings" data-guide="playback-settings">
<div class="guide-shot" style="--shot-ratio:438/265;--shot-width:467.57991%;--shot-left:-324.65753%;--shot-top:-147.92453%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-settings.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Playback SettingsのOutput Submix、Additional Submix Sends、Sound Class Override、Volume 1.0、Muted無効。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:8.21918%;--y:17.73585%;--w:89.72603%;--h:36.98113%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:8.21918%;--y:78.11321%;--w:61.41553%;--h:20.37736%"><b>2</b></span>
</div>
<figcaption>
<p><strong>コンポーネントの出力先と音量</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-settings.jpg">原寸画像を開く</a></p>
<ol>
<li>Output Submixと追加送信を設定します。Noneは元音声・エンジンの設定を継承します。</li>
<li>撮影例のVolumeは1.0、Mutedは無効です。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:playback-settings:end -->

## 3D再生とゲーム停止

Playback Modeは既定でTwo Dimensionalです。Three DimensionalではAttachment、Socket、Transform、Attenuation Settingsを指定できます。Attachment未指定なら所有ActorのRootへ接続し、未登録・別World・存在しないSocketはエラーです。

**Play When Game Paused**は既定でfalseです。有効時はゲーム停止中も再生できます。メッシュのAnimBPも更新するには、SkeletalMeshComponentの**Tick Even When Paused**を有効にします。

<!-- guide:playback-3d:start -->
<figure class="guide-figure" id="figure-playback-3d" data-guide="playback-3d">
<div class="guide-shot" style="--shot-ratio:463/175;--shot-width:442.33261%;--shot-left:-307.12743%;--shot-top:-372.57143%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/playback-3d.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Playback ModeのTwo DimensionalとThree Dimensionalの選択肢、Attachment、Socket、Attenuation Settings。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:35.42117%;--y:4%;--w:29.80562%;--h:37.71429%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:7.77538%;--y:50.85714%;--w:84.88121%;--h:42.85714%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Playback Modeと3D音声の設定箇所</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/playback-3d.jpg">原寸画像を開く</a></p>
<ol>
<li>Playback ModeでThree Dimensionalを選択します。撮影時は選択肢を開いた状態です。</li>
<li>TransformとAttenuation Settingsも用途に合わせて設定します。AttachmentとSocketはメニューを閉じると確認できます。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:playback-3d:end -->

## 終了イベントを使い分ける

| イベント | 発火条件・用途 |
| --- | --- |
| On Playback Started | 再生が受け付けられたとき。Seekでは再通知しない |
| On Playback Finished | 音声の自然終了のみ。次の台詞へ進む用途 |
| On Playback Ended | 受け付け済み再生の終了時に一度。ReasonでCompleted / Stopped / Replaced / Interrupted / Failedを区別 |
| On Playback Failed | 入力不正・再生拒否など。Error.Code / Error.Messageを確認 |
| On Playback State Changed | Starting / Playing / Paused / FadingIn / FadingOut / Stopped / Failed |

通知にはコンポーネント内で単調増加するPlayback IdとClipを含みます。イベント内で次の再生を始める場合も、現在のClipを再取得せず、イベント引数で対象を判断してください。`Get Playback Info`で状態、位置、長さ、進捗を取得できます。

事前検証で拒否された要求はFailedだけを通知し、既存の再生を維持します。再生開始後はState Changed → Ended → Finished（自然終了）またはFailedの順です。Actor破棄・PIE終了中は通知を抑止します。自然終了は元音声の終端で、リバーブなどの残響終了は待ちません。

## 事前解析したClipの再生（v0.3.0）

プラグイン1f06ac8では、[保存済みClip](/LAMAudio2Expression-UE/baked-clips/)を同じ再生ノードに指定できます。先にClipと音声をロードし、ストリーミング音声にはPrime Soundを早めに呼びます。解析待ちは省けますが、音声出力までの遅延をゼロにするものではありません。v0.3.0の配布ZIPに含まれます。
