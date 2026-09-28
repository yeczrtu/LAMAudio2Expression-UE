---
title: "Unreal Engineで音声から表情をつくる"
description: "LAM Audio2Expressionは音声からARKit 52表情カーブを生成するUnreal Engineプラグイン。日本語デモ、導入、Blueprintによるリップシンクの手順を解説します。"
sidebar: {"label":"概要"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / README.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"},{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

<p class="lead">SoundWaveを解析し、声に合わせてキャラクターの口と表情を動かす。LAM Audio2Expressionは、PC内で推論するUnreal Engine用ランタイムプラグインです。</p>

<div class="signal-flow" aria-label="音声から表情への処理">
  <div><span>01 INPUT</span><strong>音声</strong><small>SoundWave / マイク / PCM</small></div>
  <div><span>02 INFERENCE</span><strong>PC内で推論</strong><small>DirectML / CPU</small></div>
  <div><span>03 EXPRESSION</span><strong>ARKit 52</strong><small>Blueprint / AnimGraph</small></div>
</div>

[プラグインを導入する](/LAMAudio2Expression-UE/installation/) · [Windowsデモを試す](/LAMAudio2Expression-UE/demo/) · [Blueprintの接続を見る](/LAMAudio2Expression-UE/blueprint/)

## ダウンロード

:::note[公開ソースの新機能：SoundWaveの事前解析]
収録済みの台詞をエディタで解析・保存し、既存のBlueprintノードで再生できます。[生成・先読み・再生成の手順](/LAMAudio2Expression-UE/baked-clips/)を追加しました。プラグイン1f06ac8以降のソースが必要で、下記v0.2.0のZIPには未収録です。
:::

用途から選びたい場合は、[音声リップシンク手法の比較](/LAMAudio2Expression-UE/lip-sync-comparison/)でLAM・Audio2Face・MetaHumanなどの出力、感情表現、導入条件を確認できます。

<div class="download-grid">
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Win64-Model.zip"><strong>プラグイン</strong><span>自分のUEプロジェクトへ<br/>モデル入りZIP · 約376 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-Win64-Demo.zip"><strong>Windowsデモ</strong><span>UEエディタなしで試す<br/>実行版ZIP · 約968 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.2.0/LAMAudio2Expression-0.2.0-UE5.8.2-Project-Model.zip"><strong>UEプロジェクト</strong><span>キャラクターと音声を編集<br/>モデル入りZIP · 約401 MiB</span></a>
</div>

すべて**v0.2.0**の配布物です。GitHubの自動生成する「Source code」ZIPにはモデルが含まれず、デモのプラグインも含まれません。チェックサムは[プラグインRelease](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/tag/v0.2.0)と[デモRelease](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/tag/v0.2.0)で確認できます。

## 日本語6音声の顔デモ

![日本語音声に合わせて表情を生成するv0.2.0顔デモの画面。公開済み録画の5秒時点。](/LAMAudio2Expression-UE/images/demo-preview.png)

<p class="media-credit">怒り、嫌悪、恐怖、喜び、悲しみ、驚きの6音声に合わせて表情を生成します。動画・音声・デモ演出：CC BY-SA 4.0。キャラクター：hinzka / VRoid。音声：JVNV / litagin。<a href="/LAMAudio2Expression-UE/licenses/">出典・利用条件</a></p>

動画の配信元は現在利用できません。[Windowsデモの起動手順](/LAMAudio2Expression-UE/demo/)から実際の動作を試せます。

## 対応環境とできること

| 項目 | 対応内容 |
| --- | --- |
| エンジン・OS | Unreal Engine 5.8.2 / Windows x64 |
| キャラクター | ARKit 52対応Morph Target、またはカーブで駆動するリグ |
| 事前解析 | 通常のSoundWave、mono / stereo、8〜192 kHz、最大5分 |
| 再生 | 音声と表情の同期、一時停止、再開、シーク、音量、Submix、2D / 3D |
| ライブ入力 | マイク・外部PCM。推論間隔は約33.3〜1000 ms |
| 推論 | DirectMLを優先し、利用できない場合はCPUへ切り替え |

実行時にPythonや外部推論サーバーは不要です。SoundCue、MetaSound、Procedural SoundWave、外部WAV / MP3の直接読み込みは対象外です。対応範囲と測定条件は[検証結果](/LAMAudio2Expression-UE/validation/)を参照してください。

## このドキュメントの対象

基本手順は公開済みv0.2.0を対象とします。デモの解析・UIをBlueprintで実装した版は公開ソース側の変更で、v0.2.0のZIPには含まれません。[デモ操作](/LAMAudio2Expression-UE/demo/)と[開発手順](/LAMAudio2Expression-UE/development/)では、この違いを明記しています。

サイトは参照元コミットを固定して編集しています。各ページ末尾から元資料を確認できます。
