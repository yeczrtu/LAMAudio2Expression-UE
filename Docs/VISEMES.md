# ARKit52 → あいうえお / Oculus Viseme

`Apply LAM Viseme Curves` は、LAMの補間済みARKit52から母音5種、またはOculus互換15枠を連続ブレンドします。追加モデル、Oculus SDK、再解析は不要です。SoundWaveとライブ入力の両方で使用できます。

## AnimGraphで使う

1. 口形状がニュートラルのメッシュに、あいうえおのMorph Targetを用意します。
2. Content Browserで **Show Plugin Content** を有効にし、`/LAMAudio2Expression/Profiles/DA_JapaneseFive` をプロジェクトへ複製します。
3. プロファイルの **Target Names** で5母音の出力先をメッシュのMorph Target名に設定します。初期名は `A/I/U/E/O`。未指定または `None` は書き込みません。
4. AnimBPで `元のポーズ → Apply LAM Viseme Curves → Output Pose` と接続し、**Profile** を指定します。**Source Component** はLAMコンポーネント、空欄なら所有Actorから検索します。複数ある場合は明示的に指定してください。

Profile未指定時も既定設定と `A/I/U/E/O` を使用します。Morph Targetと同名のアニメーションカーブを出力し、ボーンを直接動かしません。モーフが存在しない名前はメッシュを変形しません。カーブ駆動リグにも利用できます。

目・眉もLAMで動かす場合は、前段の `Apply LAM ARKit Curves` に `/LAMAudio2Expression/Profiles/DA_UpperFaceOnly` を設定します。このプロファイルは `jaw*`、`mouth*`、`tongueOut` の出力を無効化します。**前段から既に入っている口カーブを消去する設定ではありません**。全52カーブのノードを別途重ねたり、同じ口をSet Morph Targetで同時制御したりしないでください。

```text
元のポーズ → Apply LAM ARKit Curves (UpperFaceOnly)
           → Apply LAM Viseme Curves (モデル用Profile) → Output Pose
```

## Oculus形式

内部配列の順序は固定です。名前プリセットと、内部配列の順序は別です。

```text
0 sil, 1 PP, 2 FF, 3 TH, 4 DD, 5 kk, 6 CH, 7 SS, 8 nn, 9 RR,
10 aa, 11 E, 12 ih, 13 oh, 14 ou
```

| 日本語 | 汎用5母音 | Oculus資料名 | Oculus SDK名 |
|---|---|---|---|
| あ | A | aa | aa |
| い | I | I | ih |
| う | U | U | ou |
| え | E | E | E |
| お | O | O | oh |

`DA_OculusReference` と `DA_OculusSDK` は15枠の名前を登録済みです。これら既存プロファイルの`FiveVowelRules`方式では、子音9枠は常に0を出力します。`sil` は `1 − 母音重みの合計` で、中立口形状の残余です。音声の無音検出ではありません。silモーフが不要ならTarget NamesのsilをNoneにしてください。

これは出力枠・名前の互換であり、Oculus Lipsyncによる音声解析や子音認識を再現するものではありません。

