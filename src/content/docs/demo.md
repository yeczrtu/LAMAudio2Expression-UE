---
title: "WindowsデモとUEプロジェクトの使い方"
description: "LAM Audio2Expression v0.3.0のWindowsデモとUEプロジェクト。日本語6音声、操作キー、同梱BlueprintとVisemeサンプルの使い方。"
sidebar: {"label":"デモを試す"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / README.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/README.md"},{"label":"Demo / Docs/FACE_DEMO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/FACE_DEMO.md"},{"label":"Demo / Docs/VIDEO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/VIDEO.md"}]
---

## Windows実行版を起動する

1. [モデル入りWindowsデモ](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-Win64-Demo.zip)をダウンロードします。
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

<!-- guide:demo-controls:start -->
<figure class="guide-figure" id="figure-demo-controls" data-guide="demo-controls">
<div class="guide-shot" style="--shot-ratio:888/500;--shot-width:100%;--shot-left:0%;--shot-top:0%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/face-demo.png" width="888" height="500" loading="lazy" decoding="async" alt="顔デモの音声選択、SpaceとRの再生操作、音量、ライブ状態を表示した既存の実画面。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.7027%;--y:18.2%;--w:21.05856%;--h:38.6%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.59009%;--y:59.2%;--w:21.28378%;--h:14.4%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:26.68919%;--y:3.8%;--w:21.84685%;--h:36.6%"><b>3</b></span>
</div>
<figcaption>
<p><strong>公開済みデモ画面の操作箇所</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/face-demo.png">原寸画像を開く</a></p>
<ol>
<li>1〜6のボタンで音声を選択します。ラベルはサンプル音声の分類です。</li>
<li>Spaceで一時停止・再開、Rで先頭から再生します。</li>
<li>音量・出力・ライブ状態を確認します。表示値は既存画像内の状態で、新しい測定結果ではありません。</li>
</ol>
<p>キャラクター：hinzka / VRoid・pixiv。音声：JVNV / litagin。素材の利用条件はライセンスページを参照してください。</p>
<p class="guide-provenance">既存の公開画像（撮影日不明）。Demo 275a683から無加工で再利用。 <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/face-demo.png">画像の出典</a> · <a href="/LAMAudio2Expression-UE/licenses/">素材のクレジット・利用条件</a></p>
</figcaption>
</figure>
<!-- guide:demo-controls:end -->

## UEで編集する

[モデル入りUEプロジェクト](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Project-Model.zip)をすべて展開し、UE 5.8.2で`LAMDemo.uproject`を開きます。`/Game/LAMFaceDemo/Maps/LAM_FaceDemo`を開いてPlayします。

画像・デモ素材の[出典と利用条件](/LAMAudio2Expression-UE/licenses/)も確認してください。

## 配布版とBlueprint版の違い

:::note[v0.3.0にはBlueprintデモを同梱]
v0.3.0 ZIPはDemo `f3b6f13`を使用し、以下のBlueprint UIと解析処理を含みます。旧v0.2.0の紹介録画はBlueprint移行前です。最新公開Demo `9dee71d`ではプラグイン参照が更新されています。[リリースマニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json)で版を確認できます。
:::

Blueprintの入口は`BP_FaceDemo`の`02_Analyze_And_Play`です。`SelectSample` → 前の処理をキャンセル → `Analyze SoundWave Async` → `Completed` → `Play Expression Clip`という接続になっています。

| Blueprint | 役割 |
| --- | --- |
| BP_FaceDemo | 解析、再生、入力、状態イベント |
| BP_FaceDemoHUD | Canvasノードによる画面とクリック処理 |
| BP_FaceDemoGameMode | HUDを指定するGameMode |
| ABP_Face52 | Apply LAM ARKit Curvesで表情を適用 |

これらは`Content/LAMFaceDemo`以下にあります。別プロジェクトへ移す場合はプラグインを導入し、Content BrowserのMigrateを使います。素材の出典・ライセンスを保持してください。通常の自作キャラクターへの導入では、デモ素材をコピーする必要はありません。

## Viseme・事前解析のサンプル

v0.3.0には5母音・Oculusテンプレートのサンプルと事前解析Clipのサンプルも含まれます。通常起動の顔デモはARKit 52のままです。5母音では`/Game/LAMVisemeExamples/ABP_LAMVisemes`と`Fcl_MTH_A/I/U/E/O`の接続を確認してください。同フォルダーにOpenFaceFX用`ABP_OpenFaceFX`とTalkingHead用`ABP_TalkingHead`があります。テスト用フラグを指定したときだけメッシュ・AnimBPを切り替える構成で、通常HUDの追加キー操作ではありません。

[Viseme設定](/LAMAudio2Expression-UE/expression-curves/#visemes)、[保存済みClipのサンプル](/LAMAudio2Expression-UE/baked-clips/#examples-and-validation-commands)、[v0.3.0の検証記録](/LAMAudio2Expression-UE/validation/#release-validation)を参照してください。
