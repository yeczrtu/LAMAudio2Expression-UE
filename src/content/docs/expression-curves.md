---
title: "ARKit 52表情カーブ・Curve Profile・Viseme変換"
description: "Unreal EngineでARKit 52を適用し、v0.3.0の5母音・Oculus互換Visemeへ変換する設定。Curve Profile、TemplateFit、口の二重適用と制約を解説します。"
sidebar: {"label":"表情カーブ設定"}
appliesTo: "v0.3.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Demo / Docs/USAGE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/USAGE.md"},{"label":"Demo / Docs/ARCHITECTURE.md · 9dee71d","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/9dee71de2b60058e3434478f4f78b080cee9ca93/Docs/ARCHITECTURE.md"},{"label":"Plugin / Docs/VISEMES.md · 1860d0e","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md"}]
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
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
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
<p>設定後、ダイアログ下部のGenerateを押します。この生成ダイアログはv0.3.0に含まれます。</p>
<p class="guide-provenance">撮影 2026-09-30 · UE 5.8.2 · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE/tree/1860d0e2b28ea120a804361d3c9f5c19622f05e4">Plugin 1860d0e</a> · <a href="https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/tree/9dee71de2b60058e3434478f4f78b080cee9ca93">Demo 9dee71d</a> · v0.3.0に含まれる操作。</p>
</figcaption>
</figure>
<!-- guide:bake-settings:end -->

## 推論と見た目の関係

約2.13秒の固定窓を1秒ずつ進めて解析し、表情を30 fpsのカーブ列として出力します。後処理は時系列全体へ適用します。上流デモのランダムな出力をそのまま再現する仕組みではありません。

出力カーブをControl Rigやボーン駆動へつなぐことはできますが、任意のリグへの自動リターゲットは行いません。[Blueprint接続](/LAMAudio2Expression-UE/blueprint/)と[検証範囲](/LAMAudio2Expression-UE/validation/)を参照してください。

<span id="visemes" class="comparison-anchor" aria-hidden="true"></span>

## 5母音・Oculus互換Visemeへ変換する

**v0.3.0に含まれる機能です。** `Apply LAM Viseme Curves`は補間後のARKit 52フレームを5母音またはOculus互換15枠へ変換します。SoundWaveの動的解析・保存済みClip・ライブ入力で使え、追加モデル・Oculus SDK・再解析は不要です。口形状の変換であり、音素認識ではありません。[公開仕様・2026-09-30確認](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md)

1. ニュートラルな口形状と必要なMorph Targetを持つメッシュを用意します。任意キャラクターのモーフを自動生成する機能ではありません。
2. **Show Plugin Content**を有効にし、`/LAMAudio2Expression/Profiles/`のProfileをプロジェクトへ複製します。**Target Names**を実際のMorph Target名に合わせます。`None`は書き込みません。
3. AnimGraphで`元のポーズ → Apply LAM Viseme Curves → Output Pose`と接続します。**Profile**を指定し、ActorのLAMコンポーネントを**Source Component**へ渡します。空欄なら所有Actorから検索しますが、複数ある場合は明示してください。
4. 一時停止・シーク・停止・中立口形状と自分の音声で確認します。存在しないモーフ名へカーブを出してもメッシュは変形しません。

| Profile | 変換方式・対象モーフ |
| --- | --- |
| `DA_JapaneseFive` | FiveVowelRules。初期名は`A/I/U/E/O` |
| `DA_OculusReference` / `DA_OculusSDK` | 15枠の名前を持つFiveVowelRules。子音9枠は常に0 |
| `DA_OculusOpenFaceFX` | TemplateFit。OpenFaceFX配合、`ih/oh/ou`などのSDK名 |
| `DA_OculusTalkingHead` | TemplateFit。TalkingHead配合、`viseme_I/viseme_O/viseme_U`などの接頭辞付き名 |

出力名と変換方式は独立した設定です。5母音だけのメッシュは**FiveVowelRules**を使います。Profile未指定時は既定の5母音設定と`A/I/U/E/O`名を使用します。FiveVowelRulesは中立時の**Input Corrections** → **Activation** → **Width / Roundness / OpenSplit** → **Vowel Gains**の順に調整します。形状による近似で、モデル横断の発音精度を保証するものではありません。

### 目・眉と口を組み合わせる

目・眉もLAMで動かす場合はARKitノードに`DA_UpperFaceOnly`を指定します。

```text
元のポーズ
  → Apply LAM ARKit Curves (DA_UpperFaceOnly)
  → Apply LAM Viseme Curves (キャラクター用Profile)
  → Output Pose
```

UpperFaceOnlyは`jaw*`・`mouth*`・`tongueOut`の書き込みを無効化します。**入力ポーズにすでに存在する口カーブを消去する設定ではありません。** 全ARKitノードやSet Morph Targetで同じ口を同時制御しないでください。一時停止では保持、シークでは新しい時刻に追従し、停止ではコンポーネントの100 msフェードで入力ポーズへ戻ります。変換による追加の時間平滑化はありません。

### TemplateFitの設定と制約

**ConversionMode=TemplateFit**は、選んだ順方向の配合へ非中立14重みを当てはめます。各値は非負、合計は1以下です。**sil**は残余の`1 − 合計`であり、音声の無音検出ではありません。中立用モーフが不要ならTarget NameをNoneにします。

`tongueOut`など未供給の入力は**Input Corrections → FitWeight=0**にします。Scale=0では「観測した値が0」という意味になります。出力14枠の倍率は**Viseme Gains**で調整し、silの倍率は無視されます。Vowel Gainsと開口・横幅・丸みの閾値はFiveVowelRules専用です。変更後は**Validate Assets**でProfileを検証してください。

両テンプレートの行列はランク11で、14重みを常に一意に逆算できません。TalkingHeadのCHとRRは同一配合で、倍率適用前に均等配分します。笑顔などの表情も口形状へ影響します。TemplateFitは形状近似であり、Oculusの音声推論の再現や、発話した子音の確実な識別ではありません。[固定配合と詳細な制約](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/1860d0e2b28ea120a804361d3c9f5c19622f05e4/Docs/VISEMES.md)を参照してください。

### Blueprintで変換値を取得する

```text
Get Current Expression Frame
  → Convert ARKit To Visemes (Frame, Profile)
  → Get Vowel Weights / Get Viseme Weight
```

15枠の順序は`sil, PP, FF, TH, DD, kk, CH, SS, nn, RR, aa, E, ih, oh, ou`で固定です。独自適用では**bValid**を確認し、**Values**には未適用の**Weight**とAlphaを一度だけ掛けます。無効フレームはsilを含む15個の0です。外部ARKit入力は`Get ARKit Curve Names`の順に52個の有限値を設定し、有効な時刻・Weight等も渡してください。

[デモアセット](/LAMAudio2Expression-UE/demo/)と[配布版の検証](/LAMAudio2Expression-UE/validation/#release-validation)も参照してください。配合データとMIT表記はプラグインに同梱されますが、OpenFaceFX・TalkingHead・Blender・Oculusのランタイムは不要です。[クレジット](/LAMAudio2Expression-UE/licenses/#viseme-templates)
