---
title: "ライセンス・クレジット"
description: "LAM Audio2Expressionのコード、学習済みモデル、JVNV音声、デモ映像、hinzka / VRoidキャラクターの出典と公開資料に記録された利用条件。"
sidebar: {"label":"ライセンス"}
appliesTo: "v0.2.0 · UE 5.8.2 / Windows x64"
sources: [{"label":"Plugin / THIRD_PARTY_NOTICES.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/THIRD_PARTY_NOTICES.md"},{"label":"Plugin / Docs/MODEL_MANAGEMENT.md · 3a04219","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Docs/MODEL_MANAGEMENT.md"},{"label":"Demo / Docs/VIDEO.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VIDEO.md"},{"label":"Demo / Resources/Demo/README.md · 275a683","url":"https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Resources/Demo/README.md"}]
---

このページはプロジェクトの公開済み帰属資料を整理したものです。コード・モデル・デモ素材は、それぞれのライセンスと表記に従います。

## 適用範囲

| 対象 | 公開資料での表記 |
| --- | --- |
| 独自のプラグイン統合コード | MIT |
| LAM由来コード・学習済みモデル | Apache-2.0 |
| 音声・音声と同期したデモ演出・紹介動画 | CC BY-SA 4.0 |
| キャラクターとテクスチャ | hinzkaの公開許諾およびVRoidの条件 |
| Unreal Engine | Epic Gamesの利用条件 |

[MIT本文](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/LICENSE) · [Apache-2.0本文](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/Licenses/Apache-2.0.txt) · [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

## 上流コードと学習済みモデル

[LAM Audio2Expression](https://github.com/aigc3d/LAM_Audio2Expression)と[モデルカードの固定版](https://huggingface.co/3DAIGC/LAM_audio2exp/blob/0fe5f4dbb283ec7d9c01688681e6e4b6ac314858/README.md)を参照しています。モデルをONNXへ変換し、Unreal EngineのNNEアセットとして利用しています。モデルをMITに変更するものではありません。

再配布時にはLICENSE、THIRD_PARTY_NOTICES、ライセンス本文、出典、変換時の変更説明を保持します。固定リビジョンとハッシュは各リポジトリのモデルマニフェストにあります。

## 音声と映像

原作品は**JVNV: A Corpus of Japanese Emotional Speech with Verbal Content and Nonverbal Expressions**。作者は**Detai Xin、Junfeng Jiang、Shinnosuke Takamichi、Yuki Saito、Akiko Aizawa、Hiroshi Saruwatari**です。[公式コーパス](https://sites.google.com/site/shinnosuketakamichi/research-topics/jvnv_corpus)

使用音声は**litagin**による[jvnv_corpus_v1_no_nv](https://huggingface.co/datasets/litagin/jvnv_corpus_v1_no_nv)の固定版`0ca4908ee5b9610bc3b739e1e67dff6120df0675`です。非言語区間と対応テキストを除いた先行改変を保持しています。

F1話者のanger_regular_31、disgust_regular_38、fear_regular_23、happy_regular_38、sad_regular_10、surprise_regular_11を使用しています。デモではSoundWaveへインポートし、Bink Audioで符号化して同期表情を生成しています。原作者による本プラグインの推奨を意味しません。

プレビュー画像は公開済み録画の5秒時点の切り出し、顔デモ画像は公開済みスクリーンショットを変更せず再掲しています。元動画の配信先が利用できないため、このサイトでは静止画を掲載しています。再利用時は帰属、ライセンスリンク、変更説明を保持してください。

## キャラクターとテクスチャ

**hinzka — [52blendshapes-for-VRoid-face](https://github.com/hinzka/52blendshapes-for-VRoid-face)**。VRoid Studio / pixivを基にしています。デモ側の出典資料は、2026年9月18日更新の[作者README固定版](https://github.com/hinzka/52blendshapes-for-VRoid-face/blob/756f5abab7d2295ad5b5dbc2cd86972c388c48d2/README.md)の利用・改変・再配布許諾を根拠として記録しています。

旧VRMメタデータには`Redistribution_Prohibited`が残っているため、デモの出典資料では新しい公開READMEの明示許諾との相違も記録しています。キャラクターをMITやCC0へ変更するものではありません。

デモではFBX変換済みFace52メッシュを移植し、テクスチャを割り当て、MToonからUEのマスク付きLitマテリアルへ変更しています。[VRoidの関連条件](https://vroid.pixiv.help/hc/ja/articles/4405813333657)とページ末尾の原文も確認してください。
