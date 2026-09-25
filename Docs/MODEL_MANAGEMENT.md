# 学習済みモデルの運用

モデルはApache-2.0で配布されています。MITへ変更せず、ライセンス本文・帰属・変換時の変更説明を保持すれば、変換済みモデルをプラグインのバイナリやアプリに同梱できます。

- [公式モデルカード（固定版）](https://huggingface.co/3DAIGC/LAM_audio2exp/blob/0fe5f4dbb283ec7d9c01688681e6e4b6ac314858/README.md)
- [同梱ライセンス本文](../Licenses/Apache-2.0.txt)
- [変換元と変更説明](../THIRD_PARTY_NOTICES.md)
- [形状・リビジョン・SHA-256](model-manifest.json)

Gitにはソース・モデルの検証情報を保存し、約384 MiBのニューラルモデルuasset、ONNX、チェックポイントは除外します。生成には [デモリポジトリのsetup](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Tools/setup.ps1) を使用してください。キャラクターと音声はデモリポジトリ側で管理します。

変換済みモデルを配布する場合は、GitHub Releaseなどに置き、プラグイン版・UE版・モデル版・SHA-256・Apache-2.0本文・変更説明をセットにします。ソース側は公式配布元の固定リビジョンを参照します。このリポジトリではモデルを含むReleaseはまだ作成していません。

この確認は配布者のライセンス表示に基づきます。学習データ全体の権利調査や権利保証を意味しません。デモ素材の条件は [デモリポジトリの出典](https://github.com/yeczrtu/LAMAudio2Expression-UE-Demo/blob/master/Resources/Demo/README.md) を参照してください。