参考: [Meta Viseme Reference](https://developers.meta.com/horizon/documentation/unity/audio-ovrlipsync-viseme-reference/)、[Meta Viseme Enum](https://developers.meta.com/horizon/reference/voice/v85/enum_meta_wit_ai_t_t_s_data_viseme/)。

## Oculus15のテンプレート逆算

子音モーフを持つモデルでは、次のプロファイルを複製して`Apply LAM Viseme Curves`に設定してください。既存アセットとProfile未指定時は従来の5母音方式を維持します。

| プロファイル（`/LAMAudio2Expression/Profiles/`） | 用途 | 初期出力名 |
|---|---|---|
| `DA_OculusOpenFaceFX` | 新規Oculus15設定の推奨出発点 | SDK名 `sil/PP/…/ih/oh/ou` |
| `DA_OculusTalkingHead` | TalkingHeadの配合で作られたモーフ | `viseme_sil/viseme_PP/…/viseme_I/viseme_O/viseme_U` |

`Settings.ConversionMode=TemplateFit`で有効になり、`Template`で配合を選びます。名前プリセットの`OculusPrefixed`は`viseme_*`名を設定します。名前と変換方式は独立して変更できます。5母音だけのモデルは`FiveVowelRules`を使用してください。

[OpenFaceFX](https://openfacefx.com/docs/retargeting/)と[TalkingHead](https://github.com/met4citizen/TalkingHead/blob/5b1f12057a0edad83d1fc75714217dbbc9496aa7/blender/build-visemes-from-arkit.py)が公開しているのはViseme→ARKitの配合です。本機能は、その配合行列Bに対して入力xを近似する14重みvを求めます。OpenFaceFXは`f898db3c825bf89cfec63391bb16d91fc42192e8`、TalkingHeadは`5b1f12057a0edad83d1fc75714217dbbc9496aa7`に固定しています。元データとMITライセンスは`ThirdParty/VisemeTemplates/`に同梱しています。

```text
minimize Σ q[i] × ((Bv)[i] − x[i])² + 0.0001 × Σ v[j]²
subject to v[j] ≥ 0, Σ v[j] ≤ 1
```

入力は既存の補正後に`jawOpen *= 1 − mouthClose`を適用します。左右は参照配合の各チャンネルを保持します。旧方式の開口ゲートは使わないため、開口のないPPなども有効です。目・眉など配合に使われない入力は推定結果に影響しませんが、52値すべての有限性を検査します。

`Input Corrections`の`FitWeight`が観測重みqです。未指定は1、範囲は0～1。供給されない`tongueOut`などは0にして、その値を誤差計算から除外します。入力が0というだけでは未供給とみなしません。`Scale=0`は「観測値0」を意味するため、未供給の指定には`FitWeight=0`を使用します。参照配合に含まれるすべての観測項を無効にした設定はエラーです。`mouthClose`は顎の補正専用で、参照配合に含まれないためFitWeightの対象外です。

`Viseme Gains`は非中立14枠の出力倍率です。未指定は1。倍率適用後に合計が1を超えた場合のみ比例縮小し、`sil=1−合計`とします。`sil`の倍率設定は無効です。`Vowel Gains`と開口・横幅・丸みの閾値は旧方式専用です。

倍精度FISTAをゼロ初期値から最大1,024反復実行し、射影勾配が`1e-8`以下なら終了します。反復中の動的メモリ確保や前フレームからの初期値の持ち越しはありません。AnimGraphは設定変更時に計算用行列を再作成します。C++で連続変換する場合も`LAM::PrepareVisemeSettings`の戻り値を保持し、prepared版の変換関数へ渡せます。この戻り値は変更せず、設定変更時に作り直してください。

両配合行列ともランク11で、14重みを常に一意に逆算することはできません。TalkingHeadではCH/RRが同じ配合なので、倍率適用前に均等配分します。OpenFaceFXでは別配合ですが、混合状態の曖昧さは残ります。これは口形状の近似であり、音素認識やOculus音声解析の再現ではありません。笑顔などの表情も口形状に含まれるため、発話との区別は行いません。

### 検証用アセット

`Tools/build_viseme_examples.py`は既存アセットを保持しながら、Face52の複製に14個の参照モーフを生成します。元のメッシュは変更しません。

- `/Game/LAMVisemeExamples/SK_VisemesOpenFaceFX`と`ABP_OpenFaceFX`
- `/Game/LAMVisemeExamples/SK_VisemesTalkingHead`と`ABP_TalkingHead`

```powershell
./Tools/test_face_demo.ps1 -Configuration Editor -VisemeTemplate OpenFaceFX -OutputDirectory Artifacts/OculusOpenFaceFXEditor
./Tools/test_face_demo.ps1 -Configuration Editor -VisemeTemplate TalkingHead -RepresentativePoses -OutputDirectory Artifacts/OculusTalkingHeadPoses
```

通常は6音声、`-RepresentativePoses`指定時は中立＋14形状を確認します。`-Configuration Shipping`でパッケージも検証できます。既存デモの通常起動は変更せず、テスト引数があるときだけメッシュとAnimBPを切り替えます。参照モーフでの検証と実モデルでの品質評価は別に行ってください。Face52ではPPの口の隙間、開口系の弱さ、舌・丸めの強さが見られます。固定配合の検証用素材であり、完成したViseme素材としての品質を保証するものではありません。

## Blueprint / C++

```text
Get Current Expression Frame
  → Convert ARKit To Visemes(Frame, Profile)
  → Get Vowel Weights       # A, I, U, E, O
  または Get Viseme Weight # ELAMVisemeで1値を取得
```

`FLAMVisemeFrame.Values` は15値です。`TimeSeconds`、`bValid`、`Weight` を伴います。値自体にWeightは掛かっていません。独自適用する場合はValidityを確認し、AlphaとWeightを一度だけ適用します。無効時も配列は15個の0で、silも0です。

`ConvertARKitToVisemes` はステートレスなBlueprintPure関数です。外部ARKit入力も `Get ARKit Curve Names` の順に52値を詰め、Validity/Weightを設定すれば変換できます。辞書の列挙順や他SDKの列挙順をそのまま渡さないでください。

C++では `LAMViseme.h` の `LAM::ConvertARKitToVisemes(Frame, Settings)` がUObject参照なしで使えます。`ULAMVisemeLibrary` の関数はプロファイルの検証とBlueprint公開を担当します。

名前の再設定には `Profile.ApplyNamePreset`、上顔用ルールの生成には `ApplyUpperFaceOnlyPreset` を使用します。前者はTarget Names全体、後者はRules全体を置き換えます。変換設定は名前プリセット変更でリセットされません。

## 変換設定

入力値を0～1に制限した後、指定されたカーブについて `sat((入力 − Baseline) × Scale)` を適用します。未指定はBaseline=0、Scale=1。左右は補正後に平均します。

```text
o = sat(jawOpen + LipOpenContribution × (avg(mouthUpperUp) + avg(mouthLowerDown))) × (1 − mouthClose)
w = smoothstep(Width.Low, Width.High, max(avg(mouthStretch), SmileContribution × avg(mouthSmile)))
r = smoothstep(Roundness.Low, Roundness.High, max(mouthFunnel, mouthPucker))
g = smoothstep(Activation.Low, Activation.High, o)
h = smoothstep(OpenSplit.Low, OpenSplit.High, o)

A = g × (1−r) × (1−w)
I = g × (1−r) × w × (1−h)
U = g × r × (1−h)
E = g × (1−r) × w × h
O = g × r × h
```

既定値: LipOpenContribution=.25、SmileContribution=.15、Width/Roundness=.10～.60、Activation=.05～.25、OpenSplit=.20～.50、VowelGains各1。母音別倍率を掛けてから、合計が1を超える場合だけ全体を比例縮小します。

調整順は、ニュートラル時の残りをInput Correctionsで補正 → Activationで反応する開口量を調整 → Width/Roundness/OpenSplitで母音の分配を調整 → Vowel Gainsで対象モーフの変形量を調整、を推奨します。既定係数は出発点であり、モデル横断で発音精度を保証するものではありません。特に「い／え」「う／お」は形状による近似です。

設定の **Validate Assets** または `ValidateProfile` で、有限値、閾値の大小、未知・重複した入力補正、重複した出力名を検査できます。閾値は0～1、High−Lowは1e-6以上です。不正設定では変換しません。52要素以外、Validity=false、非有限の入力・時刻・Weightも無効扱いです。

## 同期とフェード

既存の補間後に変換し、追加の時間平滑化は行いません。Pauseでは保持、Seekでは即座に追従します。AnimGraphで `lerp(元のカーブ, 変換値, clamp(Alpha × Frame.Weight))` を適用します。停止はコンポーネントの100 msフェード、供給元消失はノードの100 msフェードで元のポーズへ戻ります。元のポーズに口のアニメーションがある場合は、その口形状へ戻ります。

## デモと検証

デモリポジトリの `/Game/LAMVisemeExamples/ABP_LAMVisemes` は、Face52に元から含まれる `Fcl_MTH_A/I/U/E/O` に接続済みです。既存の顔デモは変更せず、テスト起動時のみAnimBPを切り替えます。

再生成: UE Pythonで `Tools/build_viseme_examples.py` を実行します。既存アセットは上書きしません。

```powershell
# ビルド済みEditorから新規テストを実行
UnrealEditor-Cmd.exe LAMDemo.uproject -unattended -nullrhi '-ExecCmds=Automation RunTests LAM.Viseme' '-TestExit=Automation Test Queue Empty'

# デモリポジトリの実メッシュ・6音声 / 中立と5母音のスクリーンショット
./Tools/test_face_demo.ps1 -Configuration Editor -Visemes -OutputDirectory Artifacts/VisemeDemo
./Tools/test_face_demo.ps1 -Configuration Editor -RepresentativePoses -OutputDirectory Artifacts/VisemePoses
```

自動テストは代表口形状、閉口、入力補正、範囲外・非有限値、ランダム入力、連続性、プリセット、カーブ適用、Alpha/Weight、入力喪失、プロファイルのスナップショットを検査します。見た目の品質は対象モデルでも確認してください。
