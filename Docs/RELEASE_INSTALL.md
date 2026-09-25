# v0.2.0 モデル入りプラグイン

対象: **Unreal Engine 5.8.2 / Windows x64**。Editor Development、Game Development / Shippingのプリコンパイル成果物とC++ソースを含みます。他のUE版はソースから再ビルドしてください。

1. エディタを終了し、ZIPを展開します。
2. `LAMAudio2Expression` を `<Project>/Plugins/LAMAudio2Expression` に置きます。既存版を更新する場合は、古いプラグインフォルダーを別の場所へ退避してから置き換えます。
3. UEでプラグインを有効にして再起動します。必要なNNE ORTとAudioCaptureは依存プラグインとして有効になります。
4. Project Settings → LAM Audio2Expression の Model は既定の `/LAMAudio2Expression/Models/LAM_A2E` を使います。
5. **Cookする場合**は Project Settings → Packaging → Additional Asset Directories to Cook に `/LAMAudio2Expression/Models` を追加します。
6. Actorに `LAMAudio2ExpressionComponent` を追加し、`Analyze SoundWave Async` のCompletedから `Play Expression Clip` を呼びます。AnimBPに `Apply LAM ARKit Curves` を接続します。

Editorと実行時にPython、外部推論サーバー、追加モデルダウンロードは不要です。DirectMLを優先し、CPUへフォールバックします。モデルは約384 MiBです。初回ロードと解析が完了してから再生します。

同梱の `Source`、`Intermediate/Build` はGameビルド用データを含みます。配布ZIPから削除しないでください。モデルのハッシュ・固定版・数値比較は [model-manifest.json](model-manifest.json) に記録しています。

顔・音声・マップは別の [デモRelease](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/releases/tag/v0.2.0) にあります。

独自統合コードはMIT、上流由来コードと学習済みモデルはApache-2.0です。`LICENSE`、`THIRD_PARTY_NOTICES.md`、`Licenses`、モデルの出典・変更説明を保持してください。モデルをMITへ変更するものではありません。

検証範囲: UE 5.8.2 / RTX 3070 / Core i7-12700。既存のPIE・Standalone・Win64 Development/Shippingテストに加え、リリースZIPを別ディレクトリへ展開して検証します。実マイクの長時間運用、実GPU故障、音声出力装置の物理遅延は未検証です。
