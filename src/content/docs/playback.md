---
title: "音声・表情の再生制御とイベント"
description: "LAM Audio2Expressionの再生、一時停止、シーク、音量、Submix、3D音声、終了イベントをBlueprintで制御する方法。"
sidebar: {"label":"再生制御"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
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

## 音声の出力先

**Output Submix**が未指定なら元音声・SoundClass・エンジンの設定を継承し、指定すると主出力を置き換えます。**Additional Submix Sends**は並列の追加送信です。レベルは0〜1、同じ送信先を複数指定した場合は最後を採用します。

`Set Output Submix`・`Set Submix Send`・`Remove Submix Send`は再生中にも使え、次回の既定設定にも反映されます。SoundWaveと解析Clip自体は変更しません。同じClipを異なるコンポーネントで別の出力先へ再生できます。同じ親Submixへ複数経路が合流すると音量が加算されます。

**Inherit SoundWave Sends**は既定で有効です。**Sound Class Override**と**Concurrency Settings**は未指定なら元音声から継承します。

## 3D再生とゲーム停止

Playback Modeは既定でTwo Dimensionalです。Three DimensionalではAttachment、Socket、Transform、Attenuation Settingsを指定できます。Attachment未指定なら所有ActorのRootへ接続し、未登録・別World・存在しないSocketはエラーです。

**Play When Game Paused**は既定でfalseです。有効時はゲーム停止中も再生できます。メッシュのAnimBPも更新するには、SkeletalMeshComponentの**Tick Even When Paused**を有効にします。

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

## 事前解析したClipの再生（公開ソース版）

プラグイン1f06ac8では、[保存済みClip](/LAMAudio2Expression-UE/baked-clips/)を同じ再生ノードに指定できます。先にClipと音声をロードし、ストリーミング音声にはPrime Soundを早めに呼びます。解析待ちは省けますが、音声出力までの遅延をゼロにするものではありません。v0.2.0のZIPには未収録です。
