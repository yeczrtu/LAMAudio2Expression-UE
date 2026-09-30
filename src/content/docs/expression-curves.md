---
title: "ARKit 52表情カーブとCurve Profile"
description: "Unreal EngineでARKit 52表情カーブをキャラクターへ適用する設定。Curve Profileによる名前変換、倍率、マスク、平滑化と無音抑制を解説します。"
sidebar: {"label":"表情カーブ設定"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/USAGE.md"},{"label":"Demo / Docs/ARCHITECTURE.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/ARCHITECTURE.md"}]
---

LAM Audio2Expressionは標準順の52カーブを出力します。キャラクターのMorph Target名と一致させるか、既存リグでカーブ値を利用します。

## キャラクターへ合わせる

`Apply LAM ARKit Curves`の**Alpha**は0〜1で、入力ポーズに対する表情の強さを調整します。自分のメッシュで口が動かない場合は、まず`jawOpen`の名前とMorph Targetの存在を確認してください。

**LAMCurveProfile** Data Assetを作成し、ノードのCurve Profileへ指定すると、カーブごとに次を設定できます。

| 設定 | 用途 |
| --- | --- |
| 名前変換 | キャラクター固有のMorph Target名へ対応付ける |
| 無効化 | 適用したくないカーブを除外する |
| 倍率 | 表情の動きの大きさを調整する |
| オフセット | 推定値に一定量を加える |

未指定のカーブは標準名のまま有効です。補正後の値は0〜1に制限されます。

```text
最終値 = lerp(入力カーブ,
              clamp(推定値 * 倍率 + オフセット, 0, 1),
              Alpha * Weight)
```

無効にしたカーブとボーン姿勢は変更しません。Weightには停止時のフェードなどが反映されます。停止後は100 msで入力ポーズへ戻り、一時停止では現在の表情を保持します。

<!-- guide:curve-profile:start -->
<figure class="guide-figure" id="figure-curve-profile" data-guide="curve-profile">
<div class="guide-shot" style="--shot-ratio:975/199;--shot-width:210.05128%;--shot-left:-1.53846%;--shot-top:-95.47739%">
<div class="guide-window">
<img src="/LAMAudio2Expression-UE/images/guides/curve-profile.jpg" width="2048" height="1111" loading="lazy" decoding="async" alt="Curve ProfileのSource NameとTarget NameはjawOpen、Enabledは有効、Scaleは0.8、Offsetは0.0。" />
</div>
<span class="guide-callout" aria-hidden="true" style="--x:2.97436%;--y:28.1407%;--w:94.35897%;--h:27.13568%"><b>1</b></span>
<span class="guide-callout" aria-hidden="true" style="--x:2.97436%;--y:56.28141%;--w:94.35897%;--h:38.69347%"><b>2</b></span>
</div>
<figcaption>
<p><strong>Curve ProfileでjawOpenを調整する例</strong> · <a class="guide-original" href="/LAMAudio2Expression-UE/images/guides/curve-profile.jpg">原寸画像を開く</a></p>
<ol>
<li>Source NameはLAMのカーブ名、Target Nameはキャラクター側の名前です。</li>
<li>この説明用ルールではEnabledを有効にし、Scale 0.8、Offset 0.0にしています。</li>
</ol>
<p>0.8は撮影用の調整例で、すべてのキャラクターに共通の推奨値ではありません。作成したProfileをAnimGraphノードへ指定してください。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.2.0と共通の操作。</p>
</figcaption>
</figure>
<!-- guide:curve-profile:end -->

## 解析設定

| 設定 | 既定値と意味 |
| --- | --- |
| Style | 0。上流モデルの話者スタイル番号、0〜11 |
| Smooth | 有効。5フレームのSavitzky–Golay平滑化と境界補間 |
| Suppress Silent Mouth | 有効。RMS 0.001未満が7フレーム続く区間で口の動きを抑制 |
| Symmetrize | 無効。左右対称化 |
| Auto Blink | 無効。音声からの推定ではなく、追加の瞬き演出 |
| Blink Seed | 自動瞬きの再現に使用する乱数シード |

Styleは上流学習モデルの番号であり、喜怒哀楽の指定ではありません。まず既定設定で接続を確認し、キャラクターに合わせてProfileの倍率やマスクを調整します。

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
<p>設定後、ダイアログ下部のGenerateを押します。解析設定はv0.2.0にもありますが、この生成ダイアログは公開ソース版の機能です。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · 公開ソース版の画面・v0.2.0 ZIPには未収録。</p>
</figcaption>
</figure>
<!-- guide:bake-settings:end -->

## 推論と見た目の関係

約2.13秒の固定窓を1秒ずつ進めて解析し、表情を30 fpsのカーブ列として出力します。後処理は時系列全体へ適用します。上流デモのランダムな出力をそのまま再現する仕組みではありません。

出力カーブをControl Rigやボーン駆動へつなぐことはできますが、任意のリグへの自動リターゲットは行いません。[Blueprint接続](/LAMAudio2Expression-UE/blueprint/)と[検証範囲](/LAMAudio2Expression-UE/validation/)を参照してください。
