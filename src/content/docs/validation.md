---
title: "検証結果と対応範囲"
description: "UE 5.8.2、Windows x64、RTX 3070環境でのLAM Audio2Expressionの数値一致、Blueprint、Shipping、ライブPCM検証と未検証範囲。"
sidebar: {"label":"検証結果"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64 · 2026-09-30"
sources: [{"label":"Demo / Docs/VALIDATION.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VALIDATION.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"},{"label":"Demo / baked-clips-results.json · f3b6f13","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/Validation/baked-clips-results.json"},{"label":"v0.3.0 / validation.json · 2026-09-28","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/validation.json"},{"label":"v0.3.0 / release-manifest.json","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json"}]
---

現在の配布版は**v0.3.0**です。以下は**2026年9月28日**の公開リリース検証を9月30日に確認した内容です。今回のドキュメント更新でUEテストや性能測定を再実行したものではありません。従来のソース検証結果は、日付と条件を分けて後半に残しています。

<span id="release-validation" class="comparison-anchor" aria-hidden="true"></span>

## v0.3.0配布版の検証

ビルド元は**Plugin 1f06ac8 / Demo f3b6f13**。環境は**UE 5.8.2、Windows 11 x64、Core i7-12700、RTX 3070**です。[配布版の検証JSON](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/validation.json)と[マニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json)に次の結果が記録されています。

| 対象 | 公開結果 |
| --- | --- |
| BuildPlugin | Editor Development・Game Development・Game Shipping成功 |
| デモビルド | Editor Development・前提ランタイム込みWin64 Shipping成功 |
| UE Automation | 全12件成功、うち警告あり2件。失敗0件 |
| Bake保存フィクスチャ | 2件成功、うち警告あり1件。失敗0件（別実行） |
| 展開ZIPの整合性 | 3種のCRC・SHA-256成功。単体・プロジェクト同梱プラグインはバイト一致 |
| 展開後Editorのモデル | 数値比較成功。CPU・DirectMLの誤差は0.001未満 |
| 展開後顔デモ | Editorで6音声すべて成功 |
| 展開後Shipping再生制御 | 保存済みClip、ライブ間隔、CPU競合を含む6ケース成功 |
| 展開後Shippingスモーク | Blueprint、ライブPCM、300秒音声を含む10ケース成功 |
| 展開後Shipping Viseme | 5母音・OpenFaceFX・TalkingHeadの各構成で6音声すべて成功 |

警告には不正なBake入力の意図的拒否、音声DDCキー警告、WASAPI raw-modeのフォールバックが含まれます。全アサーションは成功しています。警告あり2件は全12件の内数で、14件ではありません。Bake保存フィクスチャは別実行の記録です。

展開後Editor・Shippingの代表画像では、顔・マテリアル・口形状・HUDの表示、テクスチャ欠落や空キャプチャがないことを目視確認済みと記録されています。機能・表示の確認であり、他方式とのリップシンク品質比較ではありません。

### 配布版の性能記録の例

展開後ShippingのDirectML試験では、300秒音声（4,800,000サンプル・9,000フレーム）が結果欄の**推論3.293秒・合計7.529秒**でした。公開試験の記録値であり、リアルタイムの提示遅延や今回の新規測定ではありません。

| 保存済みClipの再生ケース | 短いClipのサンプリングP95 | 300秒ClipのサンプリングP95 | 動的Viseme変換P95 |
| --- | --- | --- | --- |
| Editor | 0.201 µs | 0.298 µs | 58.699 µs |
| 展開後Shipping | 0.100 µs | 0.200 µs | 110.400 µs |

両ケースで`baked_model_unloaded=1`が記録されています。サンプリングとViseme変換は別のマイクロベンチマークで、ストレージ読み込み、物理音声出力までの遅延、ゲーム全体のフレーム時間を含みません。再生がゼロレイテンシーになる意味ではありません。後半の旧ソース測定は別実行で、v0.3.0配布版の測定値として扱いません。

配布版の記録では**Android、他のUE版、実マイクの長時間取得、実GPU故障、物理音声出力までの遅延**は未検証です。Wav2ARKitの機能ブランチは含まれていません。

## 過去の記録：9月24〜25日

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

<span id="baked-clips" class="comparison-anchor" aria-hidden="true"></span>

## 事前解析Clipの検証（2026-09-28）

この節は**プラグイン1f06ac8 / デモf3b6f13、UE 5.8.2 / Win64**の[公開検証記録](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/Validation/baked-clips-results.json)に基づきます。v0.2.0の結果とは別で、本サイト作成時にUEテストを再実行したものではありません。

- 自動テスト12件成功（警告なし10件・警告あり2件、失敗0件）。生成・再生成、データ検証を含みます。
- Editor / Development / Shippingそれぞれ6件、計18件の再生テストがPASS。各構成に保存済みClipの再生ケースを含みます。
- 同じ入力・設定によるCPUの動的解析との最大絶対カーブ誤差は0（許容値0.00001）。
- 保存済みClipの再生ケースではモデル未ロードを確認。再生制御、ルーティング、3D、Concurrencyなども確認しています。

| 構成 | 短いClipのサンプリングP95 | 300秒ClipのサンプリングP95 | 動的Viseme変換P95 |
| --- | --- | --- | --- |
| Editor | 0.201 µs | 0.399 µs | 78.700 µs |
| Development | 0.200 µs | 0.200 µs | 53.800 µs |
| Shipping | 0.200 µs | 0.300 µs | 65.400 µs |

これらはローカルのマイクロベンチマークです。モデル推論時間、ゲーム全体のフレーム時間、再生開始待ち、物理音声出力までの遅延ではありません。記録はCPU / GPU型番を示していないため、上記の旧測定環境と同一とは扱いません。Viseme変換は保存済みClipから独立した実行時処理です。実マイク取得と生成ダイアログの目視確認は、この記録では未実施です。

[生成・先読み・パッケージ化と再実行手順](/LAMAudio2Expression-UE/baked-clips/)
