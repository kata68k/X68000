	.include	IOCSCALL.MAC
	.list

MOON_VERSION	.equ	0
MOON_REGIST		.equ	1
MOON_PLAY		.equ	2
MOON_DATAVER	.equ	3
MOON_GETBUF		.equ	4
	
	.even
	.text

	.even
	.xdef	_MoonRegst,_MoonPlay,_MoonPlay2,_MACS_Play,_MACS_Vsync,_MACS_Vsync_R,_MACS_Sleep

_MoonRegst:
	*【入力】a1.l = ファイル名文字列へのポインタ

	*【出力】d0.l = ステータス
	*		  -1 : 登録用のバッファに空きがない
	*		  -2 : FAT の分断が多すぎる
	*		 -64 : バッファサイズよりファイルが大きい
	*		 -65 : ファイルが見つからない
	*		-128 : ディスク I/O の最中だった
	*		0以上: 正常終了
	
	link	a6,#0
	movem.l	d1/a0-a1,-(sp)

*	lea.l	filename,a1
	movea.l	8(a6),a1
	moveq.l	#MOON_REGIST,d1
	IOCS	_MOON
	tst.l	d0
*	bmi	error
*		:
	movem.l	(sp)+,d1/a0-a1
	unlk	a6
	rts

_MoonPlay:
	*	.MCS を再生する
	*【入力】
	*		a1.l = ファイル名文字列へのポインタ
	*		d3.w = アボードフラグ
	*				既に MACS データが再生中の時にそれを中断して再生するかどうか
	*		d3.w = 0 : 再生しない
	*		d3.w = 1 : 中断して再生
	*
	*		d4.l = 特殊効果フラグ（MACSパラメータ）
	*
	*【出力】
	*		d0.l = ステータス
	*			  -1～-8 : MACS エラーコード
	*				 -64 : バッファサイズよりファイルが大きい
	*				 -65 : ファイルのオープンに失敗
	*				-128 : ディスク I/O の最中だった
	*				0以上: 正常終了
	*
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

*	lea.l	filename,a1
	movea.l	8(a6),a1
	moveq.l	#1,d3			* stop and play
*	moveq.l	#0,d4			* normal play
	moveq.l	#4,d4			* 特殊効果フラグ b0000 0100 
*	moveq.l	#$64,d4			* 特殊効果フラグ b0110 0100 
	moveq.l	#MOON_PLAY,d1
	IOCS	_MOON
	tst.l	d0
*	bmi	error
*			:
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts

_MoonPlay2:
	*	.MCS を再生する
	*【入力】
	*		a1.l = ファイル名文字列へのポインタ
	*		d3.w = アボードフラグ
	*				既に MACS データが再生中の時にそれを中断して再生するかどうか
	*		d3.w = 0 : 再生しない
	*		d3.w = 1 : 中断して再生
	*
	*		d4.l = 特殊効果フラグ（MACSパラメータ）
	*
	*【出力】
	*		d0.l = ステータス
	*			  -1～-8 : MACS エラーコード
	*				 -64 : バッファサイズよりファイルが大きい
	*				 -65 : ファイルのオープンに失敗
	*				-128 : ディスク I/O の最中だった
	*				0以上: 正常終了
	*
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

*	lea.l	filename,a1
	movea.l	8(a6),a1
	moveq.l	#1,d3			* stop and play
*	moveq.l	#0,d4			* normal play
*	moveq.l	#4,d4			* 特殊効果フラグ b0000 0100 
	move.l	12(a6),d4		* 特殊効果フラグ 第二引数
*	moveq.l	#$64,d4			* 特殊効果フラグ b0110 0100 
	moveq.l	#MOON_PLAY,d1
	IOCS	_MOON
	tst.l	d0
*	bmi	error
*			:
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts
	
_MACS_Play:
*	Input:	d1.w > コマンドコード		Output:  d0.l > Status
*		d2.l > バッファサイズ
*		d3.w > アボートフラグ
*		d4.l > 特殊効果フラグ
*		a1.l > 処理アドレス
*	------------------------------------------------------------------------------
*		d1.w = 0	アニメーション再生
*					(a1)から始まるアニメデータを再生します。
*		d2.lは通常-1($FFFFFFFF)にして下さい。
*				（アニメデータ再生中、d2.lのエリアを越えようとした時,再生を中止します）
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

	movea.l	8(a6),a1
	moveq.l	#0,d1			* play
	moveq.l	#-1,d2			* 
	moveq.l	#1,d3			* stop and play
	moveq.l	#4,d4			* 特殊効果フラグ b0000 0100 
