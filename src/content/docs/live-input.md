---
title: "UE5でマイク・PCM入力のリップシンクを動かす"
description: "UE5でマイクや外部PCMからライブリップシンクを動かす手順。Blueprintの開始・停止、推論間隔、提示遅延、P95メトリクスと未検証範囲を解説します。"
sidebar: {"label":"マイク・ライブ入力"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / Docs/PLAYBACK_AND_LIVE.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/PLAYBACK_AND_LIVE.md"},{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"}]
---

**UE5**でマイクや外部PCMストリームからライブリップシンクを生成する手順です。SoundWave全体を事前解析する代わりに、Blueprintから入力の開始・停止と時間設定を制御します。ライブ入力と通常解析は同じ専用ワーカーを使用します。収録済みの台詞には[SoundWaveの事前解析](/LAMAudio2Expression-UE/baked-clips/)を利用できます。

## マイク入力

`Start Microphone(Settings, DeviceIndex=-1)`で既定の録音デバイスを使用します。Windowsのマイクアクセス許可が必要です。終了には`Stop Microphone`を呼びます。

キャプチャ音声のスピーカーへの折り返しは行いません。起動時にはモデルロードと初期化が必要です。あらかじめSoundWaveを解析してモデルアセットをロードしておくこともできます。

<!-- guide:microphone-blueprint:start -->
<figure class="guide-figure" id="figure-microphone-blueprint" data-guide="microphone-blueprint">
<div class="guide-shot" style="--shot-ratio:652/470;--shot-width:314.11043%;--shot-left:-84.66258%;--shot-top:-44.04255%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/microphone-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="MからStart Microphone、NからStop Microphoneを呼ぶBlueprint。Device Indexは-1。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:48.61963%;--y:2.97872%;--w:49.07975%;--h:38.93617%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:48.61963%;--y:66.59574%;--w:49.07975%;--h:25.53191%"><b>2</b></span>
</div>
<figcaption>
<p><strong>マイク開始と停止の接続例</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/microphone-blueprint.jpg">原寸画像を開く</a></p>
<ol>
<li>Start MicrophoneのDevice Index -1は既定デバイスです。</li>
<li>終了時は同じLAMにStop Microphoneを呼びます。</li>
</ol>
<p>接続だけの撮影例です。実マイクの録音・動作試験は行っていません。キーイベントを使う場合はActorの入力を有効にしてください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:microphone-blueprint:end -->

## 外部PCMを渡す

```text
Start PCM Stream(Settings)
  → ゲームスレッドから繰り返し
      Push PCM Audio(InterleavedPCM, SampleRate, Channels)
  → Stop Microphone
```

- float値域は−1〜1、mono / stereoのインターリーブ形式。
- サンプルレートは8〜192 kHz、1コール最大2秒。
- サンプルレートを変更する場合はストリームを再開始。
- まとめて大きく渡すより、小さいチャンクを定期的に渡します。

<!-- guide:pcm-blueprint:start -->
<figure class="guide-figure" id="figure-pcm-blueprint" data-guide="pcm-blueprint">
<div class="guide-shot" style="--shot-ratio:480/558;--shot-width:426.66667%;--shot-left:-132.91667%;--shot-top:-29.39068%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/pcm-blueprint.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Start PCMStream、OnPCMBlockReceivedカスタムイベントからPush PCMAudio、Stop Microphoneを接続したBlueprint。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:44.79167%;--y:1.6129%;--w:46.04167%;--h:18.99642%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.5%;--y:37.81362%;--w:94.79167%;--h:27.41935%"><b>2</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:44.79167%;--y:80.28674%;--w:46.04167%;--h:15.41219%"><b>3</b></span>
</div>
<figcaption>
<p><strong>外部PCMを小さなブロックで渡す</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/pcm-blueprint.jpg">原寸画像を開く</a></p>
<ol>
<li>先にStart PCMStreamを呼びます。</li>
<li>外部のPCM供給側からゲームスレッドでイベントを繰り返し呼びます。図は16,000 Hz・monoのfloat配列の例です。</li>
<li>ストリーム終了にもStop Microphoneを使います。</li>
</ol>
<p>OnPCMBlockReceivedは説明用のカスタムイベントで、自動では呼ばれません。実データに合わせてSample RateとChannelsを指定してください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:pcm-blueprint:end -->

## 更新間隔と提示遅延

`Set Live Inference Interval`の単位はmsです。約33.3〜1000 msの範囲で、内部では1〜30フレームに丸めます。既定は10フレーム、約333.3 msです。実行中の変更は次の推論ジョブから反映されます。`Get Live Inference Interval`で丸めた要求値を確認できます。

**Presentation Delay**は希望する最小遅延で、既定0.75秒、設定範囲0.4〜2秒です。実効値は要求下限と「処理間隔＋結果到着遅延P95＋1フレーム」の大きい方で、最大2秒です。実行中は増加方向にだけ調整し、表示時刻を巻き戻しません。縮小は次の開始時に再計算します。

間隔を短くすると更新頻度は上がりますが、実時間動作は機器性能と同時負荷に依存します。指定したmsで結果が届く保証ではありません。

## 実行状態を確認する

`Get Live Metrics`でActual Interval、Effective Presentation Delay、Inference P95、Result Latency P95、Initialization、Dropped Intervals、Backend、Stateを取得します。P95は直近60回で、準備中の0は未計測です。Result Latencyはワーカー待ちを含み、Initializationは分離されます。

`On Live State Changed`はPreparing / Running / Lagging / Failed / Stoppedを通知します。入力待ちキューは最大2秒、同時推論はセッションあたり1件です。処理が遅れると古い待ち仕事を破棄し、最新位置へ復帰します。結果のない区間では100 ms保持後に100 msで表情を戻します。

<!-- guide:live-metrics:start -->
<figure class="guide-figure" id="figure-live-metrics" data-guide="live-metrics">
<div class="guide-shot" style="--shot-ratio:1069/432;--shot-width:191.58092%;--shot-left:-32.27315%;--shot-top:-52.31481%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/live-metrics.jpg" width="2048" height="1104" loading="lazy" decoding="async" alt="Set Live Inference Intervalに333.333333 msを渡し、Get Live MetricsをBreak LAMLive Metricsへ接続したBlueprint。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:28.99906%;--y:3.24074%;--w:30.12161%;--h:39.58333%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:28.99906%;--y:63.42593%;--w:69.97194%;--h:28.24074%"><b>2</b></span>
</div>
<figcaption>
<p><strong>推論間隔の指定とメトリクスの取得</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/live-metrics.jpg">原寸画像を開く</a></p>
<ol>
<li>Millisecondsに希望する間隔を渡します。図の333.333333は設定値で、実測時間ではありません。</li>
<li>Get Live Metricsを分解してStateやActual Intervalを読みます。下向き矢印で他の項目を展開できます。</li>
</ol>
<p>取得先をUIやログへ接続すると値を評価できます。この図は値の出力先を省略した接続例です。Iキーを使う場合はActorの入力を有効にしてください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:live-metrics:end -->

## 検証済みの範囲

公開テストはPCM連続入力、実行中の間隔変更、CPU / DirectML、ワーカー競合を扱っています。**実マイクの取得・切断、長時間運転は未検証**です。機能テストのP95を運用保証として扱わず、対象環境で確認してください。[検証結果](/LAMAudio2Expression-UE/validation/)
