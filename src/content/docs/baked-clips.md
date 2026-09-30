---
title: "SoundWaveを事前解析して表情Clipを保存する"
description: "Unreal EngineのSoundWaveからARKit 52カーブを事前生成・保存し、Blueprintで解析待ちなしに再生する手順。再生成、先読み、Cook、検証範囲も解説します。"
sidebar: {"label":"SoundWaveの事前解析"}
appliesTo: "v0.3.0 · UE 5.8.2 / Win64"
sourceSummary: "2026-09-30にv0.3.0の配布情報を確認。事前解析はPlugin 1f06ac8 / Demo f3b6f13をビルドしたv0.3.0に含まれます。配布前のソース検証記録は区別して記載しています。"
sources:
  - label: "Plugin / Docs/BAKED_CLIPS.md · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"
  - label: "Plugin / LAMBakedExpressionClip.cpp · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2Expression/Private/LAMBakedExpressionClip.cpp"
  - label: "Plugin / LAMBakeSubsystem.h · 1f06ac8"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2ExpressionEditor/Public/LAMBakeSubsystem.h"
  - label: "Demo / Docs/DEVELOPMENT.md · f3b6f13"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/DEVELOPMENT.md"
  - label: "Demo / baked-clips-results.json · f3b6f13"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/f3b6f13e98d75d6e23933669adee79319ef3363f/Docs/Validation/baked-clips-results.json"
---

収録済みの台詞は、エディタで一度解析し、ARKit 52表情カーブをアセットとして保存できます。ゲームでは保存済みClipを既存の`Play Expression Clip`へ渡すため、再生時のモデルロード・解析用デコード・推論を省けます。

