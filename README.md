# chnos010
- 川合秀実氏著「30日でできる！OS自作入門」を参考に開発しているOSです。
 - ページング対応、仮想86モードを経由したBIOS呼び出し等を実装しています。
- マルチプラットフォームでの開発に対応
 - 各環境に合わせて、修正版のz_toolsをダウンロードして差し替えるだけで、ソースを修正することなくコンパイル・実行が可能です。
  - [Mac OSX版](https://github.com/HariboteOS/z_tools_osx)
  - [開発環境ソースコード](https://github.com/HariboteOS/tolsrc)

- bochs対応
 - `make brun`で実行できます。
