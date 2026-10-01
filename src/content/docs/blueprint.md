---
title: "UE5のBlueprintでリップシンクを実装する"
description: "UE5のBlueprintで音声リップシンクを接続する方法。Analyze SoundWave Async、Play Expression Clip、AnimGraphのARKit表情カーブ適用と保存済みClipの再生を解説します。"
sidebar: {"label":"Blueprintの接続"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Demo / Docs/FACE_DEMO.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/FACE_DEMO.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

**Unreal Engine 5（UE5）**のBlueprintで、音声解析・再生・表情カーブ適用を分けてリップシンクを接続します。先に[プラグインを導入](/LAMAudio2Expression-UE/installation/)してください。ActorにはLAMAudio2ExpressionComponent、キャラクターメッシュにはAnimation Blueprintを用意し、[ARKitカーブまたはViseme](/LAMAudio2Expression-UE/expression-curves/)をメッシュに合わせます。

## SoundWaveを解析する

```text
BeginPlay または任意のイベント
  → Analyze SoundWave Async(Component, SoundWave, Settings)
      Completed(Clip) → Play Expression Clip(Clip, StartTime=0)
      Progress       → ロード表示を更新
      Failed         → Errorを表示
      Cancelled      → ロード表示を終了
```

通常のmono / stereo SoundWave、8〜192 kHz、最大300秒に対応します。外部WAV / MP3を直接渡すことはできません。UEへインポートしてSoundWaveとして使用します。

`Completed`のClip出力を再生ノードへ渡します。他のActorでも再利用する場合はClipをBlueprint変数で保持してください。解析中の中断は`Cancel Analysis`です。同じコンポーネントで新しい解析を開始すると、前の解析はキャンセルされます。

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

<!-- guide:blueprint-analysis:start -->
<figure class="guide-figure" id="figure-blueprint-analysis" data-guide="blueprint-analysis">
<div class="guide-shot" style="--shot-ratio:1051/318;--shot-width:194.86204%;--shot-left:-33.30162%;--shot-top:-88.99371%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/blueprint-analysis.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="BeginPlayからAnalyze SoundWave Asyncを呼び、CompletedとClipをPlay Expression Clipへ接続したBlueprint。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:32.35014%;--y:5.66038%;--w:28.92483%;--h:90.56604%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:68.03045%;--y:5.66038%;--w:30.63749%;--h:58.1761%"><b>2</b></span>
</div>
<figcaption>
<p><strong>非同期解析のCompletedから再生する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/blueprint-analysis.jpg">原寸画像を開く</a></p>
<ol>
<li>同じLAMコンポーネントとSoundWaveを解析ノードへ渡します。</li>
<li>Completedの実行線とClip出力を再生ノードへ接続します。</li>
</ol>
<p>接続の最小例です。製品ではFailed・Cancelled・Progressも処理してください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:blueprint-analysis:end -->

## AnimGraphへ表情を適用する

```text
既存のポーズ → Apply LAM ARKit Curves → Output Pose
```

1. キャラクターのAnimBPを開きます。
2. `Apply LAM ARKit Curves`を既存ポーズとOutput Poseの間に接続します。
3. **Source Component**に、解析・再生を担当するLAMコンポーネントを指定します。
4. **Alpha**を0〜1で調整します。カーブ名が異なる場合は[Curve Profile](/LAMAudio2Expression-UE/expression-curves/)を設定します。

Source Componentが空欄なら、SkeletalMeshを所有するActorから検索します。複数のLAMコンポーネントを持つActorでは明示的に指定してください。

標準名`jawOpen`などと同名のMorph Targetを持つメッシュで利用できます。ボーン駆動のリグでは出力カーブをControl Rigなどへ接続します。プラグインは独自のボーン配置を自動推定しません。

<!-- guide:animgraph:start -->
<figure class="guide-figure" id="figure-animgraph" data-guide="animgraph">
<div class="guide-shot" style="--shot-ratio:753/219;--shot-width:271.97875%;--shot-left:-73.70518%;--shot-top:-177.62557%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/animgraph.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Local Space Ref Pose、Apply LAM ARKit Curves、Output Poseを接続した実際のAnimGraph。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.39044%;--y:8.21918%;--w:24.9668%;--h:34.7032%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:36.78619%;--y:6.84932%;--w:28.15405%;--h:65.2968%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:68.6587%;--y:7.76256%;--w:29.08367%;--h:85.84475%"><b>3</b></span>
</div>
<figcaption>
<p><strong>既存ポーズへARKit表情カーブを適用する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/animgraph.jpg">原寸画像を開く</a></p>
<ol>
<li>このサンプルのRef Poseは説明用です。実際には既存のアニメーションポーズを入力します。</li>
<li>Source Componentに解析・再生用LAMを指定します。空欄なら所有Actorから検索されます。</li>
<li>出力をOutput Poseへ接続します。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:animgraph:end -->

## 再生と状態取得

`Pause`・`Resume`・`Stop`・`Seek`は音声と表情を一緒に制御します。一時停止では表情を保持し、停止では100 msで元のポーズへ戻ります。詳細は[再生制御](/LAMAudio2Expression-UE/playback/)を参照してください。

- `Get Current Expression Frame`：時刻、52値、Validity、適用Weight。
- `Get ARKit Curve Value`：名前を指定したカーブの推定値。停止フェードのWeightは別扱いです。

## 接続済みの例

配布プロジェクトの`/Game/Examples/BP_LAMPlayback`は非同期解析と再生の例、`/Game/Examples/ABP_LAMCurves`はAnimGraphの例です。後者はテスト用スケルトンで作られているため、自分のAnimBPへノード構成をコピーします。

公開ソースのBlueprint顔デモでは、`BP_FaceDemo`の`02_Analyze_And_Play`から接続を確認できます。v0.3.0 ZIPにこのデモ実装が含まれます。[デモのバージョン差](/LAMAudio2Expression-UE/demo/)

## 保存済みClipを使う（v0.3.0）

収録済み音声は[SoundWaveの事前解析](/LAMAudio2Expression-UE/baked-clips/)でClipアセットを生成・保存できます。ロード済みClipをそのまま再生ノードへ渡し、実行時の解析を省きます。v0.3.0の配布ZIPに含まれる機能です。

最新ソースPlugin `1860d0e`では**Lipsync**でもノードを検索できます。v0.3.0 ZIPでは**LAM**または表示名を使ってください。5母音・Oculusリグには[Apply LAM Viseme Curves](/LAMAudio2Expression-UE/expression-curves/#visemes)で口のカーブを適用します。
