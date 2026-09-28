# Wav2ARKit CPUモデル

既存のModel設定から [myned-ai/wav2arkit_cpu](https://huggingface.co/myned-ai/wav2arkit_cpu) を選択できます。UE 5.8.2 / Win64のCPU対応です。SoundWave解析、解析済みClip、ライブPCM・マイクの共通推論処理を使用し、BlueprintやAnimGraphの接続は従来と同じです。既定モデルは従来LAMのままです。

## 導入

このモデル用のビルド済みReleaseはまだありません。対応ソースをビルドして使用してください。取得・インポート用のToolsは[デモリポジトリ](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo)側にあります。

1. デモプロジェクトの既存セットアップを済ませ、対応ソースのEditorをビルドします。
2. デモのルートで `.work/venv/Scripts/python.exe Tools/setup_wav2arkit.py` を実行します。
3. 次のコマンドでモデルを取り込みます（UEのパスは環境に合わせて変更）。

```powershell
& D:/Unreal/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe ./LAMDemo.uproject -run=pythonscript "-script=$PWD/Tools/import_wav2arkit.py" -unattended -nop4 -nosplash -nullrhi
```

4. Project Settings → LAM Audio2Expression → Modelで `/LAMAudio2Expression/Models/Wav2ARKit_CPU` を指定します。ライブ実行中の場合は停止してから変更し、再開してください。
5. パッケージ化では従来と同じく `/LAMAudio2Expression/Models` をCook対象に含めます。別プロジェクトへ移す場合は取り込んだモデルアセットをMigrateしてください。

CPU限定のモデルアセットを作るため、Prefer GPUが有効でもこのモデルでDirectMLを試行しません。実行環境にPythonや取得元のONNXファイルは不要です。UEは外部重みを含めてアセットへ取り込み、ランタイム側で必要に応じて作業用ファイルを作成します。

## 仕様と制約

- Style設定はこのモデルでは無視され、identity 11固定です。0〜11のどの値でも同じ結果になります。従来LAMのStyle機能は維持します。
- 生音声16 kHz・mono・float32を34,133サンプルずつ入力し、64フレーム×ARKit 52カーブを取得します。切り出し、30 fpsの同期、後処理は既存方式です。
- 公開ONNXの構造や重みは変更しません。モデル定義約1.86 MBと外部重み約402 MBの両方が必要です。
- 固定リビジョン、配布元のSHA-256、アセット名は [manifest](wav2arkit-model-manifest.json) に記録しています。取得・取り込みの両段階でファイルを照合し、不足・破損時は停止します。
- GPU、Android、実マイクの長時間運用は今回の検証範囲外です。配布元の速度表記はこのプラグインでの速度保証ではありません。

## 再検証

デモのルートで `Tools/setup_wav2arkit.py --fixtures` を実行し、インポートコマンドへ `-LAMWav2ARKitTests` を追加します。テスト用音声・不正モデルは `/Game/Wav2ARKitTests` に隔離されます。続いて以下を実行します。

```powershell
./Tools/test_wav2arkit.ps1 -Configuration Editor
./Tools/package.ps1 -Configuration Shipping -IncludeBaked -IncludeWav2ARKitTests
./Tools/test_wav2arkit.ps1 -Configuration Shipping
```

既存のBakedテストマップがない場合は、先に `Tools/test_baked_clips.ps1` を実行してください。Shipping検証中は取得元の2ファイルを一時的に改名し、終了時に元に戻します。テストはモデル原本の取得ディレクトリを他の処理が使用していない状態で実行してください。

CPU数値比較はPython ONNX Runtimeの無音・固定乱数・実発話を基準にし、初回／連続推論の最大絶対誤差1e-3以下を確認します。実発話の検証素材はデモのJVNV音声を使用し、その出典・利用条件はデモ側のResources/Demo/README.mdに従います。参照テンソルと生成アセットはGitへ含めません。
