# LAM Audio2Expression for Unreal Engine

[![Release](https://img.shields.io/github/v/release/yeczrtu/LAMAudio2Expression-UE?style=flat-square&color=6366f1)](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/latest)
![Unreal Engine 5.8.2](https://img.shields.io/badge/Unreal_Engine-5.8.2-313131?style=flat-square&logo=unrealengine)
![Windows x64](https://img.shields.io/badge/Windows-x64-0078d4?style=flat-square)
[![Code License MIT](https://img.shields.io/badge/Code_License-MIT-22c55e?style=flat-square)](LICENSE)

**音声から、ARKit 52種類の表情カーブを生成。** SoundWaveを解析し、音声に合わせてキャラクターの口や表情を動かすランタイムプラグインです。推論はPC内で完結し、Pythonや外部サーバーは不要です。

**[モデル入りZIPをダウンロード](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip)** · **[導入](#導入)** · **[Windowsデモ](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo#ダウンロード)** · **[Blueprintの接続例](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/FACE_DEMO.md)**

## デモ動画

https://github.com/user-attachments/assets/e93530a4-197d-4a56-860c-0890da1002e7

日本語音声6件／1分24秒・音声あり。動画・音声・デモ演出：CC BY-SA 4.0。キャラクター：hinzka / VRoid、音声：JVNV / litagin。[出典・利用条件](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/VIDEO.md)

## できること

- **音声と表情を同期** — Blueprintで解析・再生・一時停止・シーク。AnimGraphの1ノードで52カーブを適用。
- **再生を細かく制御** — 2D / 3D、サブミックス、音量、ミュート、フェード、開始・終了イベント。
- **マイク・PCMからライブ生成** — 推論間隔を約33〜1000 msに調整。実マイクの長時間運用は未検証です。

| 動作環境・入力 | 対応内容 |
|---|---|
| エンジン / OS | **UE 5.8.2 / Windows x64** |
| キャラクター | ARKit 52対応Morph Target、またはカーブで駆動するリグ |
| 音声 | SoundWave · mono / stereo · 8〜192 kHz · 最大5分 |
| 推論 | DirectML優先、CPUフォールバック |

検証環境はRTX 3070 / Core i7-12700です。SoundCue・MetaSound・Procedural SoundWave・外部WAV/MP3の直接読み込みは対象外。再生は1倍速・ループなしです。

## 導入

1. [v0.2.0のモデル入りZIP（約376 MiB）](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip) をダウンロード。
2. UEエディタを終了し、ZIP内の `LAMAudio2Expression` を `<Project>/Plugins/` に配置。
3. プロジェクトで **LAM Audio2Expression** を有効にし、必要に応じて再起動。
4. Project Settings → **LAM Audio2Expression** のModelが `/LAMAudio2Expression/Models/LAM_A2E` であることを確認。
5. パッケージ化する場合は、Packaging → **Additional Asset Directories to Cook** に `/LAMAudio2Expression/Models` を追加。

> [!NOTE]
> ビルド済みプラグイン・ソース・モデルを同梱しています。GitHubの「Source code」ZIPにはモデルとバイナリが含まれません。同梱の `Source` と `Intermediate/Build` は保持してください。[詳しい導入手順](Docs/RELEASE_INSTALL.md)

## Blueprintで使う

Actorに **LAMAudio2ExpressionComponent** を追加し、コンポーネントとSoundWaveを解析ノードに渡します。

```text
Blueprint   Analyze SoundWave Async → Completed → Play Expression Clip
AnimGraph   元のポーズ → Apply LAM ARKit Curves → Output Pose
```

`Completed` のClipを再生ノードへ、表情を供給するコンポーネントをAnimGraphノードの **Source Component** へ接続。**Alpha** で強さ、**Curve Profile** で名前変換・倍率・マスクを調整できます。

自然終了は `On Playback Finished`、停止や差し替えを含む終了は `On Playback Ended` で受け取れます。[再生制御・イベントの詳細](Docs/PLAYBACK_AND_LIVE.md)

## ドキュメント

[Blueprint・表情・ライブ入力](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/USAGE.md) · [検証結果](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/VALIDATION.md) · [ソースからビルド](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/DEVELOPMENT.md) · [問題を報告](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues)

## ライセンス

独自コードは **[MIT](LICENSE)**。[LAM Audio2Expression](https://github.com/aigc3d/LAM_Audio2Expression) 由来のコードと学習済みモデルは **Apache-2.0** です。キャラクター・音声などのデモ素材は[別リポジトリ](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo#ライセンスクレジット)で配布しています。

再配布時は `LICENSE`・`THIRD_PARTY_NOTICES.md`・`Licenses` の表記を保持してください。[第三者表記](THIRD_PARTY_NOTICES.md) · [モデルの出典と運用](Docs/MODEL_MANAGEMENT.md)
