---
title: "モデル管理とWindowsパッケージ化"
description: "LAM Audio2Expressionの学習済みモデル、UEのCook設定、CPU・DirectML用データ、配布物のチェックサムとライセンス表記を整理します。"
sidebar: {"label":"モデルとパッケージ化"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/RELEASE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/RELEASE.md"}]
---

## モデルの配置

モデル入りZIPは、約384 MiBのニューラルモデルアセットを含みます。既定の参照は`/LAMAudio2Expression/Models/LAM_A2E`です。UEはNNEアセット内のONNXとCook済みCPU / DirectML用データを使います。

モデルを通常のGit履歴へ追加しない運用です。ソースから取得した場合は[セットアップ手順](/LAMAudio2Expression-UE/development/)で固定版チェックポイントを取得・変換するか、公開済みモデル入り配布物を使います。

## Cookのチェックリスト

1. Project Settings → LAM Audio2Expression → Modelが正しいことを確認。
2. Packaging → Additional Asset Directories to Cookに`/LAMAudio2Expression/Models`を追加。
3. デモを配布する場合は、`/Game/LAMFaceDemo/Maps/LAM_FaceDemo`をMaps to Cookへ追加。
4. プラグインの`Source`と`Intermediate/Build`を保持。
5. ビルド後、配布フォルダー全体を別の短いパスへコピー・展開して起動確認。

実行ファイルだけの配布ではなく、Cook済みデータ、必要なDLL、ライセンスを含むフォルダー全体を使用します。公開WindowsデモではVC++ランタイムのインストーラーも同梱されています。

## GPUとCPU

DirectML対応GPUを優先し、利用できない場合はCPUへフォールバックします。モデルロードや初期化の時間は、解析中表示などで利用者に伝えてください。GPUモデルを利用できない場合の代替経路は検証されていますが、実GPU故障の再現試験とは異なります。[検証結果](/LAMAudio2Expression-UE/validation/)

## 版とチェックサム

モデルの由来、固定リビジョン、形状、SHA-256は[モデルマニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/model-manifest.json)に記録されています。Releaseには`release-manifest.json`と`SHA256SUMS.txt`を添付します。

配布物を記録するときは、プラグイン版・UE版・モデル版・ハッシュをまとめて保持します。公開済みZIPを差し替えるより、新しいバージョンとして配布する運用です。

## 配布時の表記

独自コードのMITと、上流由来コード・モデルのApache-2.0は適用範囲が異なります。`LICENSE`、`THIRD_PARTY_NOTICES.md`、`Licenses`、モデルの出典と変換内容を保持してください。デモの音声・映像・キャラクターにも別の条件があります。[ライセンス・クレジット](/LAMAudio2Expression-UE/licenses/)
