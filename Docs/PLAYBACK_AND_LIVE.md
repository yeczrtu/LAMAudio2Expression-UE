# 再生制御とライブ入力（0.2）

UE 5.8.2 / Win64。既存の Play Expression Clip、Pause、Resume、Stop、Seek、AnimGraph 接続を維持しています。

## 再生と出力先

コンポーネントの Playback Settings を設定して Play Expression Clip を呼びます。呼び出しごとに変える場合は Play Expression Clip With Settings を使います。

| 設定 | 既定・動作 |
|---|---|
| Output Submix | 未指定なら音声/SoundClass/エンジンから継承。指定すると主出力を置き換える |
| Additional Submix Sends | 追加の並列送信。Level は 0〜1、同じ送信先は最後の指定を採用 |
| Inherit SoundWave Sends | 有効。無効時は元SoundWaveの追加センドを除外 |
| Sound Class Override | 未指定なら元音声の設定 |
| Volume / Muted | 1 / false。ミュートでも表情と再生位置は進む |
| Playback Mode | Two Dimensional。Three Dimensional では Attachment / Socket / Transform を指定 |
| Attachment | 3Dでは未指定なら所有ActorのRoot。未登録・別World・存在しないSocketはエラー |
| Attenuation Settings | 3Dの距離減衰・空間化。2Dでは減衰・空間化を無効化 |
| Concurrency Settings | 未指定なら元音声から継承。指定時はそのUSoundConcurrencyアセットで制御 |
| Play When Game Paused | false。有効時は音声と表情をゲーム停止中も更新 |
| Fade In Duration | 0秒。線形フェード |

Set Output Submix、Set Submix Send、Remove Submix Send、Set Volume、Set Muted は再生中にも反映します。これらのSetterは現在の再生とコンポーネントの次回既定設定を更新します。主出力を未指定に戻すと継承へ戻り、追加センドを削除するとその送信先の元音声設定を再び継承します。

音声・解析結果のアセットを書き換えません。同じクリップを別コンポーネントで別々の出力先へ再生できます。送信先が同じ親サブミックスへ合流する場合、並列センドの音声は加算されます。

Play When Game PausedはSoundClassのUI指定より優先します。

ゲーム停止中に表示メッシュのAnimBPも評価する場合、そのSkeletalMeshComponentの **Tick Even When Paused** も有効にしてください。Face Demoでは設定済みです。Attenuationアセットによる距離依存センドやリバーブはUE側の設定を維持します。

Fade Out And Stop は音量を線形に下げて停止します。Pause は再生位置・表情・フェードを保持し、Resume で継続します。Stopと自然終了の後は表情を100msで入力ポーズへ戻します。Seek は同じ再生IDを維持し、内部再生成時にも設定と一時停止状態を維持します。停止後のSeekは新しい再生を開始します。

## イベント

各イベントのInfoには、コンポーネント内で単調増加する Playback Id と Clip を含みます。Get Playback InfoからState、Position、Duration、Progressも取得できます。

- On Playback Started: エンジンが再生を受け付けたとき。シークでは再通知しません。
- On Playback Finished: 元音声の終端を確認できた自然終了のみ。
- On Playback Ended: 受け付け済み再生につき一度。理由はCompleted / Stopped / Replaced / Interrupted / Failed。
- On Playback Failed: Error.Code / Error.Messageで入力不正や開始拒否を通知。
- On Playback State Changed: Starting / Playing / Paused / FadingIn / FadingOut / Stopped / Failed。

事前検証で拒否された要求はFailedイベントのみを通知し、既存の再生を維持します。開始後の終了は State Changed → Ended → Finished（自然終了）またはFailed（開始失敗）の順です。リスナーで次の再生を開始できるため、通知内容はCurrentClipではなくイベント引数で参照してください。

Actor破棄・PIE終了中のイベントは抑止します。ループと速度変更は対象外で、ループ指定のSoundWaveは拒否します。完了はクリップの音声終端を表し、サブミックス側のリバーブ等の残響終了は待ちません。

会話を次へ進める場合はOn Playback Finished、UIの後始末にはOn Playback Endedを使用できます。例えば、解析のCompleted → Play Expression Clip With Settingsへ接続し、Info.PlaybackIdを会話要求と対応させます。失敗はOn Playback FailedのCodeを記録し、EndedのReasonがReplaced/Stoppedのときは次の台詞を自動開始しない構成にします。

## ライブ推論

Start Microphone / Start PCM Streamの前後にSet Live Inference Intervalを呼びます。引数はms、内部では1〜30フレーム（約33.3〜1000ms）へ丸めます。既定は10フレーム（約333.3ms）。Get Live Inference Intervalで丸めた要求値、Get Live Metricsで実際の処理中の値を取得します。

実行中の変更は次のジョブから反映します。固定入力34133サンプル・出力64×52・30fpsは維持し、間隔Hフレームに対して末尾Hフレームを採用します。SoundWave事前解析の1秒刻みとキャッシュは変更しません。

Presentation Delayは希望する最小遅延（秒、既定0.75、0.4〜2.0）。実効値は要求下限と「処理間隔＋結果到着遅延P95＋1フレーム」の大きい方で、最大2秒です。実行中は増加方向のみ調整し、表示時刻を巻き戻さず保持します。縮小は次回の開始時に再計算します。

Get Live Metricsは次を返します。

- Actual Interval Milliseconds / Effective Presentation Delay Milliseconds
- Inference P95 Milliseconds（推論・後処理）、Result Latency P95 Milliseconds（キュー待ちを含む）
- Initialization Milliseconds（モデルロード、モデル/インスタンス初期化。P95から分離）
- Dropped Intervals、Backend、State

P95は直近60回。準備中の0は未計測を表します。On Live State ChangedでPreparing / Running / Lagging / Failed / Stoppedを通知します。短い間隔でのリアルタイム性はハードウェアと同時負荷に依存し、指定間隔を保証するものではありません。

入力待ちは最大2秒、推論はセッションあたり同時に1件です。古い待ち仕事を破棄して最新位置へ復帰します。固定モデルの文脈用履歴は入力待ち2秒とは別に保持します。結果がない区間は100ms保持してから100msで表情を戻し、有効な結果の到着で復旧します。

マイクのスピーカー再生は行いません。PCMは8〜192kHz、モノラル/ステレオのインターリーブfloat、1回最大2秒。サンプルレート変更時はストリームを再開始してください。

## 実装上の注意

主出力と追加センドは音声デバイスごとのUAudioEngineSubsystemで再生IDを限定して適用します。コールバックと実音声クロックのスナップショットをゲームスレッドで消費します。

再生・シークの初期化中の進捗値を誤採用しないよう、SourceBufferListenerで最初のバッファ生成を確認してから時計を採用します。リスナーは音声データを複製・変更・保存しません。

UE 5.8のFActiveSound::OverrideVirtualizationModeはEngine DLLからエクスポートされていません。そのためミュート時の継続再生には軽量なUSoundBaseプロキシを使用します。共通の音声設定と元SoundWaveへの参照だけを持ち、PCMやCook済みバルクデータは複製しません。主出力の変更にも、エンジンや共有SoundWaveの書き換えは不要です。

同じSoundWaveの同時再生ではプロキシを共有し、SoundWave内のConcurrency Overridesもコンポーネント間で共有します。通常のAudioComponentによる直接再生とプラグイン再生を同じ制限グループにする場合は、共通のUSoundConcurrencyアセットを指定してください。ルーティング・音量・空間設定は再生ごとに独立しています。
