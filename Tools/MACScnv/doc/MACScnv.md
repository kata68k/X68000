# 動画(MACSデータ)を作ってみよう！

MACSとは？
------------

#### MACSDRV.doc 引用

>MACSとはX680x0における'アニメーション'をもっと手軽に扱うために生まれた常駐型の
>アニメーションコントローラです。

## MACSデータ作成補助ツール（MACScnv.x)について
MACSデータは、プログラム(アセンブラ)＋画像データ＋音声データの組み合わせで作成します。
いかにも大変そうなので、MACSデータ作成補助ツール（MACScnv.x)を作りました。

Windows環境で画像データと音声データを抽出した後、X68000にデータをコピーします。
MACSデータ作成補助ツール（MACScnv.x)を起動し、生成されたソースとバッチファイルでMACSデータを作ります。

MACScnv.xは、画像データと音声データを用意すれば、プログラムを吐き出す仕組みになっています。
４つのバッチファイルが生成されるので、順番に実行してMACSデータを作ります。
- 0_MakeTxTp.bat　(画像処理)
- 1_MakePCM.bat　 (音声データ変換)
- 2_MakeBLK.bat　 (データ結合)
- 3_MakeMACS.bat　(アセンブル、リンク、MACSデータ作成)

### 必要な環境
WindowsとX68000間でデータのやり取りができること。
#### Windows
| No.| ツール名            | 概要                                |
|----|---------------------|-------------------------------------|
| 1  | VideoProc Converter | 動画を画像データと音声データに分解(これができるならどのツールでもよい)  |
| 2  | IrfanView           | 複数の画像データを一括で加工できる  |

#### X68000(エミュレータ推奨)
| No.| ツール名         |Version | 概要                                |所在|
|----|------------------|--------|-------------------------------------|----|
| 1  | switch.x         |        | 搭載メモリサイズを変更              |システムディスクに同梱|
| 2  | FLOATx.X         |        | 浮動小数点型(float)が扱える         |システムディスクに同梱|
| 3  | (V)TwentyOne.sys |        | 長いファイル名が扱える              |リンク参照|
| 4  | MACSDRV.x        |        | 常駐型のアニメーションコントローラ  |リンク参照|
| 5  | MakeMCS.x        |        | MACSデータ生成                      |MACS116.LZHに同梱  |
| 6  | txs.x            |        | 画像データをMACS形式で保存          |MACS116.LZHに同梱|
| 7  | blk.x            |        | バイナリを合体させる                |MACS116.LZHに同梱  |
| 8  | APICG.r          |        | 画像ローダー＆セーバー              |リンク参照|
| 9  | YGMC.X           |        | 画面モードを変更する                |どこ？|
| 10 | HAS.x            |        | アセンブラ                          |リンク参照|
| 11 | hlk.r            |3.01+15 | リンカ                              |リンク参照|
| ~~12~~ | ~~CV.X~~             |        | ~~ファイルコンバーター~~                |~~リンク参照~~|
| 13 | pcm3pcm.x        |        | WAVからPCMへ変換する                |どこ？|
| 14 | MACScnv.x        |        | MACSデータ作成補助                  |X68K BBS|

| No.| ファイル名       | 概要                |所在               |
|----|------------------|---------------------|-------------------|
| 1  | MACS_sch.h       | MACS用ヘッダファイル|MACS116.LZHに同梱  |


### リンク

