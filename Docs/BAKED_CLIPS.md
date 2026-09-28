# SoundWaveの事前解析

SoundWaveをエディタで解析し、ARKit52カーブをアセットに保存できます。保存済みClipの再生ではモデルロード・解析用デコード・推論を行いません。従来の `Analyze SoundWave Async`、マイク、PCM入力はそのまま利用できます。

## 生成

1. コンテンツブラウザでSoundWaveを選択します。複数選択も可能です。
2. 右クリック → **Generate LAM Expression Clip** を選びます。
3. Style、平滑化、無音時の口抑制、対称化、瞬きとシードを設定して **Generate** を押します。
4. 完了後、生成されたアセットをUEの **Save All** で保存します。

元音声と同じフォルダーに `<音声名>_LAMClip` が生成されます。同じ音声の既存Clipは更新され、別種類・別音声の同名アセットがある場合は連番が付きます。設定違いを残す場合は、先に生成済みClipを複製またはリネームしてください。

Clipの右クリック → **Regenerate** は、そのClipに保存された解析設定で再生成します。設定を変えて生成する場合は元音声の **Generate LAM Expression Clip** を使います。PIE中の生成は無効です。

バッチは1音声ずつ処理します。失敗した音声の理由は画面とOutput Logに表示し、次の音声へ進みます。Cancelまたはウィンドウを閉じると残りの処理を中止します。処理中の推論呼び出しは完了まで待ちますが、結果は反映しません。完了済みのClipは残り、更新失敗したClipの旧データも保持されます。

## Blueprintでの再生

```text
ロード画面・BeginPlay: ClipとSoundWaveをロード → Prime Sound
後の再生操作:         保存済みClip → Play Expression Clip
AnimGraph:            Apply LAM ARKit Curves / Apply LAM Viseme Curves
```

生成アセットは既存の `ULAMExpressionClip` の派生型なので、既存のClipピンへ直接指定できます。Pause、Resume、Seek、音量、フェード、2D/3D、サブミックスとイベントは従来と共通です。

BlueprintでClipを直接参照すると音声もロード対象になります。Soft Object Referenceを使う場合は、UEのAsync Load Assetの完了を待って参照を保持してください。ストリーミング音声は先行して **Prime Sound** で読み込みます。Prime Soundは非同期で、読込完了通知ではありません。再生直前ではなくロード画面などで呼び出し、必要に応じて音声のRetain On Load設定を使用してください。キャッシュやストレージの状態による音声側の待ちは別途考慮します。

**解析待ちゼロ**とは、ロード済みClipの再生時に解析完了を待たないことです。UEの音声デコード・音声出力バッファ・描画周期までゼロにするものではありません。

Visemeは保存しません。変換方式、テンプレート、入力補正、各口形の強度を再生時に変更できますが、Viseme変換のCPU負荷は残ります。ARKitカーブの保存容量は約366KiB/分（float32、30fps、52チャンネル）です。

## 更新と配布

Clipには音声更新GUID、解析入力PCMのSHA-1、モデルGUID、解析・形式バージョンを記録します。右クリックのUE標準 **Validate Assets** で入力の変更を確認できます。音声の再インポート、加工・圧縮設定変更、モデル更新後は再生成してください。解析中に音声やモデルが変更された場合、その結果は破棄されます。

ロード時にデータを検証し、破損・未対応形式は再生時の `On Playback Failed` に `InvalidBakedClip` と理由を返します。自動で動的解析へ切り替えることはありません。生成済みClipからの再生は参照共有と52カーブの補間であり、再生ごとの全配列コピーはありません。

生成ClipをBlueprintなどから参照するとCook対象になります。Soft参照だけの管理ではAsset ManagerまたはAdditional Asset Directories to Cookで対象に含めてください。モデルの同梱設定、対応プラットフォーム、通常解析の仕様は変更しません。初版はUE 5.8.2 / Win64、非ループ・1倍速、mono/stereo、8-192kHz、最大5分です。

## 自動化

Editor Utility Blueprint/Pythonから `LAMBakeSubsystem` を取得し、`GenerateClips` / `RegenerateClips` を呼び出せます。`IsBusy`、`GetProgress`、`GetStatus`、`GeneratedClips`、`Errors`、`Cancel` を利用してください。処理中はエディタをTickさせる必要があるため、Pythonの同期ループで待機しないでください。生成成功はディスク保存を意味しません。

デモ側の `Tools/build_baked_examples.py` は、生成済み `speech_stream_LAMClip` から `/Game/Examples/BP_LAMBakedPlayback` を作ります。レベルに配置するとBeginPlayで先読みし、Spaceで再生します。既存の動的解析例を置き換えません。
