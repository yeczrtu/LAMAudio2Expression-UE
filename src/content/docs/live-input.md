---
title: "マイク・PCMからライブ表情を生成する"
description: "Unreal Engineのマイクと外部PCMをLAM Audio2Expressionへ入力。推論間隔、提示遅延、ライブ状態、P95メトリクスの設定と制約を解説します。"
sidebar: {"label":"マイク・ライブ入力"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"}]
---

事前にSoundWave全体を解析する代わりに、マイクやPCMストリームから継続的に表情を生成できます。ライブ入力と通常解析は同じ専用ワーカーを使用します。

## マイク入力

`Start Microphone(Settings, DeviceIndex=-1)`で既定の録音デバイスを使用します。Windowsのマイクアクセス許可が必要です。終了には`Stop Microphone`を呼びます。

キャプチャ音声のスピーカーへの折り返しは行いません。起動時にはモデルロードと初期化が必要です。あらかじめSoundWaveを解析してモデルアセットをロードしておくこともできます。

## 外部PCMを渡す

```text
Start PCM Stream(Settings)
  → ゲームスレッドから繰り返し
      Push PCM Audio(InterleavedPCM, SampleRate, Channels)
  → Stop Microphone
```

- float値域は−1〜1、mono / stereoのインターリーブ形式。
- サンプルレートは8〜192 kHz、1コール最大2秒。
- サンプルレートを変更する場合はストリームを再開始。
- まとめて大きく渡すより、小さいチャンクを定期的に渡します。

## 更新間隔と提示遅延

`Set Live Inference Interval`の単位はmsです。約33.3〜1000 msの範囲で、内部では1〜30フレームに丸めます。既定は10フレーム、約333.3 msです。実行中の変更は次の推論ジョブから反映されます。`Get Live Inference Interval`で丸めた要求値を確認できます。

**Presentation Delay**は希望する最小遅延で、既定0.75秒、設定範囲0.4〜2秒です。実効値は要求下限と「処理間隔＋結果到着遅延P95＋1フレーム」の大きい方で、最大2秒です。実行中は増加方向にだけ調整し、表示時刻を巻き戻しません。縮小は次の開始時に再計算します。

間隔を短くすると更新頻度は上がりますが、実時間動作は機器性能と同時負荷に依存します。指定したmsで結果が届く保証ではありません。

## 実行状態を確認する

`Get Live Metrics`でActual Interval、Effective Presentation Delay、Inference P95、Result Latency P95、Initialization、Dropped Intervals、Backend、Stateを取得します。P95は直近60回で、準備中の0は未計測です。Result Latencyはワーカー待ちを含み、Initializationは分離されます。

`On Live State Changed`はPreparing / Running / Lagging / Failed / Stoppedを通知します。入力待ちキューは最大2秒、同時推論はセッションあたり1件です。処理が遅れると古い待ち仕事を破棄し、最新位置へ復帰します。結果のない区間では100 ms保持後に100 msで表情を戻します。

## 検証済みの範囲

公開テストはPCM連続入力、実行中の間隔変更、CPU / DirectML、ワーカー競合を扱っています。**実マイクの取得・切断、長時間運転は未検証**です。機能テストのP95を運用保証として扱わず、対象環境で確認してください。[検証結果](/LAMAudio2Expression-UE/validation/)
