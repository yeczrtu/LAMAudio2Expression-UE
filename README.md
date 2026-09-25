# LAM Audio2Expression for Unreal Engine 5.8

Windows x64用ランタイムプラグインです。SoundWaveを非同期解析し、音声再生に同期したARKit 52カーブを専用AnimGraphノードで適用します。UE NNEのDirectMLを優先し、CPUへフォールバックします。

このリポジトリのルートがプラグインです。`.uproject`は含みません。

```text
LAMAudio2Expression.uplugin
Source/
Content/Demo/
Resources/Demo/
Licenses/
Docs/
```

## 導入

UE 5.8.2、Windows、Visual Studio 2022 C++で検証しています。プロジェクトのPluginsに、このリポジトリを `LAMAudio2Expression` フォルダー名で配置してビルドしてください。

```powershell
git submodule add https://github.com/yeczrtu/LAMAudio2Expression-UE.git Plugins/LAMAudio2Expression
```

通常のcloneやZIPからのコピーでも導入できます。Project Settings → LAM Audio2Expression のModelを `/LAMAudio2Expression/Models/LAM_A2E` に設定し、パッケージ設定のAdditional Asset Directories to Cookに `/LAMAudio2Expression/Models` を追加します。

## 学習済みモデルとデモプロジェクト

ニューラルモデルはGitに含みません。[LAM-A2EUE](https://github.com/yeczrtu/LAM-A2EUE) がこのプラグインをサブモジュールとして使用するデモ・検証用プロジェクトです。

```powershell
git clone --recurse-submodules https://github.com/yeczrtu/LAM-A2EUE.git
cd LAM-A2EUE
./Tools/setup.ps1 -Engine D:\Unreal\UE_5.8
```

setupは固定版の公式チェックポイントを取得・検証し、ONNX変換、数値比較、UEへのインポートを行います。生成先は `Plugins/LAMAudio2Expression/Content/Models` です。他のプロジェクトへ導入するときは、この生成済み `Content/Models` もコピーしてください。Pythonとネットワークが必要なのは変換時だけです。

## プラグイン内デモ

Content Browserで「プラグインコンテンツを表示」を有効にし、`/LAMAudio2Expression/Demo/Maps/LAM_FaceDemo` を開いてPlayします。hinzka / VRoidのFace52とJVNV / litaginの音声6件を同梱しています。1～6またはクリックで音声選択、Spaceで一時停止／再開、Rで再生できます。

`Content/Demo` は `/Game` やデモプロジェクトのC++モジュールに依存しません。パッケージに含める場合はこのマップをMaps to Cookに追加してください。

## Blueprint / AnimGraph

Actorへ `LAMAudio2ExpressionComponent` を追加し、`Analyze SoundWave Async` のCompletedから `Play Expression Clip` を呼びます。AnimBPでは入力ポーズに `Apply LAM ARKit Curves` を接続します。52値を個別に配線する必要はありません。

詳細なAPI・マイク／PCM入力・検証手順は [デモリポジトリのREADME](https://github.com/yeczrtu/LAM-A2EUE#blueprint)、数値検証は [検証結果](https://github.com/yeczrtu/LAM-A2EUE/blob/master/Docs/VALIDATION.md) を参照してください。

## ライセンス

独自コードは [MIT](LICENSE)。上流由来部分と学習済みモデルはApache-2.0です。デモ音声と同期映像表現はCC BY-SA 4.0、キャラクターは作者の再配布許諾に従います。

[第三者表記](THIRD_PARTY_NOTICES.md)、[デモ素材の出典・条件](Resources/Demo/README.md)、[モデル運用方針](Docs/MODEL_MANAGEMENT.md)を参照してください。配布時はこれらと `Licenses` を保持してください。
