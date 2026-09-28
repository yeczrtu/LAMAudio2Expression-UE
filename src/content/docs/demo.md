---
title: "WindowsデモとUEプロジェクトの使い方"
description: "LAM Audio2Expressionの日本語6音声デモを起動する手順。操作キー、UEプロジェクト、v0.2.0配布版とBlueprintソース版の違いを説明します。"
sidebar: {"label":"デモを試す"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"}]
---

## Windows実行版を起動する

1. [モデル入りWindowsデモ](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-Win64-Demo.zip)をダウンロードします。
2. `C:\LAMDemo`など短いパスへ**すべて展開**し、`LAMDemo.exe`を起動します。
3. 画面左の音声ボタン、または**1〜6**キーで音声を選びます。解析完了後に表情と音声が再生されます。

モデル、プラグイン、キャラクター、6音声は同梱済みです。UEエディタとPythonは不要です。VC++ランタイム不足の場合は、同梱の`Engine/Extras/Redist/en-us/vc_redist.x64.exe`を実行します。

## 操作一覧

| キー | 操作 |
| --- | --- |
| 1〜6 | 音声を選択 |
| Space | 一時停止・再開 |
| R | 先頭から再生 |
| V / − / ＋ | ミュート / 音量を下げる / 上げる |
| O / F | 出力Submix切り替え / フェード停止 |
| M / I | マイク開始・停止 / 推論間隔100・333・1000 ms切り替え |
| Alt + F4 | 終了 |

マイクはWindows側のアクセス許可が必要です。スピーカーへの折り返しはありません。実マイクの長時間運用は未検証です。

## UEで編集する

[モデル入りUEプロジェクト](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Project-Model.zip)をすべて展開し、UE 5.8.2で`LAMDemo.uproject`を開きます。`/Game/LAMFaceDemo/Maps/LAM_FaceDemo`を開いてPlayします。

![Unreal Engine上でキャラクターと音声選択UIを表示した顔デモ](/LAMAudio2Expression-UE/images/face-demo.png)

画像・デモ素材の[出典と利用条件](/LAMAudio2Expression-UE/licenses/)も確認してください。

## 配布版とBlueprint版の違い

:::note[v0.2.0のZIP・動画とソース版]
v0.2.0の配布物と紹介動画は、デモ処理をBlueprintへ移行する前の版です。以下のBlueprint構成は、公開ソースのコミット`275a683`を対象とします。入手・ビルドは[開発手順](/LAMAudio2Expression-UE/development/)を参照してください。
:::

ソース版の入口は`BP_FaceDemo`の`02_Analyze_And_Play`です。`SelectSample` → 前の処理をキャンセル → `Analyze SoundWave Async` → `Completed` → `Play Expression Clip`という接続になっています。

| Blueprint | 役割 |
| --- | --- |
| BP_FaceDemo | 解析、再生、入力、状態イベント |
| BP_FaceDemoHUD | Canvasノードによる画面とクリック処理 |
| BP_FaceDemoGameMode | HUDを指定するGameMode |
| ABP_Face52 | Apply LAM ARKit Curvesで表情を適用 |

これらは`Content/LAMFaceDemo`以下にあります。別プロジェクトへ移す場合はプラグインを導入し、Content BrowserのMigrateを使います。素材の出典・ライセンスを保持してください。通常の自作キャラクターへの導入では、デモ素材をコピーする必要はありません。
