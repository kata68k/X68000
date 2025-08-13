#ifndef	IF_FILEMANAGER_C
#define	IF_FILEMANAGER_C

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <io.h>
#include <time.h>
#include <doslib.h>
#include <iocslib.h>

#include <usr_macro.h>

#include "IF_FileManager.h"

#include "BIOS_PCG.h"
#include "IF_Input.h"
#include "IF_MUSIC.h"
#include "IF_Memory.h"
#include "IF_PCG.h"
#include "IF_Text.h"

/* グローバル変数 */
int8_t		g_data_list[DATA_MAX][256]	=	{0};
uint32_t	d_list_max	=	0u;
int8_t		g_data_list_sel	=	0u;
//static int32_t		nMaxFreeMem;

/* 関数のプロトタイプ宣言 */
int16_t Init_FileList_Load(void);
size_t File_Load(int8_t *, void *, size_t, size_t);
int16_t File_Save(int8_t *, void *, size_t, size_t);
int16_t File_Load_CSV(int8_t *, uint16_t *, uint16_t *, uint16_t *);
int16_t PCG_SP_dataload(int8_t *);
int16_t PCG_PAL_dataload(int8_t *);
int16_t PCG_MAP_dataload(int8_t *);
int16_t Load_Music_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_SE_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_CG_List(	int8_t *, int8_t *, CG_LIST *, uint32_t *);
int16_t Load_MACS_List(	int8_t *, int8_t *, MOV_LIST *, uint32_t *);
int16_t Load_DATA_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_SP_List(int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t GetFileLength(int8_t *, int32_t *);
int16_t GetFilePICinfo(int8_t *, BITMAPINFOHEADER *);
int16_t GetRectangleSise(uint16_t *, uint16_t, uint16_t, uint16_t);
int16_t	Get_DataList_Num(uint16_t *);
int16_t	Set_DataList_Num(uint16_t);
int8_t *Get_DataList_Path(void);
int16_t Get_FileAlive(const int8_t *);
int16_t Get_ZPDFileName(const int8_t *, int8_t *);

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Init_FileList_Load(void)
{
	int16_t ret = 0;
	uint8_t	sListFilePath[255] = {0};
	uint8_t	sListFileName[255] = {0};
	uint32_t	i;
	int16_t ofst_y = 2;
	
	/* データリスト有無 */
	if(GetFileLength("data\\datalist.txt", &i) == -1)
	{
		strcpy( g_data_list[0], "data" );
		Set_DataList_Num(0);
		d_list_max = 1;
	}
	else
	{
		Load_DATA_List("data\\", "datalist.txt", g_data_list, &d_list_max);

		if(d_list_max <= 1)
		{
			ret = 0;
		}
		else
		{
			static int8_t s_bFlagInput = FALSE;
			static int8_t s_bFlagInputAB = FALSE;

			T_Clear();	/* テキスト画面クリア */

			_iocs_b_locate(0, 0);	/*  原点 */

			puts("==============< 起動メニュー >==============================================");
			puts("==============< datalist.txt >=============================================");
			for(i = 0; i < d_list_max; i++)
			{
				if(ret == i)
				{
					_iocs_b_putmes( 10, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
				}
				else
				{
					_iocs_b_putmes( 3, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
				}
			}
			_iocs_b_locate(0, i + ofst_y);	/* 移動 */
			puts("===========================================================================");
			puts("==============< 上下で選択 Aボタンで決定 >===================================");

			do
			{
				Input_Main(FALSE);	/* 入力受付 */

				/* 選択 */
				if( ( GetInput() & KEY_UPPER ) != 0) { /* 上 */
					if(s_bFlagInput == FALSE)
					{
						ret = Mdec(ret, 1);
						s_bFlagInput = TRUE;

						/* クリップ処理 */
						if(ret < 0)
						{
							ret = 0;
						}
						else if(ret >= d_list_max)
						{
							ret = d_list_max - 1;
						}
						else
						{
							/* 何もしない */
						}
						for(i = 0; i < d_list_max; i++)
						{
							if(ret == i)
							{
								_iocs_b_putmes( 10, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
							}
							else
							{
								_iocs_b_putmes( 3, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
							}
						}
					}
				}
				else if( ( GetInput() & KEY_LOWER ) != 0 ) { /* 下 */
					if(s_bFlagInput == FALSE)
					{
						ret = Minc(ret, 1);
						s_bFlagInput = TRUE;

						/* クリップ処理 */
						if(ret < 0)
						{
							ret = 0;
						}
						else if(ret >= d_list_max)
						{
							ret = d_list_max - 1;
						}
						else
						{
							/* 何もしない */
						}
						for(i = 0; i < d_list_max; i++)
						{
							if(ret == i)
							{
								_iocs_b_putmes( 10, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
							}
							else
							{
								_iocs_b_putmes( 3, 0,	i + ofst_y, strlen(&g_data_list[i][0]), &g_data_list[i][0]);
							}
						}
					}
				}
				else /* なし */
				{
					s_bFlagInput = FALSE;
				}
				
				/* 決定 */
				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* スペースキー */
				{
					if(s_bFlagInputAB == FALSE)
					{
						s_bFlagInputAB = TRUE;

						break;
					}
				}
				else /* なし */
				{
					s_bFlagInputAB = FALSE;
				}

			}
			while(1);

			for(i = 0; i < 1000; i++)
			{
				if((i % 2) == 0)
				{
					_iocs_b_putmes( 10 , 0,	ret + ofst_y, strlen(&g_data_list[ret][0]), &g_data_list[ret][0]);
				}
				else
				{
					_iocs_b_putmes(  2 , 0,	ret + ofst_y, strlen(&g_data_list[ret][0]), &g_data_list[ret][0]);
				}
			}
		}

		Set_DataList_Num(ret);
	}
	printf("Input Data Number [0 - %d] = %d\n", d_list_max - 1, ret);
#ifdef DEBUG
	printf("Load_DATA_List=%s(%d)\n", Get_DataList_Path(), ret);
//	printf("Load_DATA_List=0x%p\n", Get_DataList_Path());
//	KeyHitESC();	/* デバッグ用 */
#endif
	printf("Load_DATA_List Max %d\n", d_list_max);

	/* グラフィック */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "cg");
	Load_CG_List(sListFilePath, "g_list.txt", g_stCG_LIST, &g_CG_List_Max);			/* グラフィックリストの読み込み */
	for(i = 0; i < g_CG_List_Max; i++)
	{
		printf("PIC File %2d = %s\n", i, g_stCG_LIST[i].bFileName);
	}
	printf("PIC File Max %d\n", g_CG_List_Max);

	/* 動画(MACS) */
#if CNF_MACS	
 #ifdef 	MACS_MOON
 	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "mov");
	Load_MACS_List(sListFilePath, "mov_list.txt", &g_mov_list[0], &g_mov_list_max);	/* 動画(MACS)リストの読み込み */
	for(i = 0; i < g_mov_list_max; i++)
	{
		printf("MACS File %2d = %s\n", i, g_mov_list[i]);
	}
	printf("MACS File Max %d\n", g_mov_list_max);
 #endif
#endif

	/* BGM */
#if		ZM_V2 == 1
	strcpy( sListFileName, "m_list.txt" );
#elif	ZM_V3 == 1
	strcpy( sListFileName, "m_list_V3.txt" );
#elif	MC_DRV == 1
	strcpy( sListFileName, "m_list_MC.txt" );
#else
	#error "No Music Lib"
#endif
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "music");
	Load_Music_List(sListFilePath, sListFileName, music_list, &m_list_max);
	for(i = 0; i < m_list_max; i++)
	{
		printf("Music File %2d = %s\n", i, music_list[i]);
	}
	printf("Music File Max %d\n", m_list_max);
	
	/* 効果音(FM) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "seFM");
#if		ZM_V2 == 1
#elif	ZM_V3 == 1
	Load_SE_List(sListFilePath, "s_list_V3.txt", se_list, &s_list_max);
	for(i = 0; i < s_list_max; i++)
	{
		printf("SE FM File %2d = %s\n", i, se_list[i]);
	}
#elif	MC_DRV == 1
#else
	#error "No Music Lib"
#endif
	
	/* 効果音(ADPCM) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "se");
	Load_SE_List(sListFilePath, "p_list.txt", adpcm_list, &p_list_max);
	for(i = 0; i < p_list_max; i++)
	{
		printf("ADPCM File %2d = %s\n", i, adpcm_list[i]);
	}
	printf("ADPCM File Max %d\n", p_list_max);
	
	/* スプライト(PCG) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "sp");
	Load_SP_List(sListFilePath, "sp_list.txt", g_sp_list, &g_sp_list_max);
	for(i = 0; i < g_sp_list_max; i++)
	{
		printf("PCG File %2d = %s\n", i, g_sp_list[i]);
	}
	printf("PCG File Max %d\n", g_sp_list_max);

	/* スプライト(PAL) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "sp");
	g_pal_list_max = 1;
	Load_List(sListFilePath, "pal_list.txt", g_pal_list, &g_pal_list_max);
	for(i = 0; i < g_pal_list_max; i++)	/* 0はテキスト兼用パレットなので1スタート */
	{
		printf("PAL File %2d = %s\n", i+1, g_pal_list[i+1]);
	}
	printf("PAL File Max %d\n", g_pal_list_max);

	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* ファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
size_t File_Load(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	size_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* ファイルが読み込めません */
		printf("ERR:%sファイルが見つかりません！\n", fname);
		ret = -1;
	}
	else
	{
		/* データ個数を指定しない場合 */
		if(n == 0)
		{
			/* ファイルサイズを取得 */
			n = filelength(fileno(fp));
		}
		
		/* ファイル読み込み */
		ret = fread (ptr, size, n, fp);

		if (ret != n) {
	        if (feof(fp)) {
				fprintf(stderr, "Premature end of file reached\n");
			} else if (ferror(fp)) {
				perror("Error while reading from file");
			}
		}

		/* ファイルを閉じる */
		fclose (fp);
	}

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* ファイル保存 */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
int16_t File_Save(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)	/* ファイルが無い */
	{
		/* ファイルを開ける */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	else
	{
		/* ファイルが存在する場合は何もしない */
	}
	/* ファイルを閉じる */
	fclose (fp);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* ファイル保存 */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* size		データのサイズ */
/* n		データの個数 */
int16_t File_Save_OverWrite(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* ファイルを開ける */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)	/* ファイルが無い */
	{
		/* ファイルを開ける */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	else
	{
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	/* ファイルを閉じる */
	fclose (fp);

	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* CSVファイル読み込み */
/* *fname	ファイル名 */
/* *ptr		格納先の先頭アドレス */
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
int16_t File_Load_CSV(int8_t *fname, uint16_t *ptr, uint16_t *Col, uint16_t *Row)
{
	FILE *fp;
	int16_t ret = 0;
	uint16_t x, y, flag, cnv_flag;
	char buf[1024], *p, *end;
	
	x = 0;
	y = 0;
	flag = 0;
	cnv_flag = 0;
	
	fp = fopen(fname, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			do
			{
				int64_t num = strtol(p, &end, 0);

//				printf("(%d,%d)=%d ->%s\n", x, y, num, end);

				if(*end == '\0')
				{
					/* 文字列の終わりだった */
					if(flag==0)
					{
						*Col = x;
					}
					flag = 1;
					break;
				}
				else if(p != end)
				{
					/* 変換できた */
					*ptr = (uint16_t)num;
					ptr++;

					p = end + 1;
					
					cnv_flag = 1;
					x++;
				}
				else
				{
					/* 変換できなかった */
					p = end + 1;
				}
			}
			while(1);
			
			if(cnv_flag != 0){
				cnv_flag = 0;
				y++;
			}
			x = 0;

			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	
	*Row = y;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
/* CSV コースデータファイル読み込み */
/* *fname	ファイル名 */
/* *st_ptr	格納先の先頭アドレス */
/* *Col		データの行数のアドレス */
/* *Row		データの列数のアドレス */
#if 0
int16_t File_Load_Course_CSV(int8_t *fname, ST_ROADDATA *st_ptr, uint16_t *Col, uint16_t *Row)
{
	FILE *fp;
	int16_t ret = 0;
	uint16_t x, y, flag, cnv_flag;
	char buf[1024], *p, *end;
	
	x = 0;
	y = 0;
	flag = 0;
	cnv_flag = 0;
	
	fp = fopen(fname, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			do
			{
				int64_t num = strtol(p, &end, 0);

//				printf("(%d,%d)=%d ->%s\n", x, y, num, end);

				if(*end == '\0')
				{
					/* 文字列の終わりだった */
					st_ptr++;
					if(flag==0)
					{
						*Col = x;
					}
					flag = 1;
					break;
				}
				else if(p != end)
				{
					/* 変換できた */
					switch(x)
					{
						case 0:
						{
							st_ptr->bHeight = (uint8_t)num;
							break;
						}
						case 1:
						{
							st_ptr->bWidth = (uint8_t)num;
							break;
						}
						case 2:
						{
							st_ptr->bAngle = (uint8_t)num;
							break;
						}
						case 3:
						{
							st_ptr->bfriction = (uint8_t)num;
							break;
						}
						case 4:
						{
							st_ptr->bPat = (uint8_t)num;
							break;
						}
						case 5:
						{
							st_ptr->bObject = (uint8_t)num;
							break;
						}
						case 6:
						{
							st_ptr->bRepeatCount = (uint8_t)num;
							break;
						}
						default:
						{
							break;
						}
					}

					p = end + 1;
					
					cnv_flag = 1;
					x++;
				}
				else
				{
					/* 変換できなかった */
					p = end + 1;
				}
			}
			while(1);
			
			if(cnv_flag != 0){
				cnv_flag = 0;
				y++;
			}
			x = 0;
		}
		fclose(fp);
	}
	
	*Row = y;
	
	return ret;
}
#endif

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_SP_dataload(int8_t *fname)
{
	int16_t ret = 0;
	uint8_t *pcg_dat;		/* ＰＣＧデータファイル読み込みバッファ */
	
	FILE *fp;

	pcg_dat = (uint8_t *)MyMalloc(PCG_MAX * SP_16_SIZE);

	/*-----------------[ ＰＣＧデータ読み込み ]-----------------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		uint32_t i, j;
		size_t	pcg_size;
		uint32_t pat;
		pcg_size = filelength( fileno( fp ) );
		pcg_size = Mmin(pcg_size, PCG_MAX * SP_16_SIZE);	/* BGの設定状態でサイズが異なる？？ */

		pat = pcg_size / SP_16_SIZE;
		
		j = fread( pcg_dat
			,  SP_16_SIZE				/* 1PCG = 128byte */
			,  pat	/* PCG */
			,  fp
			) ;
		fclose( fp ) ;

		for( i = 0; i < pat; i++ )
		{
			_iocs_sp_defcg( i, 1, pcg_dat );
			pcg_dat = pcg_dat + (i * SP_16_SIZE);
		}
	}

	MyMfree(pcg_dat);
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_PAL_dataload(int8_t *fname)
{
	int16_t ret = 0;
	uint16_t	pal_dat[ 128 ];	/* パレットデータファイル読み込みバッファ */


	FILE *fp;
	uint32_t i;

	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		fread( pal_dat
			,  2		/* 1palet = 2byte */
			,  256		/* 16palet * 16block */
			,  fp
			) ;
		fclose( fp ) ;

		/* スプライトパレットに転送 */
		for( i = 0 ; i < 256 ; i++ )
		{
			_iocs_spalet( (i&15) | (1<<0x1F) , Mdiv16(i)+1 , pal_dat[i] ) ;
		}
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t PCG_MAP_dataload(int8_t *fname)
{
	int16_t ret = 0;
	uint16_t	map_dat[ 4096 ];	/* MAPデータファイル読み込みバッファ */

	FILE *fp;
	uint16_t x, y;
	uint16_t dat;

	/*--------[ スプライトパレットデータ読み込みと定義 ]--------*/

	fp = fopen( fname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		fseek( fp, 0x1Eu, SEEK_SET);
		fread( map_dat
			,  2		/* data */
			,  4096		/* 64 x 64 */
			,  fp
			) ;
		fclose( fp ) ;

		for (y = 0; y < 64; y++)
		{
			for (x = 0; x < 64; x++)
			{
				dat = map_dat[y*64 + x];
//				_iocs_bgtextst( 0, x, y, dat );		/* BGテキスト設定 */
				_iocs_bgtextst( 1, x, y, dat );		/* BGテキスト設定 */
			}
	    }
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_Music_List(int8_t *fpath, int8_t *fname, int8_t (*music_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				sscanf(p,"%d = %s", &num, z_name);
				if(i == num)
				{
					sprintf(music_list[i], "%s%s", fpath, z_name);
				}
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_SE_List(int8_t *fpath, int8_t *fname, int8_t (*music_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;

		memset(buf, 0, sizeof(buf));
		
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				sscanf(p,"%d = %s", &num, z_name);
				if(i == num)
				{
					sprintf(music_list[i], "%s%s", fpath, z_name);
				}
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_SP_List(int8_t *fpath, int8_t *fname, int8_t (*list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	uint16_t pal, width, height, anime;
	int16_t hit_x, hit_y, hit_width, hit_height;

	sprintf(z_name, "%s%s", fpath, fname);
	
//	printf("%s,%s,%s\n", z_name, fpath, fname);

	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				pal 	= 0;
				width 	= 0;
				height 	= 0;
				anime 	= 0;
				hit_x	= 0;
				hit_y	= 0;
				hit_width	= 0;
				hit_height	= 0;

				sscanf(p,"%d = %s %hd %hd %hd %hd %hd %hd %hd %hd", &num, z_name, &pal, &width, &height, &anime, &hit_x, &hit_y, &hit_width, &hit_height);
#ifdef DEBUG
//				printf("%d = %s %hd %hd %hd %hd %d %d %d %d\n", num, z_name, pal, width, height, anime, hit_x, hit_y, hit_width, hit_height);
//				KeyHitESC();	/* デバッグ用 */
#endif
				if(i == num)
				{
					sprintf(list[i], "%s%s", fpath, z_name);
					if(pal != 0)
					{
						g_stST_PCG_LIST[i].Pal = (uint8_t)pal;
					}
					else
					{
						g_stST_PCG_LIST[i].Pal = 1;
					}

					if(width != 0)
					{
						g_stST_PCG_LIST[i].Pat_w = (uint8_t)width;
					}
					else
					{
						g_stST_PCG_LIST[i].Pat_w = 1;
					}

					if(height != 0)
					{
						g_stST_PCG_LIST[i].Pat_h = (uint8_t)height;
					}
					else
					{
						g_stST_PCG_LIST[i].Pat_h = 1;
					}
					if(anime != 0)
					{
						g_stST_PCG_LIST[i].Pat_AnimeMax = (uint8_t)anime;
					}
					else
					{
						g_stST_PCG_LIST[i].Pat_AnimeMax = 1;
					}
					/*------------------------------------------------------------------------------*/
					if(hit_x != 0)
					{
						g_stST_PCG_LIST[i].hit_x = hit_x;
					}
					else
					{
						g_stST_PCG_LIST[i].hit_x = 0;
					}

					if(hit_y != 0)
					{
						g_stST_PCG_LIST[i].hit_y = hit_y;
					}
					else
					{
						g_stST_PCG_LIST[i].hit_y = 0;
					}

					if(hit_width != 0)
					{
						g_stST_PCG_LIST[i].hit_width = hit_width;
					}
					else
					{
						g_stST_PCG_LIST[i].hit_width = Mmul16(width);
					}

					if(hit_height != 0)
					{
						g_stST_PCG_LIST[i].hit_height = hit_height;
					}
					else
					{
						g_stST_PCG_LIST[i].hit_height = Mmul16(height);
					}
#ifdef DEBUG
//					printf("%d = %s %hd %hd %hd %hd %hd %hd %hd %hd\n", num, z_name,
//					 g_stST_PCG_LIST[i].Pal, 
//					 g_stST_PCG_LIST[i].Pat_w, 
//					 g_stST_PCG_LIST[i].Pat_h, 
//					 g_stST_PCG_LIST[i].Pat_AnimeMax, 
//					 g_stST_PCG_LIST[i].hit_x, 
//					 g_stST_PCG_LIST[i].hit_y, 
//					 g_stST_PCG_LIST[i].hit_width, 
//					 g_stST_PCG_LIST[i].hit_height);
//					KeyHitESC();	/* デバッグ用 */
#endif
				}
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_List(int8_t *fpath, int8_t *fname, int8_t (*list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, j=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = j = *list_max;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				memset(z_name, 0, sizeof(z_name));
				sscanf(p,"%d = %s", &num, z_name);
				if(i == num)
				{
					sprintf(list[i], "%s%s", fpath, z_name);
				}
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i - j;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_CG_List(int8_t *fpath, int8_t *fname, CG_LIST *cg_list, uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	int32_t nType = 0, nTransPal = 0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				nType = 0;
				nTransPal = 0;
				memset(z_name, 0, sizeof(z_name));
				sscanf(p,"%d= %s %d %d", &num, z_name, &nType, &nTransPal);	/* format:番号 = ファイルパス＆ファイル名,グラフィックのタイプ,透過色のパレット番号 */
#if 0
				FILE *fp_d;
				
				p = strtok(z_name, ".");
				sprintf(z_name, "%s.GRP", p, );
				fp_d = fopen(z_name, "r");
				if(fp_d == NULL)
				{
				}
#endif
			
#ifdef DEBUG
//				printf("%d=%s,%d\n", num, z_name, bType);
//				KeyHitESC();	/* デバッグ用 */
#endif
				if(i == num)
				{
					sprintf( cg_list->bFileName, "%s%s", fpath, z_name );
					cg_list->bType = (int8_t)nType;
					cg_list->TransPal = (int16_t)nTransPal;
				}
				cg_list++;
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_MACS_List(int8_t *fpath, int8_t *fname, MOV_LIST *mov_list, uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	int32_t nGR = 0, nSP = 0, nKey = 0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				nGR = 0;
				nSP = 0;
				nKey = 0;
				memset(z_name, 0, sizeof(z_name));
				sscanf(p,"%d= %s %d %d %d", &num, z_name, &nGR, &nSP, &nKey);	/* format:番号 = ファイルパス＆ファイル名,グラフィック,スプライト */
				if(i == num)
				{
					sprintf( mov_list->bFileName, "%s%s", fpath, z_name );
					(mov_list)->bGR = (int8_t)nGR;
					(mov_list)->bSP = (int8_t)nSP;
					(mov_list)->bKey = (int8_t)nKey;
				}
				mov_list++;
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Load_DATA_List(int8_t *fpath, int8_t *fname, int8_t (*data_list)[256], uint32_t *list_max)
{
	FILE *fp;
	int16_t ret = 0;
	int8_t buf[256], *p;
	int8_t z_name[256];
	uint32_t i=0, num=0;
	
	sprintf(z_name, "%s%s", fpath, fname);
	fp = fopen(z_name, "r");
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		i = 0;
		
		memset(buf, 0, sizeof(buf));

		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			int32_t size;
			size = strlen(buf);

	        p = buf;
			if(*p == ';')	/* コメント */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				memset(z_name, 0, sizeof(z_name));
				sscanf(p,"%d = %s", &num, z_name);
				if(i == num)
				{
					sprintf(data_list[i], "%s%s", fpath, z_name);
//					printf("No.(%d) = %s\n", i, z_name);
				}
				i++;
			}
			memset(buf, 0, sizeof(buf));
		}
		fclose(fp);
	}
	*list_max = i;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetFileLength(int8_t *pFname, int32_t *pSize)
{
	FILE *fp;
	int16_t ret = 0;
	int32_t	Tmp;

	fp = fopen( pFname , "rb" ) ;
	if(fp == NULL)		/* Error */
	{
		ret = -1;
	}
	else
	{
		Tmp = fileno( fp );
		if(Tmp == -1)	/* Error */
		{
			*pSize = 0;
			ret = -1;
		}
		else
		{
			Tmp = filelength( Tmp );
			if(Tmp == -1)		/* Error */
			{
				*pSize = 0;
				ret = -1;
			}
			else
			{
				*pSize = Tmp;
//				printf("GetFileLength = (%4d, %4d)\n", *pSize, Tmp );
			}
		}

		fclose( fp ) ;
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetFilePICinfo(int8_t *pFname, BITMAPINFOHEADER *info)
{
	FILE *fp;
	int16_t ret = 0;
	uint8_t	bFlag = FALSE;
	uint8_t	*pBuf;
	uint8_t	sBuf[128] = {0};
	uint16_t	uWord;
	uint32_t	i;

	fp = fopen( pFname , "rb" ) ;
	if(fp == NULL)
	{
		ret = -1;
	}
	else
	{
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* PIC */
		fread( pBuf, sizeof(uint8_t), 3, fp );
		if(strncmp( pBuf, "PIC", 3 ) == 0)
		{
			/* 一致 */
//			printf("%sファイルです\n", pBuf );
			bFlag = TRUE;
		}
		else
		{
			bFlag = FALSE;
			printf("error:PICファイルではありません = %s\n", pBuf );
		}
		
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
		pBuf = &sBuf[0];	/* 先頭アドレス */

		/* 拡張ヘッダか？ */
		fread( pBuf, sizeof(uint8_t), 3, fp );
		if(strncmp( pBuf, "/MM", 3 ) == 0)
		{
			/* 一致 */
//			printf("拡張ファイルです\n", pBuf );

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			for(i=0; i < 128; i++)
			{
				/* 拡張ヘッダ判定 */
				fread( pBuf, sizeof(uint8_t), 1, fp );
				if(strncmp( pBuf, "/", 1 ) == 0)
				{
					*pBuf = 0x00;
//					printf("%s\n", &sBuf[0]);
					
					memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
					pBuf = &sBuf[0];	/* 先頭アドレス */

					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* 拡張ヘッダ終了判定 */
					if(strncmp( &sBuf[0], ":", 1 ) == 0)
					{
						break;
					}
					else
					{
						pBuf++;
					}
					
					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* 画像データの作者判定 */
					if(strncmp( &sBuf[0], "AU", 2 ) == 0)
					{
//						printf("画像データの作者:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
						pBuf = &sBuf[0];	/* 先頭アドレス */
					}
					/* 画像のロード座標 */
					else if(strncmp( &sBuf[0], "XY", 2 ) == 0)
					{
//						printf("XY:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
						pBuf = &sBuf[0];	/* 先頭アドレス */
					}
					else
					{
						pBuf++;
					}
				}
				else
				{
					pBuf++;
				}
			}
			
			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */

			/* コメント終了まで読み込む */
			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
				if(*pBuf == 0x1Au)
				{
					if(i != 0u)
					{
//						printf("コメント(%d)：%s\n", i, sBuf[0] );
					}
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}
		else
		{
			/* 普通のヘッダファイルでした */
			pBuf += 3;
			bFlag = FALSE;

			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
				if(*pBuf == 0x1Au)
				{
//					printf("コメント(%d)：%s\n", i, sBuf[0] );
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}

		/* ダミー */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* バッファクリア */
			pBuf = &sBuf[0];	/* 先頭アドレス */
			
			while(1)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
				if(*pBuf == 0x00u)
				{
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}

		/* 予約文字(0) */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
				bFlag = TRUE;
			}
		}

		/* タイプ・モード */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
//				printf("Type/Mode：%d\n", *pBuf );
				bFlag = TRUE;
			}
		}
		
		if(bFlag == TRUE)
		{
			/* 色のビット数 */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biBitCount = uWord;
//			printf("color bit：%d\n", uWord );

			/* Ｘ方向のサイズ */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biWidth	= (int64_t)uWord;
//			printf("X,Y：(%4d,", uWord );

			/* Ｙ方向のサイズ */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biHeight	= (int64_t)uWord;
//			printf("%4d)\n", uWord );
		}

		fclose( fp ) ;
	}
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	GetRectangleSise(uint16_t *uSize, uint16_t uWidth, uint16_t uHeight, uint16_t uMaxSize)
{
	int16_t ret = 0;
	
	*uSize = Mmax( ((((uWidth+ 7u) & 0xFFF8u) * uHeight) << uMaxSize),  (512u << uMaxSize) );
	
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Get_DataList_Num(uint16_t *p_uNum)
{
	int16_t ret = 0;
	
	*p_uNum = g_data_list_sel;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t	Set_DataList_Num(uint16_t uNum)
{
	int16_t ret = 0;
	
	g_data_list_sel = uNum;
	
	return ret;
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int8_t *Get_DataList_Path(void)
{
	return &g_data_list[g_data_list_sel][0];
}
/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Get_FileAlive(const int8_t *sFile)
{
	int16_t ret = 0;
    // ファイルを読み込みモードで開く
    FILE *fp;
	fp = fopen(sFile, "r");
    // ファイルが存在するかどうかを確認
    if (fp) {
 //       printf("ファイル '%s' は存在します。\n", sFile);
        fclose(fp); // ファイルを閉じる
    } else {
//        printf("ファイル '%s' は存在しません。\n", sFile);
		ret = -1;
    }
	return ret;
}

/*===========================================================================================*/
/* 関数名	：	*/
/* 引数		：	*/
/* 戻り値	：	*/
/*-------------------------------------------------------------------------------------------*/
/* 機能		：	*/
/*===========================================================================================*/
int16_t Get_ZPDFileName(const int8_t *sFile, int8_t *sZpdFile)
{
	int16_t found_63 = 0;	/* c */
	int16_t found_00 = 0;	/* null */
	uint8_t	buffer[1];
	size_t i=0;
    size_t capacity = 1024; // バッファの初期容量
    size_t size = 0; // 現在のデータのサイズ
    uint8_t *data;
    FILE *file;

	file = fopen(sFile, "rb"); // バイナリモードでファイルを開く
    if (file == NULL)
	{
        return 1;
    }

    // 0x63を探す
    while (!feof(file))
	{
        if (fread(buffer, sizeof(uint8_t), 1, file) != 1)
		{
            break;
        }
        if (buffer[0] == 0x63)
		{
            found_63 = 1;
            break;
        }
		i++;
		if(i >= 100)
		{
            break;
		}
    }

    // 0x63が見つからなかった場合
    if (found_63 == 0)
	{
//        printf("Pattern 0x63 not found.\n");
        fclose(file);
        return 2;
    }

    // 次のバイトデータが0x2Eを探す
    data = (uint8_t *)malloc(capacity);
    if (data == NULL)
	{
        fclose(file);
        return 1;
    }

	i = 0;

    // データを読み込みながら0x2Eを探す
    while (!feof(file)) {
        if (fread(buffer, sizeof(uint8_t), 1, file) != 1) {
            break;
        }
        if (buffer[0] == 0x00) {	/* 終端を探す */
            found_00 = 1;
            break;
        }
        // データをバッファに追加
        if (size >= capacity) {
			uint8_t *new_data;
            capacity *= 2; // バッファの容量を倍に拡張
            new_data = (unsigned char *)realloc(data, capacity);
            if (new_data == NULL) {
                fclose(file);
                free(data);
                return 1;
            }
            data = new_data;
        }
        data[size++] = buffer[0];
		i++;
		if(i >= 64)
		{
            break;
		}
    }

    // 0x00が見つからなかった場合
    if (found_00 == 0) {
        fclose(file);
        free(data);
        return 2;
    }

    // 見つかったデータを表示
    for (i = 0; i < size; ++i) {
        *sZpdFile = data[i];
		sZpdFile++;
    }

    // ファイルを閉じる
    fclose(file);
    // メモリを解放
    free(data);

    return 0;
}

#endif	/* IF_FILEMANAGER_C */