- [システムディスク](http://retropc.net/x68000/software/sharp/human302/):Makoto KamadaさんのX68000 LIBRARYのページ
- [MACSDRV](http://retropc.net/x68000/software/movie/macs/macsmoon/):Makoto KamadaさんのX68000 LIBRARYのページ
- [(V)TwentyOne](http://gorry.haun.org/x68index.html):GORRYさんの「ポリバケツＸ」...X680x0のページ
- [APICG](http://gorry.haun.org/x68index.html):GORRYさんの「ポリバケツＸ」...X680x0のページ
- [HAS](http://http://retropc.net/x68000/software/develop/as/has/):Makoto KamadaさんのX68000 LIBRARYのページ
- [hlk](https://github.com/kg68k/hlk-ev/releases/):立花@桑島技研さんのGitHub
- ~~[CV](http://retropc.net/x68000/software/develop/cv/cv_x/):Makoto KamadaさんのX68000 LIBRARYのページ~~

## 手順

---
### 1. 好きな動画ファイルを準備しよう！<BR>
VideoProc Converterの機能が便利です。<BR>
体験版は、５分までの制約がありますが、X68000では扱いきれないサイズなので問題ありません。(笑)

---
### 2. VideoProc Converterで動画ファイルから画像データ(PNG)を出力しよう！<BR>
「ビデオ」を選択します。<BR>
![静止画](img/GP1.png)<BR>
---
下のメニューから「スナップショット」を選択します。<BR>
![静止画](img/GP2.png)<BR>
---
「画素数」を最大値になるように上げます。※直接入力ではなく、カーソルキー↑、PageUpがオススメ<BR>
![静止画](img/GP3.png)<BR>
---
右下の「出力フォルダ」で適当な場所を選択し、「RUN」を押して画像を抽出します。<BR>
![静止画](img/GP4.png)<BR>
---
PNGファイルが抽出されます。<BR>
![静止画](img/GP5.png)<BR>

---
### 3. VideoProc Converterで動画ファイルから音声データ(WAV)を出力しよう！<BR>
下のメニューから「PCM」を選択します。<BR>
![音声](img/WA1.png)<BR>
---
オプションの変更は不要です。
![音声](img/WA2.png)<BR>
---
WAVファイルが作成されるので、複製を作り「AD.WAV」にファイル名を変更ください。<BR>
![音声](img/WA4.png)<BR>

---
### 4. リサイズ＆減色＆PNG→BMP変換しよう！<BR>
「ファイル」→「一括処理」<BR>
![加工1](img/CG1.png)<BR>
---
「形式設定」→ BMP - Windows Bitmap<BR>
「リネーム設定」→「CG#####」 #は５個<BR>
「ファイル管理フォルダ」 →「変換後のファイルが保存されるところ」<BR>
「ファイルの場所」→「PNGファイルが保存されている場所」<BR>
「ファイルの種類」→ 「PNG - Portable Network Graphics 」<BR>
![加工1](img/CG2.png)<BR>
---
「詳細設定」を押す<BR>
「開始番号」→ 0<BR>
「加算数」 → 1<BR>
![加工1](img/CG3.png)<BR>
---
「追加設定」を押す<BR>
「サイズ変更」にチェックを入れる<BR>
「横サイズ」「縦サイズ」→ 256（MACSデータが256ｘ256なので）<BR>
「アスペクト比を維持する 」にチェックを入れる<BR>
「画像をリサンプルする」にチェックを入れる<BR>
「表示色変更」にチェックを入れる<BR>
「設定の色数へ表示色変更」→8～14    ※注意：15、16にすると後工程のパレット作成で失敗する。<BR>
![加工1](img/CG4.png)<BR>
---
「すべて追加」を押して「作業ファイルの数」に登録する。<BR>
「並べ替え」を押して、「混成の名前」を選択し、ファイル名が１から順番に並ぶようにする。<BR>
![加工1](img/CG5.png)<BR>
---
「実行」を押す。
![加工1](img/CG6.png)<BR>
---
アスペクト比が保持されたBMP画像ができる。<BR>
![加工1](img/CG7.png)<BR>

---
### 5. 空白を埋めて256x256サイズの画像を作ろう！<BR>
256x256サイズが必要なので、高さを拡張する。<BR>
またもや「ファイル」→「一括処理」を選ぶ。<BR>
「ファイル管理フォルダ」 →「変換後のファイルが保存されるところ（X68000側からも見えるところを設定）」<BR>
「ファイルの場所」→「リサイズ、減色したBMPファイルが保存されている場所」<BR>
「ファイルの種類」→ 「BMP - Windows Bitmap」<BR>
「すべて追加」を押して「作業ファイルの数」に登録する。<BR>
![加工2](img/CG8.png)<BR>
---
「追加設定」を押す<BR>
「サイズ変更」にチェックを外す<BR>
「表示色変更」にチェックを外す<BR>
「キャンバスサイズ変更」にチェックを入れる<BR>
「設定確認」を押す
![加工2](img/CG9.png)<BR>
---
「設定確認」を押したら<BR>
「Method1」の「Top side」「Bottom side」を広げる<BR>
今回の例だと 256x192→256x256にサイズを変更するには、<BR>
高さが足りてないので上下32ピクセルずつ増やす。<BR>
「Canvas Color」は、黒にしておきましょう。<BR>
![加工2](img/CG10.png)<BR>
---
「実行」を押す。<BR>
![加工2](img/CG11.png)<BR>
---
256x256サイズで、4BPP(16色以下)の画像が出来上がる。<BR>
![加工2](img/CG12.png)<BR>

---
### 6. MACSデータ作成補助ツール（MACScnv.x)を実行しよう！<BR>
X68000のエミュレータを起動して、変換したBMPが見れるか確認する。<BR>
MACScnv.x、MACS_SCH.h、__Clean.batをコピーしておく。<BR>
変換で必要なツールは、パス設定があるところに配置しておく。<BR>
![MACS](img/MC1.png)<BR>
---
MACSデータ作成補助ツール（MACScnv.x)を実行する。<BR>
下記の例だと<BR>
＜第一引数 開始番号＞ ex. 0ならCG00000が最初の画像になる<BR>
＜第二引数 増加値  ＞ ex. 4ならCG00000、CG00004、CG00008、CG00012と+4ずつ処理する。<BR>
＜第三引数 終了番号＞ ex. 1620ならCG01620まで処理対象となる。<BR>
＜第四引数 動画の再生時間 or 動画の音声の再生時間＞ ex. 1分21秒の動画なら81秒なので81と入力する。<BR>
これにより、1621÷4＝405枚の画像を使った1秒あたり5枚の画像を書き換えるようなMACSデータができます。
![MACS](img/MC2.png)<BR>
バッチファイル４つとバイナリファイルリスト３種類、アセンブラソースが作成される。<BR>
![MACS](img/MC3.png)<BR>
既にファイルが存在する場合は、上書きしません。<BR>
__Clean.batを実行して、生成物を削除してください。
![MACS](img/MC4.png)<BR>
0_MakeTxTp.batを実行して、MACSデータに必要な画像を作ります。
![MACS](img/MC5.png)<BR>
BMP画像をテキスト画像に変換している様子<BR>
![MACS](img/MC5-1.png)<BR>
1_MakePCM.batを実行して、WAVファイルをADPCMのデータに変換します<BR>
![MACS](img/MC6.png)<BR>
変換してる様子<BR>
![MACS](img/MC6-1.png)<BR>
2_MakeBLK.batを実行して、バイナリファイル結合をします<BR>
![MACS](img/MC7.png)<BR>
ワーニングメッセージは気にしなくてよい感じ<BR>
![MACS](img/MC7-1.png)<BR>
3_MakeMACS.batを実行して、アセンブル、リンク(コンバート)、MACSデータ化を行います<BR>
![MACS](img/MC8.png)<BR>
Out of memoryが出ると失敗です<BR>
メモリが足りないこと~~、もしくは、ファイルサイズが8MBに収まってないこと~~が原因です。<BR>
対策１：常駐物を減らしてメモリの空き領域を増やす<BR>
対策２：MACScnv.xの第二引数の数値を見直しフレームレートを落とす<BR>
対策３：3_MakeMACS.bat内のhlk.rをハイメモリで実行する。※8MB以上のMACSデータを作る場合必須<BR>
<BR>
 ＜ハイメモリを使い方＞<BR>
  3_MakeMACS.batを機種にあわせて<font color="Red">**太字**</font>を追記編集ください<BR>
 * Xellent30の場合 + 拡張メモリの場合<BR>
<font color="Red">**検証中（できないかも）**</font><BR>
 * X68030 + 拡張メモリの場合<BR>
<font color="Red">**検証中（できないかも）**</font><BR>
 * 040 turboの場合<BR>
<font color="Red">**検証中（できないかも）**</font><BR>
 * 060 turboの場合<BR>
`has -u MACSsrc`<BR>
<font color="Red">**`060high.x 1`**</font><BR>
<font color="Red">**`060loadhigh.x`**</font> `hlk -r MACSsrc LIST0_Tx (中略) LIST_PCM`<BR>
<font color="Red">**`060high.x 0`**</font><BR>
`MakeMCS MACSsrc`<BR>

上記対策で、解決しない場合は、__Clean.batを実行してX68000で行う作業から、リトライしましょう！（これが辛い…）<BR>
![MACS](img/MC8-1.png)<BR>

---
### 6. 再生してみよう！<BR>
MACSDRV.xを常駐させた後に、コマンドラインで以下を実行してください。<BR>
※ファイル名（MACSsrc.MCS）は、適当にリネームしておくといいでしょう。<BR>

\>copy MACSsrc.MCS MACS<BR>
※メインメモリ以上のMACSデータを再生する場合は、MACSplay.xをお使いください。<BR>
![MACS](img/MC9.png)<BR>

動画が再生できたら完成です！
![MACS](img/MC9-1.png)<BR>
---
# おわり