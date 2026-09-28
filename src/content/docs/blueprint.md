---
title: "Blueprintで音声と表情を接続する"
description: "Analyze SoundWave AsyncからPlay Expression Clip、AnimGraphのApply LAM ARKit Curvesまで、Unreal Engineで音声リップシンクを組み立てる手順。"
sidebar: {"label":"Blueprintの接続"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"}]
---

音声の解析と、解析結果の再生を分けて接続します。Actorには`LAMAudio2ExpressionComponent`、表示するメッシュにはAnimation Blueprintが必要です。

## SoundWaveを解析する

```text
BeginPlay または任意のイベント
  → Analyze SoundWave Async(Component, SoundWave, Settings)
      Completed(Clip) → Play Expression Clip(Clip, StartTime=0)
      Progress       → ロード表示を更新
      Failed         → Errorを表示
      Cancelled      → ロード表示を終了
```

通常のmono / stereo SoundWave、8〜192 kHz、最大300秒に対応します。外部WAV / MP3を直接渡すことはできません。UEへインポートしてSoundWaveとして使用します。

`Completed`のClip出力を再生ノードへ渡します。他のActorでも再利用する場合はClipをBlueprint変数で保持してください。解析中の中断は`Cancel Analysis`です。同じコンポーネントで新しい解析を開始すると、前の解析はキャンセルされます。

## AnimGraphへ表情を適用する

```text
既存のポーズ → Apply LAM ARKit Curves → Output Pose
```

1. キャラクターのAnimBPを開きます。
2. `Apply LAM ARKit Curves`を既存ポーズとOutput Poseの間に接続します。
3. **Source Component**に、解析・再生を担当するLAMコンポーネントを指定します。
4. **Alpha**を0〜1で調整します。カーブ名が異なる場合は[Curve Profile](/LAMAudio2Expression-UE/expression-curves/)を設定します。

Source Componentが空欄なら、SkeletalMeshを所有するActorから検索します。複数のLAMコンポーネントを持つActorでは明示的に指定してください。

標準名`jawOpen`などと同名のMorph Targetを持つメッシュで利用できます。ボーン駆動のリグでは出力カーブをControl Rigなどへ接続します。プラグインは独自のボーン配置を自動推定しません。

## 再生と状態取得

`Pause`・`Resume`・`Stop`・`Seek`は音声と表情を一緒に制御します。一時停止では表情を保持し、停止では100 msで元のポーズへ戻ります。詳細は[再生制御](/LAMAudio2Expression-UE/playback/)を参照してください。

- `Get Current Expression Frame`：時刻、52値、Validity、適用Weight。
- `Get ARKit Curve Value`：名前を指定したカーブの推定値。停止フェードのWeightは別扱いです。

## 接続済みの例

配布プロジェクトの`/Game/Examples/BP_LAMPlayback`は非同期解析と再生の例、`/Game/Examples/ABP_LAMCurves`はAnimGraphの例です。後者はテスト用スケルトンで作られているため、自分のAnimBPへノード構成をコピーします。

公開ソースのBlueprint顔デモでは、`BP_FaceDemo`の`02_Analyze_And_Play`から接続を確認できます。このデモ実装はv0.2.0 ZIPより新しい版です。[デモのバージョン差](/LAMAudio2Expression-UE/demo/)
