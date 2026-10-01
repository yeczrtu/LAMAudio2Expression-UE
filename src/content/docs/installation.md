---
title: "UE5リップシンクプラグインの導入方法"
description: "UE5にLAM Audio2Expressionを導入する手順。モデル入りZIPの配置、プラグイン有効化、Cook設定から最初のBlueprint再生まで。v0.3.0の配布対象はUE 5.8.2／Windows x64。"
sidebar: {"label":"プラグインの導入"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / README.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/README.md"}]
---

**Unreal Engine 5（UE5）**へLAM Audio2Expressionのリップシンクプラグインを導入する手順です。v0.3.0の配布対象はUE 5.8.2／Windows x64です。有効化後は[Blueprintのリップシンク接続](/LAMAudio2Expression-UE/blueprint/)へ進みます。動作だけを先に試す場合は[Windowsデモ](/LAMAudio2Expression-UE/demo/)を利用できます。

## 事前に用意するもの

- Windows x64と**Unreal Engine 5.8.2**。
- ARKit 52対応Morph Target、またはカーブ駆動リグ。5母音・Oculus互換モーフは[Viseme変換](/LAMAudio2Expression-UE/expression-curves/#visemes)を利用できます。
- UEへインポート済みの通常のSoundWave。

配布物にはEditor Development、Game Development / Shippingのビルド済みデータとC++ソースが含まれます。他のUE版は[ソースからの再ビルド](/LAMAudio2Expression-UE/development/)が必要です。

## モデル入りZIPを配置する

1. [v0.3.0のプラグインZIP](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Win64-Model.zip)をダウンロードします。
2. UEエディタを終了し、ZIPをすべて展開します。
3. `LAMAudio2Expression`フォルダーを`<Project>/Plugins/LAMAudio2Expression`に配置します。更新時は既存フォルダーを別の場所へ退避してから置き換えます。
4. UEで**LAM Audio2Expression**を有効にし、再起動します。依存するNNE ORTとAudioCaptureも有効になります。
5. Project Settings → LAM Audio2Expression → Modelが`/LAMAudio2Expression/Models/LAM_A2E`になっていることを確認します。

:::caution[配布ファイルを保持してください]
`Source`と`Intermediate/Build`にはGameビルド用データがあります。削除しないでください。GitHubの「Source code」ZIPはモデル入り配布物の代わりにはなりません。
:::

<!-- guide:plugin-enabled:start -->
<figure class="guide-figure" id="figure-plugin-enabled" data-guide="plugin-enabled">
<div class="guide-shot" style="--shot-ratio:675/169;--shot-width:138.81481%;--shot-left:-36.74074%;--shot-top:-32.54438%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/plugin-enabled.jpg" width="937" height="494" loading="lazy" decoding="async" alt="LAMで検索したPlugins画面。LAM Audio2Expressionが有効になっている。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:1.77778%;--y:5.91716%;--w:83.25926%;--h:16.56805%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:4%;--y:59.1716%;--w:3.40741%;--h:18.93491%"><b>2</b></span>
</div>
<figcaption>
<p><strong>PluginsでLAM Audio2Expressionを有効にする</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/plugin-enabled.jpg">原寸画像を開く</a></p>
<ol>
<li>検索欄にLAMを入力します。</li>
<li>チェックを有効にし、要求されたらUEを再起動します。</li>
</ol>
<p>画面のVersion 0.2.0は撮影した公開ソースの記述子の値です。v0.3.0配布ZIPでは0.3.0へ更新されています。撮影コミットは下記を参照してください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:plugin-enabled:end -->

<!-- guide:model-settings:start -->
<figure class="guide-figure" id="figure-model-settings" data-guide="model-settings">
<div class="guide-shot" style="--shot-ratio:700/195;--shot-width:134%;--shot-left:-31.14286%;--shot-top:-99.48718%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/model-settings.jpg" width="938" height="494" loading="lazy" decoding="async" alt="LAM Audio2ExpressionのModelにLAM_A2Eを指定し、Prefer GPUを有効、Cache MiBを64にした設定画面。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:57.85714%;--y:17.4359%;--w:40.14286%;--h:34.87179%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:58.42857%;--y:52.82051%;--w:18.85714%;--h:40.51282%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Project Settingsのモデル指定</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/model-settings.jpg">原寸画像を開く</a></p>
<ol>
<li>Modelで/LAMAudio2Expression/Models/LAM_A2Eを選択します。</li>
<li>撮影例はPrefer GPU有効、Cache MiB 64です。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:model-settings:end -->

## 最初の音声を再生する

Actorに`LAMAudio2ExpressionComponent`を追加します。`Analyze SoundWave Async`にコンポーネントとSoundWaveを渡し、`Completed`で得たClipを`Play Expression Clip`へ接続します。

AnimBPでは`Apply LAM ARKit Curves`を既存ポーズとOutput Poseの間に置き、同じコンポーネントをSource Componentに指定します。[Blueprint接続の詳細](/LAMAudio2Expression-UE/blueprint/)へ進んでください。

初回は約384 MiBのモデルのロードと推論初期化を待ちます。実行時のPython・外部推論サーバー・追加モデルダウンロードは不要です。

<!-- guide:add-component:start -->
<figure class="guide-figure" id="figure-add-component" data-guide="add-component">
<div class="guide-shot" style="--shot-ratio:329/157;--shot-width:622.4924%;--shot-left:0%;--shot-top:-64.33121%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/add-component.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="BlueprintのComponentsパネル。Addボタンと追加済みのLAMコンポーネント。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.34347%;--y:18.47134%;--w:21.8845%;--h:18.47134%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:8.20669%;--y:71.97452%;--w:86.32219%;--h:17.83439%"><b>2</b></span>
</div>
<figcaption>
<p><strong>ActorへLAMコンポーネントを追加する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/add-component.jpg">原寸画像を開く</a></p>
<ol>
<li>AddでLAM Audio2Expression Componentを検索して追加します。</li>
<li>追加したコンポーネントをLAMという名前で使用した例です。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:add-component:end -->

## パッケージ化する場合

Project Settings → Packaging → **Additional Asset Directories to Cook**へ`/LAMAudio2Expression/Models`を追加します。Editorでは動くのに配布版でモデルが見つからない場合は、まずこの設定を確認します。[モデルとパッケージ化](/LAMAudio2Expression-UE/models-and-packaging/)

顔・音声・マップはプラグイン単体には含まれません。接続済みのキャラクターで試したい場合は[デモプロジェクト](/LAMAudio2Expression-UE/demo/)を使用してください。

<!-- guide:cook-settings:start -->
<figure class="guide-figure" id="figure-cook-settings" data-guide="cook-settings">
<div class="guide-shot" style="--shot-ratio:702/68;--shot-width:133.61823%;--shot-left:-31.05413%;--shot-top:-438.23529%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/cook-settings.jpg" width="938" height="494" loading="lazy" decoding="async" alt="PackagingのAdditional Asset Directories to Cookに/LAMAudio2Expression/Modelsを登録した画面。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.5641%;--y:5.88235%;--w:93.73219%;--h:38.23529%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:57.69231%;--y:48.52941%;--w:30.76923%;--h:39.70588%"><b>2</b></span>
</div>
<figcaption>
<p><strong>モデルフォルダーをCook対象に含める</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/cook-settings.jpg">原寸画像を開く</a></p>
<ol>
<li>PackagingでAdditional Asset Directories to Cookを開きます。</li>
<li>モデルのフォルダー/LAMAudio2Expression/Modelsを追加します。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:cook-settings:end -->
