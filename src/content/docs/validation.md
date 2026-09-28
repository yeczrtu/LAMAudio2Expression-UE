---
title: "検証結果と対応範囲"
description: "UE 5.8.2、Windows x64、RTX 3070環境でのLAM Audio2Expressionの数値一致、Blueprint、Shipping、ライブPCM検証と未検証範囲。"
sidebar: {"label":"検証結果"}
appliesTo: "公開ソースのスナップショット · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/VALIDATION.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VALIDATION.md"}]
---

公開資料の**2026年9月24〜25日**の結果を要約します。環境はUE 5.8.2、Windows x64、Visual Studio 2022 / MSVC 14.44、Core i7-12700、RTX 3070、メモリ64 GBです。配布版・後続ソース版・旧測定を区別して記載します。

## モデルの数値一致

固定窓、FP32、opset17、後処理前の3,328値をPyTorch基準値と比較した最大絶対誤差です。

| 入力 | Python ONNX Runtime | UE CPU | UE DirectML |
| --- | ---: | ---: | ---: |
| noise / style 0 | 7.2122e-6 | 7.2122e-6 | 1.7136e-6 |
| silence / style 0 | 1.7509e-7 | 1.70e-7 | 6.3e-8 |
| noise / style 11 | 4.2617e-6 | 4.232e-6 | 1.088e-6 |

すべて1e-3以下でした。これはテンソルの数値比較であり、主観的なリップシンク品質の評価とは異なります。

## 0.2の機能テスト

| 検証 | 公開結果 |
| --- | --- |
| UE Automation | 5件成功。デコーダー、境界・カーブ、PIE破棄・キャンセル、ライブ時刻、モデル一致 |
| 再生制御・ライブ追加テスト | Editor / Development / Shippingで各5件成功 |
| 既存パッケージ回帰 | Development / Shippingで各10件成功 |
| Shipping顔デモ | JVNV 6音声すべて成功 |

音量・Submix・Concurrency・ゲーム停止・終了イベントのほか、ライブ間隔の実行中変更とCPU競合時の回復を確認しています。記録は[Shipping追加テスト](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/Shipping-playback-controls-0.2.json)と[Automation結果](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/automation-0.2.json)で確認できます。

## Blueprint版の顔デモ

**v0.2.0 ZIPより後の公開ソース版**では、3つのBlueprintがエラー・警告なしでコンパイルされました。Editor Standalone / Development / Shippingで各6音声が成功し、HUDクリックから解析・再生・AnimGraphのjawOpen出力まで確認しています。[18件の記録](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/Validation/blueprint-demo-results.json)

44アセットの依存関係にデモ用C++モジュールへの参照がないことも確認しました。キーボードの実入力・物理マイク入力はこの自動検証に含まれません。

## 性能値の読み方

初版のShipping / DirectML / NullRHI測定では、5分の音声は推論＋後処理3.489秒、モデルロード・デコードを含め8.220秒でした。別プロセスのCPU強制試験では、それぞれ74.965秒と78.519秒です。プロセス起動時間は含みません。

同一プロセス内の初期化後の固定窓はCPU約50〜57 ms、DirectML約6.7 msでした。短い窓の値を長時間解析へ単純に外挿できません。Shippingプロセス全体のピークWorking Setは約1,748〜1,775 MiBで、UE・モデル・NNEを含みます。

0.2ではP95から初期化を分離し、Result Latencyにワーカー待ちを含めています。旧測定のP95とは定義が異なります。NullRHIの機能確認を通常描画時の性能保証として扱わないでください。

## 未検証の範囲

- 実マイクの取得・切断・オーバーフロー、長時間ライブ運転。
- 多数キャラクター、長時間のキャッシュ圧迫・メモリリーク試験。
- 実GPUデバイス喪失・ドライバー故障、破損Cookチャンク。
- 物理音声出力を含めた同期誤差、実ソケット追従、距離減衰・定位の聴感。
- 発話全体の主観品質と他キャラクターへの適用。

再実行方法は[開発・リリース](/LAMAudio2Expression-UE/development/)を参照してください。
