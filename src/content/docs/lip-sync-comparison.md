---
title: "音声リップシンク手法の比較：LAM・Audio2Face・MetaHuman"
description: "Unreal Engineの音声リップシンクをLAM、Audio2Face、MetaHuman Animator、OVRLipsync、SG Comで比較。ARKit 52、感情制御、ライブ入力、Unity・Web向け候補の違いを解説します。"
sidebar: {"label":"リップシンク手法の比較"}
appliesTo: "2026年9月28日確認 · 公開仕様による比較"
sourceSummary: "本文の公式資料・公式リポジトリ・原論文を2026年9月28日に確認しました。LAMはサイトで固定した公開ソースとv0.2.0を対象とします。他製品との実機比較は行っていません。"
sources:
  - label: "LAM Plugin / README · 3a04219"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md"
  - label: "LAM Demo / 検証記録 · 275a683"
    url: "https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/275a683a530254451efae8409e6ec2d2f57af6bb/Docs/VALIDATION.md"
  - label: "NVIDIA Audio2Face-3D / 公式製品・モデル一覧"
    url: "https://github.com/NVIDIA/Audio2Face-3D"
  - label: "Epic Games / Audio Driven Animation"
    url: "https://dev.epicgames.com/documentation/metahuman/audio-driven-animation"
  - label: "Meta / Oculus Lipsync Guide"
    url: "https://developers.meta.com/horizon/documentation/unreal/audio-ovrlipsync/"
  - label: "Speech Graphics / SG Com 5.0"
    url: "https://docs.speech-graphics.com/en/sg-com/5.0/what-is-sg-com"
---

音声からキャラクターを動かす方式は、口形状だけを推定するものから、感情を含む顔全体の動きを生成するものまであります。**使うキャラクター、必要な演技、実行環境**から候補を絞ると、導入後の作り直しを減らせます。

この記事は**2026年9月28日**に確認した公開資料による選定ガイドです。以下の推奨は仕様からの判断で、同条件の実機比較や品質ランキングではありません。各リンク先の対象版と、LAMの配布版・公開ソース版を区別しています。

<span id="choose-by-use-case" class="comparison-anchor" aria-hidden="true"></span>

## 用途から選ぶ

<div class="comparison-table" role="region" aria-label="用途別のリップシンク候補" tabindex="0">

