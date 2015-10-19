#!/bin/sh
FILENAME=mysql

#実行ファイルの削除
rm $FILENAME

#コンパイル
make

#実行
./$FILENAME
