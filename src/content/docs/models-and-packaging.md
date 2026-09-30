---
title: "モデル管理とWindowsパッケージ化"
description: "LAM Audio2Expressionの学習済みモデル、UEのCook設定、CPU・DirectML用データ、配布物のチェックサムとライセンス表記を整理します。"
sidebar: {"label":"モデルとパッケージ化"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/RELEASE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/RELEASE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

## モデルの配置

モデル入りZIPは、約384 MiBのニューラルモデルアセットを含みます。既定の参照は`/LAMAudio2Expression/Models/LAM_A2E`です。UEはNNEアセット内のONNXとCook済みCPU / DirectML用データを使います。

モデルを通常のGit履歴へ追加しない運用です。ソースから取得した場合は[セットアップ手順](/LAMAudio2Expression-UE/development/)で固定版チェックポイントを取得・変換するか、公開済みモデル入り配布物を使います。

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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.2.0と共通の操作。</p>
</figcaption>
</figure>
<!-- guide:model-settings:end -->

<!-- guide:plugin-content:start -->
<figure class="guide-figure" id="figure-plugin-content" data-guide="plugin-content">
<div class="guide-shot" style="--shot-ratio:222/155;--shot-width:100%;--shot-left:0%;--shot-top:-188.3871%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/plugin-content.jpg" width="222" height="746" loading="lazy" decoding="async" alt="Content BrowserのSettingsメニューでShow Plugin Contentが有効になっている。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.6036%;--y:70.32258%;--w:91.89189%;--h:18.70968%"><b>1</b></span>
</div>
<figcaption>
<p><strong>Content Browserにプラグインのアセットを表示する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/plugin-content.jpg">原寸画像を開く</a></p>
<ol>
<li>Content Browser右上のSettingsからShow Plugin Contentを有効にします。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.2.0と共通の操作。</p>
</figcaption>
</figure>
<!-- guide:plugin-content:end -->

## Cookのチェックリスト

1. Project Settings → LAM Audio2Expression → Modelが正しいことを確認。
2. Packaging → Additional Asset Directories to Cookに`/LAMAudio2Expression/Models`を追加。
3. デモを配布する場合は、`/Game/LAMFaceDemo/Maps/LAM_FaceDemo`をMaps to Cookへ追加。
4. プラグインの`Source`と`Intermediate/Build`を保持。
5. ビルド後、配布フォルダー全体を別の短いパスへコピー・展開して起動確認。

実行ファイルだけの配布ではなく、Cook済みデータ、必要なDLL、ライセンスを含むフォルダー全体を使用します。公開WindowsデモではVC++ランタイムのインストーラーも同梱されています。

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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.2.0と共通の操作。</p>
</figcaption>
</figure>
<!-- guide:cook-settings:end -->

## GPUとCPU

DirectML対応GPUを優先し、利用できない場合はCPUへフォールバックします。モデルロードや初期化の時間は、解析中表示などで利用者に伝えてください。GPUモデルを利用できない場合の代替経路は検証されていますが、実GPU故障の再現試験とは異なります。[検証結果](/LAMAudio2Expression-UE/validation/)

## 版とチェックサム

モデルの由来、固定リビジョン、形状、SHA-256は[モデルマニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/model-manifest.json)に記録されています。Releaseには`release-manifest.json`と`SHA256SUMS.txt`を添付します。

配布物を記録するときは、プラグイン版・UE版・モデル版・ハッシュをまとめて保持します。公開済みZIPを差し替えるより、新しいバージョンとして配布する運用です。

## 配布時の表記

独自コードのMITと、上流由来コード・モデルのApache-2.0は適用範囲が異なります。`LICENSE`、`THIRD_PARTY_NOTICES.md`、`Licenses`、モデルの出典と変換内容を保持してください。デモの音声・映像・キャラクターにも別の条件があります。[ライセンス・クレジット](/LAMAudio2Expression-UE/licenses/)

## 保存済みClipをCookする（公開ソース版）

プラグイン1f06ac8の[事前解析Clip](/LAMAudio2Expression-UE/baked-clips/)は、生成後に保存し、元SoundWaveとともにCook対象へ含めます。Soft参照のみの場合はAsset Managerなどで対象を明示してください。Clip再生経路ではモデルをロードしませんが、既存のモデル同梱設定は変更しません。v0.2.0のZIPには未収録です。
