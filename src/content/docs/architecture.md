---
title: "推論・同期・AnimGraphの実装構成"
description: "LAM Audio2ExpressionのUEモジュール構成、専用ワーカー、音声時刻との同期、表情カーブのスナップショットと解析キャッシュを説明します。"
sidebar: {"label":"アーキテクチャ"}
appliesTo: "公開ソースのスナップショット · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/ARCHITECTURE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/ARCHITECTURE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"}]
---

## 処理の流れ

```text
SoundWave → デコード → 16 kHz PCM → 固定窓推論 → 後処理 → Clip
                                                             ↓
音声再生時計 → 現在の表情スナップショット → AnimGraph → ARKitカーブ
```

`LAMAudio2Expression`はRuntimeモジュールです。`LAMAudio2ExpressionEditor`はUncookedOnlyで、AnimGraph編集ノード、モデル設定、接続例生成、PIEテストを担当します。エンジンの改変は不要です。

| 実装 | 担当 |
| --- | --- |
| LAMAnalyzeAsync | 非同期ロード、解析ジョブ、BP通知、PCMハッシュ、LRUキャッシュ |
| LAMDecoder | Cook済みSoundWaveのデコード、ストリーミングチャンク保持 |
| LAMCore | リサンプル、窓切り出し、NNE推論、CPU代替、後処理 |
| LAMAudio2ExpressionComponent | 再生制御、同期時計、表情スナップショット |
| AnimNode_LAMARKit | スナップショット取得とカーブ合成 |
| LAMLive | マイク・PCM入力、連続リサンプル、ライブ推論、提示遅延 |
| LAMTypes | 52カーブ名、設定、Clip、Profile、補間 |

## スレッドと寿命

モデルUObjectのロードとNNEモデル作成はゲームスレッドです。デコード、リサンプル、NNEモデルインスタンス作成、推論、後処理は専用ワーカー1本で処理します。Blueprintへの完了・進捗通知はゲームスレッドです。

実行中のNNE呼び出しそのものは中断しません。キャンセルフラグで後続処理と結果通知を抑止します。EndPlayや非同期アクション破棄時もジョブを無効化します。

AnimNodeはPreUpdateでカーブ配列とProfileをコピーし、Evaluate_AnyThreadではそのコピーとSourcePoseだけを使用します。この段階で推論やUObject検索は行いません。

## 解析窓と時間

通常解析は16,000サンプルずつ進み、各34,133サンプル窓の64出力フレーム中34〜63を採用します。開始前・終了後はゼロで補います。最終フレーム数は整数演算`(samples * 30 + 15999) / 16000`で求めます。

再生では音声時刻を基準に、次のコールバックまで最大1/30秒の補間予測を許可します。Seekで内部音声を再生成する際は旧再生の通知を無視します。物理的な出力デバイス遅延の補償は含まれません。

## キャッシュと共有

解析キャッシュは既定64 MiBで、16 kHz PCMのSHA-1、モデルGUID、スタイル、後処理設定、処理版がキーになります。再生Clipは結果配列を独立して所有するため、キャッシュから追い出されても消失しません。モデルと利用者が保持するClipのメモリはこの予算とは別です。

## ライブ処理

入力キューは最大2秒で、推論用の過去文脈を別に保持します。古い世代の結果は破棄し、遅延時には最新位置へ復帰します。0.2のライブ間隔は可変で、詳細は[ライブ入力](/LAMAudio2Expression-UE/live-input/)にまとめています。音声のルーティング・Concurrency・イベントは[再生制御](/LAMAudio2Expression-UE/playback/)を参照してください。
