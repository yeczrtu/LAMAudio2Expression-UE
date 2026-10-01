# Google検索への登録と効果確認

対象：`https://yeczrtu.github.io/LAMAudio2Expression-UE/`

## サイト内で対応した内容

2026-10-01に、トップ・導入・Blueprint・比較・ライブ入力・SoundWave事前解析のタイトル、説明文、導入文を日英で編集しました。日本語トップのタイトルは「UE5の音声リップシンクプラグイン | LAM Audio2Expression」です。トップから利用目的ごとのガイドへ案内し、対応版はUE 5.8.2／Windows x64と明記しています。URLとページ数（日英30ページ）は維持しています。

静的HTML、自己参照canonical、相互hreflang、OG、パンくず、内容に対応するTechArticle、XMLサイトマップは既存構成を使用します。404はnoindexで、サイトマップから除外します。検索語を並べた隠し文やmeta keywordsは追加しません。

## Search Consoleの設定手順

現時点では所有権確認・サイトマップ送信・Googleの登録状況は未確認です。サイトの公開成功だけではGoogleへの登録完了とは扱いません。

1. サイト所有者のGoogleアカウントで[Search Console](https://search.google.com/search-console/)を開きます。すでに対象プロパティがある場合は再利用します。
2. 未登録なら「URLプレフィックス」に`https://yeczrtu.github.io/LAMAudio2Expression-UE/`を指定します。GitHubのドメイン全体を所有していないため、DNSでのドメインプロパティ作成は使いません。
3. 「HTMLタグ」を選び、実際に発行された`google-site-verification`タグを取得します。所有者のアカウントに対応する値を使用し、別アカウントの値や仮の値は公開しません。
4. サイト専用ブランチの`src/components/Head.astro`で、日本語トップの`head`内に確認タグを静的出力します。ほかの確認タグがあれば保持します。実装後に`npm run check`、`npm run build`を実行して公開します。
5. ログアウト状態でも公開トップのHTMLソースに正しいタグがあることを確認し、Search Consoleで「確認」を実行します。認証を維持するため、確認後もタグを削除しません。
6. サイトマップ画面で`https://yeczrtu.github.io/LAMAudio2Expression-UE/sitemap-index.xml`を送信し、処理結果を確認します。
7. トップ、`installation/`、`lip-sync-comparison/`をそれぞれURL検査します。公開URLテスト、インデックス可否、選択されたcanonical、表示された除外理由を確認し、問題がなければ登録をリクエストします。

HTMLタグは所有権確認用で、アクセス解析のコードではありません。Googleアナリティクス、広告、追加ユーザーへの権限付与はこの作業に含めません。Googleへのログイン等の本人操作が必要な場合は、所有者が操作します。

ドメイン直下の`https://yeczrtu.github.io/robots.txt`が対象であり、このサイトのサブディレクトリにrobots.txtを置いても代わりにはなりません。登録のためにホスト直下の設定を変更する必要があるとは扱いません。

## 登録結果として残す項目

実際に確認できた段階だけ記録します。未確認項目を成功として報告しません。

| 項目 | 記録内容 |
| --- | --- |
| 所有権 | 対象URL、確認日時、確認方法、成功／未完了 |
| サイトマップ | 送信URL、送信日時、Search Consoleの状態と検出URL数 |
| URL検査 | 対象URL、Google登録状態、公開URLテスト、canonical、除外理由、登録リクエストの受付状況 |
| サイト公開 | コミット、Actions結果、公開HTMLとローカル成果物の一致 |

Googleの掲載と順位は外部の判断です。登録リクエストは即時掲載の保証ではなく、同じURLへ繰り返し送ってもクロールを早める手段にはなりません。

## 効果を見る

データが蓄積したら、Search Consoleの「検索パフォーマンス」で検索タイプを「ウェブ」にし、「UE5」や「リップシンク」を含む検索語を確認します。表示回数、クリック数、CTR、平均掲載順位を、検索語と掲載ページを組み合わせて見ます。データが十分なら直近28日と前の28日を比較します。新規サイトのデータ不足を掲載不可と断定しません。

- 未登録：URL検査でクロール可否、noindex、canonical、サイトマップの検出状況を確認します。
- 掲載されるが表示が少ない：実際の検索語とページ内容が合っているかを見て、利用者に不足する説明を追加します。
- 表示されるがクリックされない：検索結果のタイトル・説明と記事内容の対応を見直します。

検索結果は環境や時点によって変わるため、単発の手動検索や`site:`検索だけで登録数・順位の成果を確定しません。定期監視や自動公開は追加していません。

## 公式資料

- [Googleのタイトルリンク指針](https://developers.google.com/search/docs/appearance/title-link?hl=ja)
- [Search Consoleの所有権確認](https://support.google.com/webmasters/answer/9008080?hl=ja)
- [サイトマップの作成と送信](https://developers.google.com/search/docs/crawling-indexing/sitemaps/build-sitemap?hl=ja)
- [再クロール・登録リクエスト](https://developers.google.com/search/docs/crawling-indexing/ask-google-to-recrawl?hl=ja)