*	moveq.l	#$64,d4			* 特殊効果フラグ b0110 0100 
	IOCS	_MACS
	tst.l	d0
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts
	
_MACS_Vsync:
*	Input:	d1.w > コマンドコード		Output:  d0.l > Status
*			d2.w > カウンタ
*			a1.l > 処理アドレス
*
*	d1.w = 10	垂直同期割り込みを登録します
*				a1.lに割り込みルーチンのアドレス、
*				d2.wにフラグ*256+カウンタをセットしてコールします。
*				フラグは現在使用されていません。0にしておいて下さい。

	link	a6,#0
	movem.l	d1-d2/a0-a1,-(sp)

	move.w	sr,-(sp)		* ステータスreg保存
	ori.w	#$0700,sr		* 全割り込み止め

	move.w	#$0*256+$1,d2
	move.w	#10,d1
	movea.l	8(a6),a1
	IOCS	_MACS
*	tst.l	d0

	tst.b	$E9A001		* 8255(ｼﾞｮｲｽﾃｨｯｸ)の空読み
	nop			* 直前までの命令パイプラインの同期
				* 早い話、この命令終了までには
				* それ以前のバスサイクルなどが
				* 完了していることが保証される。
				
	move.w	(sp)+,sr		*割り込み開始

	movem.l	(sp)+,d1-d2/a0-a1
	unlk	a6
	rts

_MACS_Vsync_R:
*	Input:	d1.w > コマンドコード		Output:  d0.l > Status
*			a1.l > 処理アドレス
*
*	d1.w = 11	垂直同期割り込みを解除します
*				a1.lに登録した割り込みルーチンのアドレスをセットして
*				コールします。

	link	a6,#0
	movem.l	d1/a0-a1,-(sp)

	move.w	sr,-(sp)		* ステータスreg保存
	ori.w	#$0700,sr		* 全割り込み止め

	move.w	#11,d1
	movea.l	8(a6),a1
	IOCS	_MACS
*	tst.l	d0

	tst.b	$E9A001		* 8255(ｼﾞｮｲｽﾃｨｯｸ)の空読み
	nop			* 直前までの命令パイプラインの同期
				* 早い話、この命令終了までには
				* それ以前のバスサイクルなどが
				* 完了していることが保証される。
				
	move.w	(sp)+,sr		* 割り込み開始

	movem.l	(sp)+,d1/a0-a1
	unlk	a6
	rts

_MACS_Sleep:
*	Input:	d1.w > コマンドコード		Output:  d0.l > Status
*			d2.w > カウンタ
*
*			d1.w = 13	スリープ状態を設定します
*			d2.wが0なら再生可能状態，0以外ならスリープします。

	link	a6,#0
	movem.l	d1-d2/a0,-(sp)

	move.w	#13,d1
	move.w	#-1,d2
	IOCS	_MACS
*	tst.l	d0

	movem.l	(sp)+,d1-d2/a0
	unlk	a6
	rts

*filename:
*	.dc.b	'ASU_S.MCS',0

	.even
	.end


*------------------------------------------------------------------------------
*特殊効果フラグ (d4.l)
*
*bit 0	ハーフトーン (1の時パレットを50%暗くする)
*bit 1	PCMカット (1の時PCMの再生をしない)
*bit 2	キーセンスアボートOFF (1の時キー入力時終了しない)
*bit 3	画面再初期化抑制フラグ (1の時アニメ終了時画面を初期化しない)
*bit 4	スリープ状態無視 (1の時MACSDRVがスリープしている時でも再生する)
*bit 5	グラフィックＶＲＡＭ表示フラグ (1の時ｸﾞﾗﾌｨｯｸを表示したまま再生する)
*bit 6	スプライト表示フラグ (1の時ｽﾌﾟﾗｲﾄを表示したまま再生する)
*
*bit 7
*  :		Reserved. (必ず0にすること)
*bit31
*
*------------------------------------------------------------------------------
*エラーコード一覧 (d0.l)
*
*-1	MACSデータではありません
*-2	MACSのバージョンが古すぎます
*-3	データがバッファに全部入力されていません
*-4	アボートされました
*-5	既に現在アニメーション再生中です
*-6	無効なファンクションを指定しました
*-7	ユーザーリストもしくは割り込みリストが一杯です
*-8	ユーザーリストもしくは割り込みリストに登録されていません
*==============================================================================