| 作りたいもの・条件 | 最初に比較する候補 | 選定のポイント |
| --- | --- | --- |
| 汎用ARKitキャラクターをUE内で動かす | [LAM](#lam-audio2expression) | CPU／DirectMLでローカル実行。Blueprintから接続 |
| 音声に加えて感情を指定する会話キャラクター | [Audio2Face-3D](#audio2face-3d)、[SG Com](#sg-com) | 感情の入力方法、リグ調整、GPU・CPUの要件を確認 |
| MetaHumanの収録済み台詞 | [MetaHuman Animator](#metahuman-animator)のオフライン音声処理 | 生成後の演技調整とSequencerでの制作 |
| 軽量な口の動き、Unity・VRM | uLipSync、SALSA | 顔全体の生成が必要か、音素の区別が必要かを先に決める |
| Webアバター、音声合成との連携 | TalkingHead／HeadAudio、Azure Speech Viseme | 音声解析とTTSの時刻情報利用を分けて検討 |
| 顔画像・完成動画の口元を変更する | Wav2Lip、MuseTalk | 出力は映像。UEの表情カーブを直接生成する方式とは異なる |

</div>

Unity・Web・制作向け候補は[その他の候補](#その他の候補)、動画生成は[研究モデルと動画生成](#研究モデルと動画生成)で説明します。

<span id="before-comparing" class="comparison-anchor" aria-hidden="true"></span>

## 比較する前に

- **ARKit 52とViseme**：ARKit係数は顔の変形を表し、Visemeは発音に対応する口形状です。係数の数だけでは、閉口の正確さや自然さを比較できません。
- **口と顔全体**：リップシンクが良くても、眉・視線・頭部の演技が付くとは限りません。自動瞬きと音声から推定した表情も区別します。
- **事前解析とライブ入力**：完成した音声全体を利用できる処理と、届いた音声から順次生成する処理では条件が違います。
- **処理時間と体感遅延**：モデル推論だけでなく、入力待ち、バッファ、描画、音声出力まで含めて確認します。「リアルタイム」は無遅延の意味ではありません。

<span id="main-methods-compared" class="comparison-anchor" aria-hidden="true"></span>

## 主要手法の比較

表は横にスクロールできます。製品名から詳しい説明と出典へ移動できます。

<span id="output-and-emotion" class="comparison-anchor" aria-hidden="true"></span>

### 出力と感情表現

<div class="comparison-table" role="region" aria-label="主要手法の出力と感情表現" tabindex="0">

| 手法 | 出力・表現範囲 | 感情・演技 | 入力・処理 |
| --- | --- | --- | --- |
| [LAM](#lam-audio2expression) | ARKit 52、30 fps。口・目・眉のカーブ | 12話者スタイル。感情の専用入力なし | SoundWave事前解析、マイク／PCMライブ |
| [Audio2Face-3D](#audio2face-3d) | 顔全体。UE連携ではARKit互換カーブ。出力形式は構成による | Audio2Emotion連携、感情入力による制御 | 録音・ストリーム。SDK／サービス構成による |
| [MetaHuman：オフライン](#metahuman-animator) | MetaHumanの顔、頭部、瞬き | 感情自動検出・上書き・強度調整 | SoundWaveからアニメーションを生成 |
| [MetaHuman：リアルタイム音声](#metahuman-animator) | Live Linkによる顔駆動 | オフライン版とは調整項目が異なる。音声ソルバーは頭部動作を生成しない | 音声Live Linkソース |
| [OVRLipsync](#ovrlipsync) | 無音・母音・子音を含む15 Viseme | 笑い検出。顔全体の感情演技は別途 | マイク／ファイル。ライブ・事前計算 |
| [SG Com](#sg-com) | 口、顔全体、瞬き、視線、頭部 | 音声駆動の感情表現 | 音声ストリームを処理・同期再生 |

</div>

<span id="runtime-and-integration" class="comparison-anchor" aria-hidden="true"></span>

### 実行環境と導入条件

<div class="comparison-table" role="region" aria-label="主要手法の実行環境と導入条件" tabindex="0">

| 手法 | 実行環境・配置 | 導入・利用条件 | 保守・対応範囲 |
| --- | --- | --- | --- |
| [LAM UE版](#lam-audio2expression) | UE 5.8.2、Windows x64。CPU／DirectML、ローカル | 独自統合MIT、上流由来コード・モデルApache-2.0 | v0.2.0と固定公開ソース。実マイク長時間運転は未検証 |
| [Audio2Face-3D](#audio2face-3d) | SDKはWindows／Linux、CUDA・TensorRT。ローカル／クラウド構成 | SDKはMIT、顔モデルとAudio2Emotionは別条件。リグへの適合が必要 | SDK・モデル・UEプラグインの対応版を個別確認 |
| [MetaHuman Animator](#metahuman-animator) | Unreal EngineとMetaHuman。UE内の処理／Live Link | MetaHumanの組み立てとリグを使用。Epicの利用条件 | オフライン音声はUE 5.6以降。使用UE版の要件を確認 |
| [OVRLipsync](#ovrlipsync) | UE／Unityプラグイン、CPUによる音声解析 | 15 VisemeへのマッピングとSDKの利用条件を確認 | サポート終了段階。QuestではMovement SDKへの移行案あり |
| [SG Com](#sg-com) | CPUのSDK。Windows／Linuxなど。UE連携あり | 商用ライセンス、キャラクター設定。認証方式で通信要件が異なる | 本記事は5.0資料を参照。対象OS・SDK版を確認 |

</div>

### LAM Audio2Expression

このUE版は音声を16 kHzへ変換し、ARKit 52カーブを30 fpsで生成します。**12種類のStyleは話者スタイルで、12種類の感情ではありません。** 喜びや怒りを直接指定する入力はありません。実行時にPythonや外部推論サーバーを必要とせず、汎用ARKitリグに接続したい場合の候補です。[公開仕様](https://github.com/yeczrtu/LAMAudio2Expression-UE/blob/3a042193a98f54400dccda3d2cc8a8adc4d70815/README.md)

顔への適合にはカーブ名、倍率、マスクの調整が必要です。任意のリグを自動で変換する機能ではありません。[導入](/LAMAudio2Expression-UE/installation/) → [Blueprint接続](/LAMAudio2Expression-UE/blueprint/) → [表情カーブ設定](/LAMAudio2Expression-UE/expression-curves/)の順で確認できます。

既定のライブ推論間隔は約333.3 ms、希望する提示遅延は0.75秒です。固定窓の高速な推論値を、そのまま会話の応答遅延として扱えません。[ライブ設定と制約](/LAMAudio2Expression-UE/live-input/)

### Audio2Face-3D

感情を含む音声駆動表情を比較するなら、旧Omniverseアプリに加えて、現在のSDK・モデル・UE連携も対象になります。顔モデルへの感情入力とAudio2Emotionとの連携を備え、出力のリグへの接続方法は構成に依存します。[公式構成一覧](https://github.com/NVIDIA/Audio2Face-3D)、[顔モデルの入出力](https://huggingface.co/nvidia/Audio2Face-3D-v3.0)

ACE UEプラグイン2.5のアニメーションノードはARKit互換カーブを追加します。対象の顔には対応するポーズやカーブ設定が必要です。[UEでのキャラクター設定](https://docs.nvidia.com/ace/ace-unreal-plugin/2.5/ace-unreal-plugin-animation.html)

公開SDKはCUDA／TensorRTとNVIDIA GPUを使用します。LAMのCPU／DirectML経路と導入条件を揃えて比較する必要があります。SDKはMIT、顔モデルv3.0はNVIDIA Open Model Licenseです。**Audio2Emotion v2.2にはAudio2Face内での利用制限があり、LAM用の独立した感情推定器として扱えません。** [SDK要件](https://github.com/NVIDIA/Audio2Face-3D-SDK)、[Audio2Emotionの条件](https://huggingface.co/nvidia/Audio2Emotion-v2.2)

### MetaHuman Animator

収録済み音声の制作では、感情の自動検出・指定・強度、頭部動作、瞬きを調整してアニメーションを出力できます。汎用のARKit 52カーブを直接受け取る構成とは違い、MetaHumanのフェイシャルリグを使います。[オフライン音声処理](https://dev.epicgames.com/documentation/metahuman/audio-driven-animation)

ライブ音声はMetaHuman Audio Live Link Sourceから駆動します。Realtime Audio Solverは頭部動作を生成せず、オフライン版の演技調整をすべて同じように使えるわけではありません。また、PerformanceアセットでRealtime Audio Solverを選ぶだけでは、入力がライブストリームになるわけではありません。[Live Linkでの駆動](https://dev.epicgames.com/documentation/metahuman/realtime-animation-using-live-link)、[ソルバーの違い](https://dev.epicgames.com/documentation/metahuman/audio-driven-animation)

### OVRLipsync

15 Visemeと笑い検出を利用する、口中心の方式です。音声から子音・母音の口形状を解析するため、少数の口形状を扱うアバターでも比較対象になります。

**2026年4月17日更新の公式案内で、今後の更新・サポートが終了するとされています。** MetaはQuest向けにMovement SDKの音声ベース顔トラッキングを案内していますが、既存のWindows・Unity・UE用途すべての直接的な置き換えを意味しません。[Meta公式仕様・終了案内](https://developers.meta.com/horizon/documentation/unreal/audio-ovrlipsync/)

### SG Com

CPUで音声ストリームから顔全体・頭部・視線を生成する商用候補です。SDKにUE連携があり、キャラクター設定とライセンスの導入が必要です。5.0資料の**50 msはメーカー公称の入力から出力までの処理遅延**で、本サイトのLAM測定と同条件の値ではありません。[SG Com 5.0仕様](https://docs.speech-graphics.com/en/sg-com/5.0/what-is-sg-com)、[CPU使用量](https://docs.speech-graphics.com/en/sg-com/5.0/sg-com-compute-resource-usage)

SDKの対象OSは[プラットフォーム一覧](https://docs.speech-graphics.com/en/sg-com/5.0/sg-com-platform-support)で確認できます。音声処理の配置とライセンス認証を分けて検討してください。クラウド認証方式にはネット接続が必要なため、完全オフライン運用は契約・認証方式まで確認します。

<span id="other-candidates" class="comparison-anchor" aria-hidden="true"></span>

## その他の候補

以下も2026年9月28日に確認した公式資料に基づきます。名称のリンクが各行の出典です。

<span id="unity-2d-and-dialogue-production" class="comparison-anchor" aria-hidden="true"></span>

### Unity・2D・台詞制作

<div class="comparison-table" role="region" aria-label="Unityと制作向け候補" tabindex="0">

| 候補 | 方式と用途 | 導入前に確認すること |
| --- | --- | --- |
| [uLipSync](https://github.com/hecomi/uLipSync) | UnityのMFCC特徴による口形状推定。ライブ・事前解析・VRM対応 | 声に合わせたプロファイル調整。MIT。5母音を中心とした構成に使えるが、母音専用ではない |
| [SALSA LipSync Suite](https://crazyminnowstudio.com/docs/salsa-lip-sync/modules/recommendations/) | Unity向け。音量の変化をもとに口形状を駆動 | 音素を識別する方式ではない。商用製品。様式化した口の動きと導入の手軽さを重視する用途 |
| [Rhubarb Lip Sync](https://github.com/DanielSWolf/rhubarb-lip-sync/blob/master/README.adoc) | 音声ファイルから6〜9種類の口形状と時刻を生成。2D制作向け | 非英語向けphonetic方式あり。事前処理であり、顔全体のライブ生成とは用途が異なる |
| [FaceFX](https://www.facefx.com/) | 台詞からアニメーションを生成し、音素・カーブを編集 | 商用製品。大量の収録済み台詞を生成・調整する制作工程向け |
| [iClone AccuLIPS](https://www.reallusion.com/iclone/lipsync-animation.html) | 音声と台本の整列、前後の音による口形変化、単語単位の編集 | 商用制作ツール。辞書は英語中心。非英語の手順もあるが、日本語の品質は別途評価 |
| [SGX](https://docs.speech-graphics.com/en/sgx/4.4/what-is-sgx) | 顔全体・頭部などの生成、バッチ処理と編集 | 商用製品。本記事は4.4資料を参照。キャラクター設定が必要。ライブSDKのSG Comと区別 |

</div>

<span id="web-and-speech-synthesis" class="comparison-anchor" aria-hidden="true"></span>

### Web・音声合成

<div class="comparison-table" role="region" aria-label="Webと音声合成向け候補" tabindex="0">

| 候補 | 方式と用途 | 導入前に確認すること |
| --- | --- | --- |
| [Azure Speech Viseme](https://learn.microsoft.com/en-us/azure/ai-services/speech-service/how-to-speech-synthesis-viseme) | TTS生成時にVisemeを取得。Blendshape出力は55要素、60 fps | クラウド音声合成の利用条件と出力別の対応言語。任意の録音を解析する機能とは異なる |
| [TalkingHead](https://github.com/met4citizen/TalkingHead) | ブラウザーの3Dアバター。テキスト・時刻情報と音声を使った口形状生成 | TTS連携と対応言語を確認。MIT。顔の表情を付ける仕組みと音声からの感情推定を区別 |
| [HeadAudio](https://github.com/met4citizen/HeadAudio) | ブラウザー内で音声からVisemeを推定 | MIT、解析サーバー不要。作者は低SNR時の検出精度・発話判定の制約を明記 |

</div>

TTSが発音の時刻情報を返すなら、それを使う方法も検討できます。音声生成後に発音を推定し直す工程を省けますが、眉・視線・感情の演技は別に設計します。

<span id="research-and-video-generation" class="comparison-anchor" aria-hidden="true"></span>

## 研究モデルと動画生成

<div class="comparison-table" role="region" aria-label="研究モデルと動画生成方式" tabindex="0">

| 手法 | 出力・研究の焦点 | LAMとの違い・利用条件 |
| --- | --- | --- |
| [EmoTalk](https://github.com/psyai-net/EmoTalk_release) | 感情と発話内容を分けて表情を生成 | 感情表現の研究候補。公開実装はCC BY-NC 4.0、商用は別途問い合わせ |
| [EMOTE](https://arxiv.org/abs/2306.08990) | 発話内容と感情を分離して制御する3D表情生成 | FLAME系の表現。ARKitリグへの適合と、コード・モデルの条件確認が必要 |
| [FaceFormer](https://github.com/EvelynFan/FaceFormer) | Transformerによる音声から3Dメッシュへの生成 | ARKit 52をそのまま出す方式ではない。学習データ・モデルとリグの適合が必要 |
| [Wav2Lip](https://github.com/Rudrabha/Wav2Lip) | 顔動画の口元を音声に同期させる | UEのMorph Targetを直接駆動しない。公開研究版は非商用条件 |
| [MuseTalk](https://github.com/TMElyralab/MuseTalk) | 顔画像・動画の口元を生成する | 映像出力。コード・モデルは商用利用可と案内されるが、依存モデルには個別条件あり |

</div>

研究の公開実装、学習済み重み、データセットには別々の条件が適用される場合があります。導入時はリンク先の対象版を確認してください。このサイトではモデルや他製品のコードを再配布しません。

<span id="how-to-read-quality-evidence" class="comparison-anchor" aria-hidden="true"></span>

## 品質評価の読み方

<span id="what-a-published-comparison-covers" class="comparison-anchor" aria-hidden="true"></span>

### 公開比較研究が示す範囲

2026年6月の[UE向け音声駆動表情の比較研究](https://arxiv.org/html/2606.10753v1)は、MetaHuman、Audio2Face、ARKit向けに学習したFaceDiffuserとProbTalk3D-Xを比較しています。12音声と8音声を使う２実験で、MetaHumanが口の同期・リアリズム・表現力の平均評価で最上位でした。

**LAMは対象外です。** 評価にはMetaHumanなどの高品質な人間型キャラクターを使用しており、日本語やアニメ調リグでの優劣を決める結果ではありません。

<span id="distinguish-lams-existing-measurements" class="comparison-anchor" aria-hidden="true"></span>

### LAMの既存測定との区別

LAMの初期化後の固定窓推論は、公開記録でDirectML約6.7 ms、CPU約50〜57 msです。これはモデルの処理時間で、音声取得から表情表示までの遅延ではありません。UE・Windows・機器条件、NullRHIの測定、実マイクの未検証範囲を[検証結果](/LAMAudio2Expression-UE/validation/)に記載しています。SG Comの50 msなどと横並びの速度ランキングにはしません。

<span id="evaluate-with-the-same-audio" class="comparison-anchor" aria-hidden="true"></span>

### 同じ音声で確かめる項目

1. **閉口**：「ぱ・ば・ま」で唇が適切に閉じるか。
2. **早口と無音**：発音の切り替わり、語尾、無音中の口の動き。
3. **感情**：同じ台詞に異なる感情を指定できるか、口以外にも意図が伝わるか。
4. **同期**：音声と口のずれ、入力開始から表情が見えるまでの時間。
5. **同時負荷**：描画中のCPU／GPU・メモリ使用量、複数キャラクター時の安定性。

入力音声、再生音量、カメラ、出力リグ、設定、機器を揃え、方式によるリグの違いは記録します。[日本語6音声のデモ](/LAMAudio2Expression-UE/demo/)は評価素材の出発点にできますが、既存の機能テストだけで他方式との品質差は決まりません。
