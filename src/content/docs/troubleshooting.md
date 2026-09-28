---
title: "導入・リップシンクのトラブル対処"
description: "モデルが見つからない、表情が動かない、音声が読み込めない、ライブ入力が遅れる場合のLAM Audio2Expression確認手順。"
sidebar: {"label":"トラブル対処"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"}]
---

## モデルが見つからない

GitHubの「Source code」ではなく、**モデル入りRelease ZIP**を取得したか確認します。Project SettingsのModelは`/LAMAudio2Expression/Models/LAM_A2E`です。

Editorで動いて配布版で失敗する場合は、PackagingのAdditional Asset Directories to Cookへ`/LAMAudio2Expression/Models`を追加します。[導入手順](/LAMAudio2Expression-UE/installation/)

## 音声は鳴るが顔が動かない

1. キャラクターのAnimBPに`Apply LAM ARKit Curves`が接続されているか確認します。
2. Source Componentが解析・再生を担当するコンポーネントを参照しているか確認します。
3. Alphaが0ではなく、Curve Profileで必要なカーブを無効にしていないか確認します。
4. メッシュが`jawOpen`など対応するMorph Targetを持つか、カーブをリグへ渡しているか確認します。

ボーン駆動の独自リグへの自動変換はありません。[表情カーブ設定](/LAMAudio2Expression-UE/expression-curves/)を参照してください。

## 解析が拒否される・音声を渡せない

対象は通常のmono / stereo SoundWave、8〜192 kHz、最大300秒です。SoundCue、MetaSound、Procedural SoundWave、外部WAV / MP3の直接入力は対象外です。通常再生ではループ指定も拒否されます。

非同期解析の`Failed`と、再生の`On Playback Failed`でErrorを記録します。新しい解析を同じコンポーネントで開始すると、以前の解析はキャンセルされます。[Blueprint接続](/LAMAudio2Expression-UE/blueprint/)

## 初回だけ時間がかかる

モデルロードとNNEインスタンス初期化があります。`Progress`をロード表示へつなぎ、`Completed`の後に再生します。初回と、初期化後の固定窓の時間を混同しないでください。[検証結果](/LAMAudio2Expression-UE/validation/)

## マイクやライブ入力が動かない・遅れる

Windowsのマイク権限、録音デバイス、`On Live State Changed`を確認します。`Get Live Metrics`で初期化、結果到着P95、破棄区間数を確認し、同じワーカーへの大量の通常解析を避けます。PCMはゲームスレッドから小さいチャンクで渡します。

マイク音声はスピーカーへ折り返しません。音が聞こえないことだけでは取得失敗と判断できません。[ライブ入力](/LAMAudio2Expression-UE/live-input/)

## 自然終了イベントで次の台詞へ進めたい

`On Playback Finished`を使用します。`On Playback Ended`はStopや差し替えでも発火するため、次の台詞を無条件に開始しないでください。イベント引数のPlayback Idで対象を判断します。[再生制御](/LAMAudio2Expression-UE/playback/)

## 問題を報告する

[プラグインのIssue](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues)か[デモのIssue](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/issues)へ、使用版、UE版、Editor / Shippingなどの構成、CPU / GPU、再現手順、Error.Code / Error.Messageを添えてください。配布版かソース版かも記載すると切り分けやすくなります。
