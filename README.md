# EU4SpecialEscape
## これはなに？
　Steam配信されているWindows版Europa Universalis IVの日本語化パッチの副産物です。パッチはこのプログラムを使って変換されたテキストのみを受け付けます。

## 環境
　Windows7 以上

## 使いかたパターン1（localisationのymlファイル）
適当なフォルダを用意します。

変換したいテキストが入ったファイルを下記に注意して、上記のフォルダ内に保存してください。上記のフォルダ内にさらにフォルダを作ってその中にテキストを置くような階層構造にしても大丈夫です。

 - BOM付きのUTF-8
 - ファイル名をxxx.utf8b.yyyにする。xxxは任意の文字列。yyyはa～zとA～Zと0～9のみ使用可能

上記のフォルダを、eu4specialescape.exeにドラッグアンドドロップしてください。フォルダ内に、xxx.yyyとして変換されたテキストが保存されます。

## 使い方パターン２（localisationのymlファイル）
適当なフォルダを用意します。

変換したいテキストが入ったファイルを下記に注意して、上記のフォルダ内に保存してください。上記のフォルダ内にさらにフォルダを作ってその中にテキストを置くような階層構造にしても大丈夫です。

 - BOMなしのUTF-8
 - ファイル名をxxx.yyy.utf8にする。xxxは任意の文字列。yyyはa～zとA～Zと0～9とアンダースコア(_)のみ使用可能

上記のフォルダを、eu4specialescape.exeにドラッグアンドドロップしてください。フォルダ内に、xxx.yyyとして変換されたテキストが保存されます。

## 使い方パターン３（ymlファイル以外のファイル。txtファイルなど）
適当なフォルダを用意します。

変換したいテキストが入ったファイルを下記に注意して、上記のフォルダ内に保存してください。上記のフォルダ内にさらにフォルダを作ってその中にテキストを置くような階層構造にしても大丈夫です。

 - BOMなしのUTF-8
 - ファイル名をxxx.yyy.utf8toCP1252にする。xxxは任意の文字列。yyyはa～zとA～Zと0～9とアンダースコア(_)のみ使用可能

上記のフォルダを、eu4specialescape.exeにドラッグアンドドロップしてください。フォルダ内に、xxx.yyyとして変換されたテキストが保存されます。

## ビルドの仕方
　VC2015以上でeu4specialescape.slnを開き、ビルドしてください。
