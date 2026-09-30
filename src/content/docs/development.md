---
title: "ソースからのビルドとリリース手順"
description: "LAM Audio2Expressionの公開ソースを固定コミットで取得し、UE 5.8.2向けモデル生成、テスト、WindowsパッケージとRelease ZIPを作成する手順。"
sidebar: {"label":"開発・リリース"}
appliesTo: "公開ソースのスナップショット · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/DEVELOPMENT.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/DEVELOPMENT.md"},{"label":"Demo / Docs/RELEASE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/RELEASE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

このページは**公開ソースのスナップショット**を対象とします。すぐに使う場合は[モデル入りv0.2.0](/LAMAudio2Expression-UE/installation/)を利用してください。Blueprint版の顔デモはv0.2.0 ZIPより新しい実装です。

## 開発環境

UE 5.8.2、Visual Studio 2022のC++開発環境、Python 3.10、Gitを用意します。以下の例ではUEを`D:\Unreal\UE_5.8`へ配置しています。実際の場所に置き換えてください。

## ドキュメントと同じ版を取得する

```powershell
git clone --recurse-submodules https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo.git
cd LAMAudio2Expression-UE-Demo
git checkout 275a683a530254451efae8409e6ec2d2f57af6bb
git submodule update --init --recursive
./Tools/setup.ps1 -Engine D:\Unreal\UE_5.8
```

このcheckoutは参照版を再現するためのものです。変更を保存する場合は作業用ブランチを作成してください。既存cloneを更新する場合も、サブモジュールの初期化が必要です。

setupは専用の`.work/venv`を使用し、固定リビジョンのモデル取得、SHA-256照合、ONNX変換、数値比較、UEモデルアセット・テスト用アセット生成を行います。Python、PyTorch、ネットワークは開発時に必要ですが、配布アプリの推論には不要です。

:::caution[生成対象]
setupは`/Game/LAMDemo`と`/Game/Audio`のテストデータを再生成します。編集している場合は先にバックアップしてください。顔デモ自体は再生成しません。
:::

## ビルドとテスト

プロジェクトのルートで実行します。

```powershell
./Tools/test.ps1 -Engine D:\Unreal\UE_5.8
./Tools/package.ps1 -Configuration Development
./Tools/package.ps1 -Configuration Shipping
./Tools/smoke.ps1 -Configuration Development
./Tools/smoke.ps1 -Configuration Shipping
./Tools/test_playback_controls.ps1 -Configuration Shipping
./Tools/test_face_demo.ps1 -Configuration Shipping -OutputDirectory Artifacts/BPChecks-Shipping
```

再生制御と顔デモのテストはEditor / Development / Shippingそれぞれで実行できます。Shippingの実行ファイルは`Artifacts/Shipping/Windows/LAMDemo.exe`です。配布にはWindowsフォルダー全体を使います。

## Release ZIPの作成

1. モデルを生成し、`Docs/model-manifest.json`とハッシュを照合します。
2. `RunUAT.bat BuildPlugin`でプラグインをWin64向けにビルドします。出力先は新規の短いパスを指定します。このコマンドは出力先を空にします。
3. デモの配布用コピーを別ディレクトリに作り、`Tools/prepare_release_examples.py`で配布用接続例を生成します。検証済みモデルをコピーし、BuildCookRunでShippingを作成します。
4. 次のコマンドで配布物をまとめます。山括弧部分は実際のパスに置き換えます。

```powershell
python Tools/assemble_release.py --plugin <BuildPlugin-output> --project <disposable-project-directory> --shipping <Shipping-archive>/Windows --output <new-output-directory>
```

新規出力先に、プラグイン・編集用プロジェクト・Windows実行版のZIP、マニフェスト、チェックサムを作成します。各ZIPを別の短いパスへ展開してモデル・顔デモ・再生制御を検証してから、両リポジトリの同じバージョンタグへ公開します。詳細な配布用コピーの条件はページ末尾のRelease原文を参照してください。

## 事前解析Clipの開発・検証（公開ソース版）

プラグイン1f06ac8 / デモf3b6f13に追加された機能です。Editorをビルドしてから、専用テストで生成・再生成・保存・別プロセス再生を確認します。Development / Shippingにも検証マップを含める`-IncludeBaked`が追加されています。[コマンドと生成対象](/LAMAudio2Expression-UE/baked-clips/#examples-and-validation-commands)を確認してください。v0.2.0の配布ZIPには含まれません。

<!-- guide:baked-blueprint:start -->
<figure class="guide-figure" id="figure-baked-blueprint" data-guide="baked-blueprint">
<div class="guide-shot" style="--shot-ratio:674/516;--shot-width:303.85757%;--shot-left:-79.97033%;--shot-top:-36.24031%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/baked-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="BeginPlayでspeech_streamをPrime Soundへ渡し、Space Barから保存済みClipをPlay Expression Clipで再生するBlueprint。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:49.85163%;--y:2.90698%;--w:36.20178%;--h:24.03101%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:49.70326%;--y:48.83721%;--w:48.07122%;--h:40.89147%"><b>2</b></span>
</div>
<figcaption>
<p><strong>音声を先読みして保存済みClipを再生する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/baked-blueprint.jpg">原寸画像を開く</a></p>
<ol>
<li>Prime Soundには元のSoundWaveを指定します。先読みは早めに要求します。</li>
<li>Play Expression ClipのClipには保存済みのspeech_stream_LAMClipを指定します。</li>
</ol>
<p>ノードのアセット名は欄幅によって省略されています。Prime Soundは読込完了通知ではありません。キー入力を使うActorには入力の受け取り設定が必要です。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · 公開ソース版の画面・v0.2.0 ZIPには未収録。</p>
</figcaption>
</figure>
<!-- guide:baked-blueprint:end -->
