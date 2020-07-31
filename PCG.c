#ifndef	PCG_C
#define	PCG_C

#include <iocslib.h>

#include "inc/usr_macro.h"
#include "PCG.h"

/* 関数のプロトタイプ宣言 */
void PCG_INIT(void);
void BG_TEXT_SET(SC *);

/* 関数 */
void PCG_INIT(void)
{
	UI i, j;
	
	/* スプライトの初期化 */
	SP_INIT();			/* スプライトの初期化 */
	SP_ON();			/* スプライト表示をＯＮ */
	BGCTRLST(0, 1, 1);	/* ＢＧ０表示ＯＮ */
	BGCTRLST(1, 1, 1);	/* ＢＧ１表示ＯＮ */
//	BGCTRLST(1, 1, 0);	/* ＢＧ１表示ＯＦＦ */
	
	/*スプライトレジスタ初期化*/
	for( j = 0x80000000; j < 0x80000000 + 128; j++ )
	{
		SP_REGST(j,-1,0,0,0,0);
	}
#if 0
	for( i = 0; i < 256; i++ )
	{
		SP_CGCLR(i);			/* スプライトクリア */
		SP_DEFCG( i, 1,  );
	}
#endif
	
	//	BGCTRLGT(1);				/* BGコントロールレジスタ読み込み */
	BGSCRLST(0,0,0);				/* BGスクロールレジスタ設定 */
	BGSCRLST(1,0,0);				/* BGスクロールレジスタ設定 */
	BGTEXTCL(0,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	BGTEXTCL(1,SetBGcode(0,0,0,0));	/* BGテキストクリア */
	//	BGTEXTGT(1,1,0);			/* BGテキスト読み込み */
}

void BG_TEXT_SET(SC *fname)
{
	US	usV_pat;
	UI	pal = 0;
	US	i,j;
	SI	x,y;
	US	map_data[64][64];

	/* マップデータ読み込み */
	File_Load_CSV( fname, map_data, &i, &j);
	
	for(y=0; y<16; y++)
	{
		for(x=0; x<i; x++)
		{
			if(y < 12)		pal = 0x0E;
			else if(x > 31)	pal = 0x0F;
			else 			pal = 0x0E;
			BGTEXTST(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
			BGTEXTST(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
		}
	}

	usV_pat=0;
	pal = 1;
	for(y=16; y<j; y++)
	{
		for(x=0; x<i; x++)
		{
			if(x < 16 || x > 40){
				BGTEXTST(0,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
				BGTEXTST(1,x,y,SetBGcode(0,0,pal,map_data[y][x]));		/* BGテキスト設定 */
			}
			else{	/* 垂直反転 */
				BGTEXTST(0,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BGテキスト設定 */
				BGTEXTST(1,x,y,SetBGcode(0,1,pal,map_data[y][x]));		/* BGテキスト設定 */
			}
		}
		
		usV_pat++;
		if(usV_pat < 12){
		}
		else{
			pal++;
			usV_pat=0;
		}
	}
}

#endif	/* PCG_C */