:::note[v0.3.0の配布版に含まれます]
[モデル入りv0.3.0プラグイン](/LAMAudio2Expression-UE/installation/)またはUEプロジェクトを導入し、プラグインを有効にしてUE 5.8.2を再起動してください。配布対象の構成ではソースビルドは不要です。ZIPのビルド元は**Plugin 1f06ac8 / Demo f3b6f13**です。[リリースマニフェスト](https://github.com/yeczrtu/LAMAudio2Expression-UE/releases/download/v0.3.0/release-manifest.json)
:::

<span id="choose-a-workflow-and-check-support" class="comparison-anchor" aria-hidden="true"></span>

## 使い分けと対応範囲

| 用途 | 選ぶ経路 |
| --- | --- |
| あらかじめ決まった台詞を繰り返し再生 | 本ページの保存済みClip |
| ゲーム中にSoundWaveを解析 | 従来の[Analyze SoundWave Async](/LAMAudio2Expression-UE/blueprint/) |
| マイク・外部PCMを逐次処理 | [ライブ入力](/LAMAudio2Expression-UE/live-input/) |

初版の対応環境は**UE 5.8.2 / Win64**です。通常のSoundWave、mono / stereo、8〜192 kHz、最大300秒、非ループ・1倍速を対象とします。外部WAV / MP3は先にUEへインポートしてください。従来の動的解析とライブ入力は引き続き利用できます。

保存されるのはfloat32のARKit 52カーブ（30 fps）です。カーブ配列だけで**約366 KiB/分**となり、音声とメタデータの容量は別です。Visemeは保存しません。公開ソースのViseme変換を利用する場合、方式・テンプレート・入力補正・強度を再生時に変更できますが、その変換処理のCPU負荷は残ります。

<span id="generate-clips-in-the-editor" class="comparison-anchor" aria-hidden="true"></span>

## エディタでClipを生成する

1. Content Browserで通常の**SoundWave**を選択します。複数選択も可能です。
2. 右クリックして**Generate LAM Expression Clip**を選びます。
3. **Style / Smooth / Suppress Silent Mouth / Symmetrize / Auto Blink / Blink Seed**を設定します。
4. **Generate**を押し、完了を待ちます。失敗理由は画面とOutput Logに表示されます。
5. **Save All**で生成アセットをディスクに保存します。

Styleは0〜11のスタイル番号で、12種類の感情ラベルではありません。各設定の意図は[表情カーブ設定](/LAMAudio2Expression-UE/expression-curves/)を参照してください。生成の完了だけではディスクへ保存されません。

元音声と同じフォルダーに`<音声名>_LAMClip`が生成されます。同じ音声に対応する既存の出力Clipは更新され、別種類・別音声の同名アセットがある場合は`_1`、`_2`のように連番が付きます。**設定違いを残す場合は、先に既存Clipを複製またはリネーム**してください。

バッチは1音声ずつ処理し、失敗した項目を記録して次へ進みます。PIE中は生成できません。**Cancel**またはウィンドウを閉じる操作で残りの処理を中止します。実行中の推論呼び出しは完了まで待ちますが、その結果は反映しません。完了済みClipと、更新に失敗したClipの旧データは保持されます。

<!-- guide:bake-generate-menu:start -->
<figure class="guide-figure" id="figure-bake-generate-menu" data-guide="bake-generate-menu">
<div class="guide-shot" style="--shot-ratio:340/84;--shot-width:458.52941%;--shot-left:-116.17647%;--shot-top:-194.04762%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-generate-menu.jpg" width="1559" height="971" loading="lazy" decoding="async" alt="SoundWaveを右クリックしたメニューのGenerate LAM Expression Clip。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:5.58824%;--y:41.66667%;--w:91.47059%;--h:38.09524%"><b>1</b></span>
</div>
<figcaption>
<p><strong>SoundWaveの右クリックメニューから生成を開始する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-generate-menu.jpg">原寸画像を開く</a></p>
<ol>
<li>Content BrowserでSoundWaveを選択し、この項目を開きます。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-generate-menu:end -->

<!-- guide:bake-settings:start -->
<figure class="guide-figure" id="figure-bake-settings" data-guide="bake-settings">
<div class="guide-shot" style="--shot-ratio:356/170;--shot-width:147.75281%;--shot-left:-2.24719%;--shot-top:-51.17647%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg" width="526" height="580" loading="lazy" decoding="async" alt="Style 0、SmoothとSuppress Silent Mouthを有効、SymmetrizeとAuto Blinkを無効、Blink Seed 1234にした生成設定。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:2.94118%;--w:93.53933%;--h:13.52941%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:17.64706%;--w:93.53933%;--h:46.47059%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:3.37079%;--y:64.70588%;--w:93.53933%;--h:31.17647%"><b>3</b></span>
</div>
<figcaption>
<p><strong>Clipに保存する解析設定</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-settings.jpg">原寸画像を開く</a></p>
<ol>
<li>Styleは0〜11の話者スタイル番号です。感情名ではありません。</li>
<li>SmoothとSuppress Silent Mouthは有効、Symmetrizeは無効の例です。</li>
<li>Auto Blinkは無効。Blink Seed 1234は自動瞬き用のシードです。</li>
</ol>
<p>設定後、ダイアログ下部のGenerateを押します。この生成ダイアログはv0.3.0に含まれます。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-settings:end -->

<!-- guide:bake-complete:start -->
<figure class="guide-figure" id="figure-bake-complete" data-guide="bake-complete">
<div class="guide-shot" style="--shot-ratio:512/76;--shot-width:102.73438%;--shot-left:-1.36719%;--shot-top:-351.31579%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-complete.jpg" width="526" height="580" loading="lazy" decoding="async" alt="進捗バーとFinished. 1 clips, 0 failures. Save generated assets to keep them.の実際の完了表示。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:1.75781%;--y:48.68421%;--w:95.70313%;--h:40.78947%"><b>1</b></span>
</div>
<figcaption>
<p><strong>生成完了を確認する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-complete.jpg">原寸画像を開く</a></p>
<ol>
<li>この撮影では1件成功、失敗0件でした。完了後も保存が必要です。</li>
</ol>
<p>1音声を生成した操作例です。新しい性能測定や網羅的な検証結果ではありません。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-complete:end -->

<!-- guide:bake-save:start -->
<figure class="guide-figure" id="figure-bake-save" data-guide="bake-save">
<div class="guide-shot" style="--shot-ratio:646/532;--shot-width:100%;--shot-left:0%;--shot-top:0%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-save.jpg" width="646" height="532" loading="lazy" decoding="async" alt="Save Contentダイアログでspeech_stream_LAMClipが選択され、Save Selectedボタンが表示されている。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.47678%;--y:18.04511%;--w:95.51084%;--h:5.45113%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:64.08669%;--y:91.54135%;--w:20.27864%;--h:5.45113%"><b>2</b></span>
</div>
<figcaption>
<p><strong>生成したClipをディスクへ保存する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-save.jpg">原寸画像を開く</a></p>
<ol>
<li>Save Allを押し、生成されたspeech_stream_LAMClipが選択されていることを確認します。</li>
<li>Save Selectedで保存を確定します。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-save:end -->

<span id="preload-and-play-in-blueprint" class="comparison-anchor" aria-hidden="true"></span>

## Blueprintで先読みして再生する

生成アセットは`ULAMExpressionClip`を継承する`ULAMBakedExpressionClip`です。既存のClip入力ピンに直接指定できます。再生の前に`Analyze SoundWave Async`を挟む必要はありません。

```text
ロード画面 / BeginPlay
  → 保存済みClipと元のSoundWaveをロード
  → Prime Sound（元のSoundWave）

後の再生イベント
  → Play Expression Clip（保存済みClip）

AnimGraph
  既存のポーズ → Apply LAM ARKit Curves → Output Pose
```

ActorのLAMコンポーネントとAnimBPの**Source Component**を接続してください。[Blueprintの接続](/LAMAudio2Expression-UE/blueprint/)と[Curve Profile](/LAMAudio2Expression-UE/expression-curves/)は従来と共通です。Visemeを使う構成では対応する`Apply LAM Viseme Curves`を使用します。

Clipのハード参照は元SoundWaveもロード対象にします。**Soft Object Reference**で管理する場合は`Async Load Asset`の完了を待ち、ロードしたClipを変数などで保持します。ストリーミング音声には`Prime Sound`を先行して呼び、必要に応じて音声の**Retain On Load**設定を使います。

:::caution[先読みと解析待ちの違い]
`Prime Sound`は非同期の先読み要求であり、読込完了通知ではありません。再生直前よりもロード画面などで早めに呼びます。**解析待ちゼロ**とは、ロード済みClipから再生するときに解析完了を待たないことです。音声デコード、ストレージ、音声出力バッファ、描画周期による遅延は残ります。
:::

`Play Expression Clip With Settings`、Pause / Resume / Seek / Stop、音量、フェード、2D / 3D、Submix、再生イベントは[既存の再生制御](/LAMAudio2Expression-UE/playback/)と共通です。保存済みカーブは参照を共有し、各フレームで52値を補間します。再生ごとの全配列コピーは行いません。

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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:baked-blueprint:end -->

<span id="regenerate-after-changing-audio-or-models" class="comparison-anchor" aria-hidden="true"></span>

## 音声やモデルを変更したら再生成する

Clipを右クリックして**Regenerate**を選ぶと、そのClipに記録された解析設定で再生成します。設定を変えたい場合は、元音声の**Generate LAM Expression Clip**を使います。

Clipには音声の更新GUID、入力PCMのSHA-1、モデルGUID、モデルのパス、処理・形式バージョンを記録します。音声の再インポート、加工・圧縮設定の変更、モデル更新後は、UE標準の**Validate Assets**で確認して再生成・保存してください。エディタの検証ではモデルをロードする場合があります。再生時に自動で再生成される仕組みではありません。

解析中に音声やモデルが変更されると、その結果は破棄されます。ロード時には形式バージョン、音声参照、長さ、フレーム数、カーブ数、値の妥当性も検証します。破損・未対応形式の場合、`On Playback Failed`の`Error.Code`は**InvalidBakedClip**となり、エラー理由も通知されます。自動で動的解析へ切り替えることはありません。

<!-- guide:bake-regenerate:start -->
<figure class="guide-figure" id="figure-bake-regenerate" data-guide="bake-regenerate">
<div class="guide-shot" style="--shot-ratio:468/241;--shot-width:333.11966%;--shot-left:-51.28205%;--shot-top:-139.41909%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/bake-regenerate.jpg" width="1559" height="971" loading="lazy" decoding="async" alt="Content Browserのspeech_stream_LAMClipと、その右クリックメニューのRegenerate。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:35.89744%;--y:39.41909%;--w:59.40171%;--h:12.44813%"><b>1</b></span>
</div>
<figcaption>
<p><strong>保存済みClipを再生成する</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/bake-regenerate.jpg">原寸画像を開く</a></p>
<ol>
<li>Clipを右クリックしてRegenerateを選び、記録済み設定で再生成します。</li>
</ol>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-regenerate:end -->

<span id="cook-and-package" class="comparison-anchor" aria-hidden="true"></span>

## Cookとパッケージ化

1. 生成したClipと元SoundWaveを保存します。
2. BlueprintなどからClipを参照し、Cook対象に含めます。
3. Soft参照だけで管理する場合は、Asset Managerまたは**Additional Asset Directories to Cook**で対象を明示します。
4. CookしたDevelopment / Shippingのアプリで再生を確認します。

**モデルの同梱設定は変更されません。** 保存済みClipの再生経路ではモデルをロードしませんが、この機能はモデル除外による配布サイズ削減を提供するものではありません。生成にはモデルが必要で、動的解析・ライブ入力でも使用します。[モデルとパッケージ化](/LAMAudio2Expression-UE/models-and-packaging/)の既存設定を維持してください。

<span id="batch-processing-with-editor-utilities" class="comparison-anchor" aria-hidden="true"></span>

## Editor Utilityで一括処理する

Editor Utility Blueprint / Pythonからエディタサブシステム`LAMBakeSubsystem`を取得します。

| API | 用途 |
| --- | --- |
| `GenerateClips(Sounds, Settings)` | SoundWave群を指定した設定で生成 |
| `RegenerateClips(Clips)` | 保存された設定で再生成 |
| `IsBusy` / `GetProgress` / `GetStatus` | 処理状況を確認 |
| `GeneratedClips` / `Errors` | 完了したClipとエラーを取得 |
| `Cancel` | 未完了の処理を中止 |

開始関数の`true`は要求を受け付けたことを示し、生成成功や保存完了を意味しません。処理中・PIE中・対象なしでは開始できません。エディタのTickを継続させ、タイマーや非同期の通知処理から状態を確認します。**Pythonの同期ループで完了を待つと処理が進みません。** 完了後は`Errors`を確認し、生成アセットを明示的に保存してください。[API定義](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Source/LAMAudio2ExpressionEditor/Public/LAMBakeSubsystem.h)

<span id="examples-and-validation-commands" class="comparison-anchor" aria-hidden="true"></span>

## サンプルと検証手順

デモf3b6f13のEditorをビルドした後、デモのプロジェクトルートで実行します。テストは`/Game/Audio/*_LAMClip`、`/Game/Examples/BP_LAMBakedPlayback`、検証マップを生成・保存するため、テスト用チェックアウトを使用してください。

```powershell
./Tools/test_baked_clips.ps1 -Engine D:\Unreal\UE_5.8
./Tools/package.ps1 -Configuration Development -IncludeBaked
./Tools/package.ps1 -Configuration Shipping -IncludeBaked
./Tools/test_playback_controls.ps1 -Configuration Development -IncludeBaked
./Tools/test_playback_controls.ps1 -Configuration Shipping -IncludeBaked
```

生成された`BP_LAMBakedPlayback`は、レベル配置後にBeginPlayで先読みし、**Space**で再生する接続例です。`Tools/build_baked_examples.py`は生成済み`/Game/Audio/speech_stream_LAMClip`を使用し、既存の同名Blueprintを上書きしません。キャラクターメッシュとAnimBPは自分のものを接続してください。既存の動的解析例も引き続き利用できます。

2026-09-28の公開記録では自動テスト12件とEditor / Development / Shippingの再生テスト計18件が成功しています。数値比較・性能値・未検証範囲は[事前解析Clipの検証結果](/LAMAudio2Expression-UE/validation/#baked-clips)を参照してください。本ページ作成時にUEテストを再実行した結果ではありません。

<span id="troubleshooting" class="comparison-anchor" aria-hidden="true"></span>

## 困ったとき

| 症状 | 確認すること |
| --- | --- |
| 生成メニューがない | v0.3.0の導入・有効化・Editor再起動、選択対象がSoundWaveであることを確認 |
| 生成できない | PIEを終了し、他のバッチ完了を待つ。モデル設定とOutput Logを確認 |
| 再起動するとClipがない | 生成後にSave Allを実行したか確認 |
| 音声を変更しても表情が変わらない | Validate Assets → Regenerate → Save All。設定を変えるなら元音声から生成 |
| 再生開始に待ちがある | Clipロード、元音声の先読み、ストレージと音声バッファを確認。解析待ちと区別する |
| `InvalidBakedClip` | エラー理由を確認し、対応版で再生成・保存・再Cookする |
| パッケージ版だけ失敗する | ClipとSoundWaveのCook対象、Soft参照のロード完了を確認 |

一般的な音声・表情・モデルの問題は[トラブル対処](/LAMAudio2Expression-UE/troubleshooting/)も参照してください。
