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

## 推論と見た目の関係

約2.13秒の固定窓を1秒ずつ進めて解析し、表情を30 fpsのカーブ列として出力します。後処理は時系列全体へ適用します。上流デモのランダムな出力をそのまま再現する仕組みではありません。

出力カーブをControl Rigやボーン駆動へつなぐことはできますが、任意のリグへの自動リターゲットは行いません。[Blueprint接続](/LAMAudio2Expression-UE/blueprint/)と[検証範囲](/LAMAudio2Expression-UE/validation/)を参照してください。
