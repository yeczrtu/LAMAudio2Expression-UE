# LAM Audio2Expression for Unreal Engine

**音声からARKit 52種類の表情カーブを生成する、UE用ランタイムプラグインです。**

SoundWaveを解析し、音声再生に合わせてキャラクターの口や表情を動かします。Blueprintで解析・再生を制御し、専用のAnimGraphノードで表情を適用できます。推論はPC内で完結し、Pythonや外部サーバーは不要です。

**[デモ動画を見る](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/LAM_A2E_Demo.mp4)** · **[Windowsデモを試す](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo#ダウンロード)** · **[モデル入りプラグインをダウンロード](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip)**

## できること

- **音声と表情の同期再生**：解析完了後に再生し、一時停止・再開・停止・シークをまとめて制御。
- **AnimBPへの組み込み**：`Apply LAM ARKit Curves` ノードで52カーブを適用。カーブ名の変換、マスク、強さの調整に対応。
- **会話システム向けの再生制御**：2D / 3D、サブミックス、音量、ミュート、フェード、再生開始・終了イベント。
- **マイク・PCM入力**：ライブ推論間隔を約33〜1000 msに調整可能。実マイクの長時間運用は未検証です。

## 動作環境

| 項目 | 対応内容 |
|---|---|
| エンジン・OS | **UE 5.8.2 / Windows x64** |
| キャラクター | ARKit 52カーブに対応したMorph Target、またはカーブから表情を駆動するリグ |
| 音声アセット | mono / stereoのSoundWave、8〜192 kHz、最大5分 |
| 推論 | DirectML対応GPUを優先。利用できない場合はCPUへフォールバック |
| 検証環境 | RTX 3070 / Core i7-12700。最低動作要件を示すものではありません |

SoundCue、MetaSound、Procedural SoundWave、外部WAV／MP3の直接読み込みには対応していません。再生速度は1倍で、ループ再生は対象外です。

## 導入

1. [v0.2.0のモデル入りZIP（約376 MiB）](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip) をダウンロードします。
2. UEエディタを終了し、ZIP内の `LAMAudio2Expression` フォルダーを `<Project>/Plugins/` に配置します。
3. プロジェクトを開き、Pluginsで **LAM Audio2Expression** を有効にします。再起動を求められたら再起動します。
4. Project Settings → **LAM Audio2Expression** のModelが `/LAMAudio2Expression/Models/LAM_A2E` になっていることを確認します。既定で同梱モデルが指定されています。
5. アプリをパッケージ化する場合は、Project Settings → Packaging → **Additional Asset Directories to Cook** に `/LAMAudio2Expression/Models` を追加します。

ZIPにはビルド済みプラグイン、C++ソース、学習済みモデルが含まれます。モデル変換や追加ダウンロードは不要です。`Source` と `Intermediate/Build` はビルドに必要なため、そのまま保持してください。[詳しい導入手順](Docs/RELEASE_INSTALL.md)

GitHubの「Source code (zip / tar.gz)」にはモデルとバイナリが含まれません。上のモデル入りZIPを選んでください。[リリース詳細・チェックサム](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/tag/v0.2.0)

## Blueprintで使う

1. キャラクターのActorに **LAMAudio2ExpressionComponent** を追加します。
2. `Analyze SoundWave Async` にコンポーネントとSoundWaveを渡します。
3. `Completed` から `Play Expression Clip` を呼び、返されたClipを渡します。

```text
Analyze SoundWave Async
    └─ Completed → Play Expression Clip
```

AnimBPでは、元のポーズと出力の間に **Apply LAM ARKit Curves** を接続します。

```text
元のポーズ → Apply LAM ARKit Curves → Output Pose
```

Source Componentに表情を供給するコンポーネントを指定します。Alphaで適用の強さを、Curve Profileでカーブ名・倍率・マスクを調整できます。`jawOpen` など標準のARKit名と同名のMorph Targetがある場合は、既定の対応で使えます。

音声を最後まで再生した後の処理には `On Playback Finished`、停止や差し替えを含む後処理には `On Playback Ended` を使用します。[再生制御・イベントの詳細](Docs/PLAYBACK_AND_LIVE.md)

## デモとドキュメント

[デモプロジェクト](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo) には、キャラクター・日本語音声6件・設定済みBlueprintを用意しています。実行版ならUEエディタなしでも試せます。デモ素材は別リポジトリで配布しています。

- [Blueprint・表情設定・マイク／PCM入力](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/USAGE.md)
- [再生制御・ライブ推論間隔・計測値](Docs/PLAYBACK_AND_LIVE.md)
- [動作確認済みの範囲と検証結果](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/VALIDATION.md)
- [開発者向け：ソースからのセットアップとテスト](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Docs/DEVELOPMENT.md)
- [問題を報告する](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues) — UEのバージョン、CPU/GPU、再現手順を添えてください。

## ライセンス

独自コードは [MIT](LICENSE)。[LAM Audio2Expression](https://github.com/aigc3d/LAM_Audio2Expression) 由来のコードと学習済みモデルは **Apache-2.0** です。モデルをMITへ変更するものではありません。

再配布時は、`LICENSE`、`THIRD_PARTY_NOTICES.md`、`Licenses` の表記を保持してください。[第三者表記](THIRD_PARTY_NOTICES.md) · [モデルの出典と運用方針](Docs/MODEL_MANAGEMENT.md)
