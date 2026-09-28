---
title: "Unreal Engineプラグインの導入"
description: "UE 5.8.2にLAM Audio2Expression v0.2.0を導入する手順。モデル入りZIP、プラグイン有効化、Cook設定、最初のBlueprint接続を説明します。"
sidebar: {"label":"プラグインの導入"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Plugin / README.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"}]
---

自分のUnreal Engineプロジェクトへ音声リップシンクを追加する手順です。動作だけを試す場合は[Windowsデモ](/LAMAudio2Expression-UE/demo/)を利用できます。

## 事前に用意するもの

- Windows x64と**Unreal Engine 5.8.2**。
- ARKit 52対応のMorph Target、または表情カーブを利用できるリグ。
- UEへインポート済みの通常のSoundWave。

配布物にはEditor Development、Game Development / Shippingのビルド済みデータとC++ソースが含まれます。他のUE版は[ソースからの再ビルド](/LAMAudio2Expression-UE/development/)が必要です。

## モデル入りZIPを配置する

1. [v0.2.0のプラグインZIP](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip)をダウンロードします。
2. UEエディタを終了し、ZIPをすべて展開します。
3. `LAMAudio2Expression`フォルダーを`<Project>/Plugins/LAMAudio2Expression`に配置します。更新時は既存フォルダーを別の場所へ退避してから置き換えます。
4. UEで**LAM Audio2Expression**を有効にし、再起動します。依存するNNE ORTとAudioCaptureも有効になります。
5. Project Settings → LAM Audio2Expression → Modelが`/LAMAudio2Expression/Models/LAM_A2E`になっていることを確認します。

:::caution[配布ファイルを保持してください]
`Source`と`Intermediate/Build`にはGameビルド用データがあります。削除しないでください。GitHubの「Source code」ZIPはモデル入り配布物の代わりにはなりません。
:::

## 最初の音声を再生する

Actorに`LAMAudio2ExpressionComponent`を追加します。`Analyze SoundWave Async`にコンポーネントとSoundWaveを渡し、`Completed`で得たClipを`Play Expression Clip`へ接続します。

AnimBPでは`Apply LAM ARKit Curves`を既存ポーズとOutput Poseの間に置き、同じコンポーネントをSource Componentに指定します。[Blueprint接続の詳細](/LAMAudio2Expression-UE/blueprint/)へ進んでください。

初回は約384 MiBのモデルのロードと推論初期化を待ちます。実行時のPython・外部推論サーバー・追加モデルダウンロードは不要です。

## パッケージ化する場合

Project Settings → Packaging → **Additional Asset Directories to Cook**へ`/LAMAudio2Expression/Models`を追加します。Editorでは動くのに配布版でモデルが見つからない場合は、まずこの設定を確認します。[モデルとパッケージ化](/LAMAudio2Expression-UE/models-and-packaging/)

顔・音声・マップはプラグイン単体には含まれません。接続済みのキャラクターで試したい場合は[デモプロジェクト](/LAMAudio2Expression-UE/demo/)を使用してください。
