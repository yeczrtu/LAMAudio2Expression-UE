---
title: "UE5の音声リップシンクプラグイン"
description: "UE5で音声に合わせてキャラクターの口と表情を動かすLAM Audio2Expression。Blueprintによる導入、SoundWave事前解析、マイク入力、ARKit 52・Visemeの設定を解説。配布・検証対象はUE 5.8.2／Windows x64。"
sidebar: {"label":"概要"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / README.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/README.md"},{"label":"Demo / README.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/README.md"},{"label":"Demo / Docs/VIDEO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/VIDEO.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

<p class="lead">LAM Audio2Expressionは、Unreal Engine 5（UE5）で音声から口の動きと表情を生成するリップシンクプラグインです。PC内で解析したARKit 52カーブやVisemeの重みを、BlueprintとAnimGraphでキャラクターへ適用します。</p>

<div class="signal-flow" aria-label="音声から表情への処理">
  <div><span>01 INPUT</span><strong>音声</strong><small>SoundWave / マイク / PCM</small></div>
  <div><span>02 INFERENCE</span><strong>PC内で推論</strong><small>DirectML / CPU</small></div>
  <div><span>03 EXPRESSION</span><strong>ARKit 52</strong><small>Blueprint / AnimGraph</small></div>
</div>

[プラグインを導入する](/LAMAudio2Expression-UE/installation/) · [Windowsデモを試す](/LAMAudio2Expression-UE/demo/) · [Blueprintの接続を見る](/LAMAudio2Expression-UE/blueprint/)

<span id="getting-started" class="comparison-anchor" aria-hidden="true"></span>

## UE5でリップシンクを始める

そのまま使える配布版は**UE 5.8.2／Windows x64**向けです。UEへインポート済みのSoundWaveと、ARKit 52・5母音・Oculus互換のMorph Target、または表情カーブで動くリグを用意します。実行時にPythonや外部推論サービスは不要です。他のUE版は再ビルドが必要で、公開済みの検証対象には含まれません。

1. [モデル入りプラグインを導入](/LAMAudio2Expression-UE/installation/)し、プロジェクトで有効にします。動作を先に見たい場合は[Windowsデモ](/LAMAudio2Expression-UE/demo/)を利用できます。
2. [Blueprintで音声解析と再生を接続](/LAMAudio2Expression-UE/blueprint/)します。解析完了時に得られるClipで音声と表情を同期再生します。
3. [ARKit表情カーブまたはVisemeを設定](/LAMAudio2Expression-UE/expression-curves/)し、AnimGraphで適用します。出力先の名前や設定をキャラクターのリグに合わせてください。

| やりたいこと | 次に読むガイド |
| --- | --- |
| 収録済みの台詞を繰り返し使う | [SoundWaveの事前解析](/LAMAudio2Expression-UE/baked-clips/)で保存し、先読みして実行時の推論なしで再生 |
| マイク・外部PCMで口を動かす | [ライブ入力の設定](/LAMAudio2Expression-UE/live-input/)で更新間隔・提示遅延・検証範囲を確認 |
| 一時停止・シーク・音量・3D音声を制御する | [再生制御とイベント](/LAMAudio2Expression-UE/playback/) |
| 使用する音声駆動方式から選ぶ | [UE5リップシンク手法の比較](/LAMAudio2Expression-UE/lip-sync-comparison/)で出力・感情制御・導入条件を確認 |

## ダウンロード

:::note[v0.3.0を公開中]
モデル入りZIPに[SoundWaveの事前解析・保存](/LAMAudio2Expression-UE/baked-clips/)、[5母音・Oculus互換Viseme変換](/LAMAudio2Expression-UE/expression-curves/#visemes)、Blueprint版の顔デモが含まれます。収録済みの台詞はエディタで一度解析し、保存済みClipを実行時の推論なしで再生できます。
:::

用途から選びたい場合は、[音声リップシンク手法の比較](/LAMAudio2Expression-UE/lip-sync-comparison/)でLAM・Audio2Face・MetaHumanなどの出力、感情表現、導入条件を確認できます。

<div class="download-grid">
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Win64-Model.zip"><strong>プラグイン</strong><span>自分のUEプロジェクトへ<br/>モデル入りZIP · 約391 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-Win64-Demo.zip"><strong>Windowsデモ</strong><span>UEエディタなしで試す<br/>実行版ZIP · 約971 MiB</span></a>
  <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/download/v0.3.0/LAMAudio2Expression-0.3.0-UE5.8.2-Project-Model.zip"><strong>UEプロジェクト</strong><span>キャラクターと音声を編集<br/>モデル入りZIP · 約427 MiB</span></a>
</div>

すべて**v0.3.0**の配布物です。GitHubの自動生成する「Source code」ZIPにはモデルが含まれず、デモのプラグインも含まれません。チェックサムは[プラグインRelease](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/tag/v0.3.0)と[デモRelease](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/tag/v0.3.0)で確認できます。

## 日本語6音声の顔デモ

![日本語音声に合わせて表情を生成するv0.2.0顔デモの画面。公開済み録画の5秒時点。](/LAMAudio2Expression-UE/images/demo-preview.png)

<p class="media-credit">怒り、嫌悪、恐怖、喜び、悲しみ、驚きの6音声に合わせて表情を生成します。動画・音声・デモ演出：CC BY-SA 4.0。キャラクター：hinzka / VRoid。音声：JVNV / litagin。<a href="/LAMAudio2Expression-UE/licenses/">出典・利用条件</a></p>

動画の配信元は現在利用できません。[Windowsデモの起動手順](/LAMAudio2Expression-UE/demo/)から実際の動作を試せます。

## 対応環境とできること

| 項目 | 対応内容 |
| --- | --- |
| エンジン・OS | Unreal Engine 5.8.2 / Windows x64 |
| キャラクター | ARKit 52、5母音・Oculus互換Morph Target、またはカーブ駆動リグ |
| 事前解析 | 通常のSoundWave、mono / stereo、8〜192 kHz、最大5分 |
| 再生 | 音声と表情の同期、一時停止、再開、シーク、音量、Submix、2D / 3D |
| ライブ入力 | マイク・外部PCM。推論間隔は約33.3〜1000 ms |
| 推論 | DirectMLを優先し、利用できない場合はCPUへ切り替え |

実行時にPythonや外部推論サーバーは不要です。SoundCue、MetaSound、Procedural SoundWave、外部WAV / MP3の直接読み込みは対象外です。対応範囲と測定条件は[検証結果](/LAMAudio2Expression-UE/validation/)を参照してください。

## このドキュメントの対象

**2026年9月30日確認**。基本手順は**v0.3.0**と公開ソースPlugin `1860d0e` / Demo `9dee71d`を対象とします。配布ZIPのビルド元はPlugin `1f06ac8` / Demo `f3b6f13`で、[リリースマニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json)に記録されています。事前解析Clip・Viseme変換・Blueprintデモはいずれも配布版に含まれます。

後続ソースではBlueprint・アニメーションノードに**Lipsync**検索キーワードが追加されました。v0.3.0 ZIPでは**LAM**またはノード名で検索してください。このメタデータ変更による実行時処理の変更はありません。ソースの記述子や既存画像には0.2.0と表示される場合がありますが、配布ZIPの記述子は0.3.0です。版の特定にはマニフェストとコミットを使用します。

上のプレビューは旧v0.2.0の録画で、v0.3.0の新規撮影ではありません。各ページ末尾から参照元を確認できます。
