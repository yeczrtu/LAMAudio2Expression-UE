---
title: "導入・リップシンクのトラブル対処"
description: "モデルが見つからない、表情が動かない、音声が読み込めない、ライブ入力が遅れる場合のLAM Audio2Expression確認手順。"
sidebar: {"label":"トラブル対処"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/RELEASE_INSTALL.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/RELEASE_INSTALL.md"},{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/README.md"},{"label":"Plugin / Docs/BAKED_CLIPS.md · 1f06ac8","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1f06ac858413090f00c3f5bb955e1d73653ef74e/Docs/BAKED_CLIPS.md"}]
---

## モデルが見つからない

GitHubの「Source code」ではなく、**モデル入りRelease ZIP**を取得したか確認します。Project SettingsのModelは`/LAMAudio2Expression/Models/LAM_A2E`です。

Editorで動いて配布版で失敗する場合は、PackagingのAdditional Asset Directories to Cookへ`/LAMAudio2Expression/Models`を追加します。[導入手順](/LAMAudio2Expression-UE/installation/)

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

## 音声は鳴るが顔が動かない

1. キャラクターのAnimBPに`Apply LAM ARKit Curves`が接続されているか確認します。
2. Source Componentが解析・再生を担当するコンポーネントを参照しているか確認します。
3. Alphaが0ではなく、Curve Profileで必要なカーブを無効にしていないか確認します。
4. メッシュが`jawOpen`など対応するMorph Targetを持つか、カーブをリグへ渡しているか確認します。

ボーン駆動の独自リグへの自動変換はありません。[表情カーブ設定](/LAMAudio2Expression-UE/expression-curves/)を参照してください。

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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.2.0と共通の操作。</p>
</figcaption>
</figure>
<!-- guide:animgraph:end -->

## 解析が拒否される・音声を渡せない

対象は通常のmono / stereo SoundWave、8〜192 kHz、最大300秒です。SoundCue、MetaSound、Procedural SoundWave、外部WAV / MP3の直接入力は対象外です。通常再生ではループ指定も拒否されます。

非同期解析の`Failed`と、再生の`On Playback Failed`でErrorを記録します。新しい解析を同じコンポーネントで開始すると、以前の解析はキャンセルされます。[Blueprint接続](/LAMAudio2Expression-UE/blueprint/)

## 初回だけ時間がかかる

モデルロードとNNEインスタンス初期化があります。`Progress`をロード表示へつなぎ、`Completed`の後に再生します。初回と、初期化後の固定窓の時間を混同しないでください。[検証結果](/LAMAudio2Expression-UE/validation/)

## マイクやライブ入力が動かない・遅れる

Windowsのマイク権限、録音デバイス、`On Live State Changed`を確認します。`Get Live Metrics`で初期化、結果到着P95、破棄区間数を確認し、同じワーカーへの大量の通常解析を避けます。PCMはゲームスレッドから小さいチャンクで渡します。

マイク音声はスピーカーへ折り返しません。音が聞こえないことだけでは取得失敗と判断できません。[ライブ入力](/LAMAudio2Expression-UE/live-input/)

## 自然終了イベントで次の台詞へ進めたい

`On Playback Finished`を使用します。`On Playback Ended`はStopや差し替えでも発火するため、次の台詞を無条件に開始しないでください。イベント引数のPlayback Idで対象を判断します。[再生制御](/LAMAudio2Expression-UE/playback/)

## 問題を報告する

[プラグインのIssue](https://github.com/yeczrtu/LAMAudio2Expression-UE/issues)か[デモのIssue](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/issues)へ、使用版、UE版、Editor / Shippingなどの構成、CPU / GPU、再現手順、Error.Code / Error.Messageを添えてください。配布版かソース版かも記載すると切り分けやすくなります。

## 事前解析Clipの問題（公開ソース版）

生成メニューが表示されない場合は、v0.2.0 ZIPではなくプラグイン1f06ac8を含むソースでEditorをビルドしたか確認します。`InvalidBakedClip`、生成後の保存、再生成、Cook漏れは[事前解析Clipのトラブル対処](/LAMAudio2Expression-UE/baked-clips/#troubleshooting)を参照してください。

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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · 公開ソース版の画面・v0.2.0 ZIPには未収録。</p>
</figcaption>
</figure>
<!-- guide:bake-regenerate:end -->
