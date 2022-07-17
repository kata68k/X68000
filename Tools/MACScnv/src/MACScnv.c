#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../../../inc/usr_style.h"
#include "../../../inc/usr_macro.h"
#include "../../../inc/usr_define.h"

#define VSYNC_sec	(0.01803177)/*(1.0f/55.45767f)*/

void HelpMessage(void);
void HelpMessage2(void);
int16_t CGcnv(int16_t, int16_t, int16_t);
int16_t BLTcnv(int16_t, int16_t, int16_t, int16_t);
int16_t MACScnv(int16_t, int16_t, int16_t, int16_t, int16_t);
int16_t main(int16_t, int8_t**);
int32_t g_nEffect = 0;
int32_t g_nEffectType = 0;
int32_t g_nEffectFixedWait = 0;

void HelpMessage(void)
{
	printf("MACScnv.x <開始番号> <増加値> <終了番号> <動画再生時間[s] or PCMの時間[s]>\n");
	printf("ex. >MACScnv.x 0 4 1620 81\n");
	printf("------------------------------\n");
	printf("変換に必要なツール＆ファイル：\n");
	printf("=< 0_MakeTxTp.bat >=============\n");
	printf("    画像表示　　　　：APICG.r\n");
	printf("    画面モード変更　：YGMC.x\n");
	printf("    テキスト画面保存：txs.x\n");
	printf("=< 1_MakePCM.bat >==============\n");
	printf("    Wav to ADPCM　　：pcm3pcm.x\n");
	printf("=< 2_MakeBLK.bat >==============\n");
	printf("    バイナリ結合　　：blk.x\n");
	printf("=< 3_MakeMACS.bat >=============\n");
	printf("    MACSヘッダ　　　：MACS_SCH.h\n");
	printf("    アセンブラ　　　：has.x\n");
	printf("    リンカ　　　　　：hlk.x\n");
	printf("    MACSデータ変換　：MakeMCS.x\n");
	printf("------------------------------\n");
}

void HelpMessage2(void)
{
	printf("MACScnv.x -S[n] <開始番号> <増加値> <終了番号> <動画再生時間[s] or PCMの時間[s]>\n");
	printf("ex. 0～1620まで存在する画像ファイルを4つずつ飛ばして、長さ81秒のMACSデータを作る場合\n");
	printf(">MACScnv.x -S1 0 4 1620 81\n");
	printf("------------------------------\n");
	printf("n=0 256x256 65536色  \n");
	printf("n=1 256x256 256色  \n");
	printf("n=2 384x256 256色  \n");
	printf("n=3 512x512 16色  \n");
	printf("n=4 512x512 4色  \n");
	printf("n=5 768x512 1色  \n");
	printf("------------------------------\n");
	printf("変換に必要なツール＆ファイル：\n");
	printf("=< 0_MakeTxTp.bat >=============\n");
	printf("    画像表示　　　　：APICG.r\n");
	printf("    画像保存　　　　：gvrsave.x\n");
	printf("=< 1_MakePCM.bat >==============\n");
	printf("    Wav to ADPCM　　：pcm3pcm.x\n");
	printf("=< 2_MakeLZE.bat >==============\n");
	printf("    画像圧縮　　　　：lze.x / lze.exe\n");
	printf("=< 3_MakeBLK.bat >==============\n");
	printf("    データオブジェクト生成　　：has060.x\n");
	printf("=< 4_MakeMACS.bat >=============\n");
	printf("    MACSヘッダ　　　：MACS_SCH.h\n");
	printf("    アセンブラ　　　：has060.x\n");
	printf("    リンカ　　　　　：hlk.r\n");
	printf("    MACSデータ変換　：MakeMCS.x\n");
	printf("------------------------------\n");
}

int16_t CGcnv(int16_t start_cnt, int16_t inc_val, int16_t end_cnt)
{
	FILE *fp;
	int8_t fname[256];
	int16_t ret = 0;
	int16_t cnt;
	
	int8_t sStingDef[16] = "screen 1,3";
	int8_t sStingMes[3][16] = {	"APICG",
								"YGMC /T4 /Z /B",
								"txs -p",
							};

	/* ファイルを開ける */
	if(g_nEffect == TRUE)
	{
		switch(g_nEffectType)
		{
		case 0:
		case 5:
			strcpy(fname, "0_MakeTx.bat");
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			strcpy(fname, "0_MakeTxTp.bat");
			break;
		default:
			break;
		}
	}
	else
	{
		strcpy(fname, "0_MakeTxTp.bat");
	}
	
	fp = fopen(fname, "r");
	if(fp != NULL)
	{
		printf("ファイルが既に存在します。(%s)\n", fname);
		printf("__Clean.bat を実行してください\n");
		/* ファイルを閉じる */
		fclose (fp);
		return -1;
	}
	
	fp = fopen(fname, "w");
	/* ファイルに書き込む */
	if(g_nEffect == TRUE)
	{
		fprintf(fp, "\n");
	}
	else
	{
		fprintf(fp, "%s\n\n", sStingDef);
	}
	
	for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
	{
		/* ファイルに書き込む */
		fprintf(fp, "%s CG%05d.bmp\n", sStingMes[0], cnt);
		
		if(g_nEffect == TRUE)
		{
			fprintf(fp, "\n");
			
			switch(g_nEffectType)
			{
			case 0:
			case 5:
				fprintf(fp, "gvrsave -s%d -oTx%05d\n\n", g_nEffectType, cnt, cnt);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				fprintf(fp, "gvrsave -s%d -oTx%05d -pTp%05d\n\n", g_nEffectType, cnt, cnt);
				break;
			default:
				break;
			}
		}
		else
		{
			fprintf(fp, "%s\n", sStingMes[1]);
			fprintf(fp, "%s Tx%05d Tp%05d\n\n", sStingMes[2], cnt, cnt);
		}
	}

	/* ファイルを閉じる */
	fclose (fp);
	
	printf("%s を作成しました。\n",fname);
	
	printf("---------------------------\n");

	/* ファイルを開ける */
	strcpy(fname, "1_MakePCM.bat");
	fp = fopen(fname, "r");
	if(fp != NULL)
	{
		printf("ファイルが既に存在します。(%s)\n", fname);
		printf("__Clean.bat を実行してください\n");
		/* ファイルを閉じる */
		fclose (fp);
		return -1;
	}
	
	fp = fopen(fname, "w");
	/* ファイルに書き込む */
	fprintf(fp, "pcm3pcm.x AD.WAV AD.PCM\n", sStingDef);
	if(g_nEffect == TRUE)
	{
		fprintf(fp, "pcm3pcm.x AD.WAV AD.s44\n", sStingDef);
	}
	/* ファイルを閉じる */
	fclose (fp);
	
	printf("%s を作成しました。\n",fname);
	printf("---------------------------\n");
	
	if(g_nEffect == TRUE)
	{
		/* ファイルを開ける */
		strcpy(fname, "2_MakeLZE.bat");
		fp = fopen(fname, "r");
		if(fp != NULL)
		{
			printf("ファイルが既に存在します。(%s)\n", fname);
			printf("__Clean.bat を実行してください\n");
			/* ファイルを閉じる */
			fclose (fp);
			return -1;
		}
		
		fp = fopen(fname, "w");
		/* ファイルに書き込む */
		fprintf(fp, "for %%%%i in (Tx*.) do lze e %%%%i %%%%i.lze\n", sStingDef);
		
		/* ファイルを閉じる */
		fclose (fp);
		
		printf("%s を作成しました。\n",fname);
		printf("---------------------------\n");
	}

	return ret;
}

int16_t BLTcnv(int16_t start_cnt, int16_t inc_val, int16_t end_cnt, int16_t mode)
{
	FILE *fp;
	int16_t ret = 0;
	int16_t i, j, flag, cnt;
	int8_t fname[256];
	int8_t str[256];
	
	i = 0;
	j = 0;
	flag = 0;
	cnt = start_cnt;
	
	if(mode == 1)
	{
		if(g_nEffect == TRUE)
		{
			switch(g_nEffectType)
			{
			case 0:
			case 5:
				return ret;
			case 1:
			case 2:
			case 3:
			case 4:
				break;
			default:
				break;
			}
		}
	}
	
	switch(mode)
	{
		case 2:
		{
			/* ファイルを開ける */
			if(g_nEffect == TRUE)
			{
				strcpy(fname, "LIST_PCM.s");
			}
			else
			{
				strcpy(fname, "LIST_PCM.blk");
			}
			fp = fopen(fname, "r");
			if(fp != NULL)
			{
				printf("ファイルが既に存在します。(%s)\n", fname);
				printf("__Clean.bat を実行してください\n");
				/* ファイルを閉じる */
				fclose (fp);
				return -1;
			}
			
			fp = fopen(fname, "w");
			
			/* ファイルに書き込む */
			if(g_nEffect == TRUE)
			{
				fprintf(fp, "pcmdat:: .insert AD.S44\n");
				fprintf(fp, "pcmend::\n");
				fprintf(fp, "adpcmdat:: .insert AD.PCM\n");
				fprintf(fp, "adpcmend::\n");
			}
			else
			{
				fprintf(fp, "pcmdat:AD.PCM\n");
				fprintf(fp, "pcmend:\n");
			}

			/* ファイルを閉じる */
			fclose (fp);
			
			printf("%s を作成しました。\n",fname);

			printf("---------------------------\n");
			return 0;
		}
		case 3:
		{
			/* ファイルを開ける */
			if(g_nEffect == TRUE)
			{
				strcpy(fname, "3_MakeBLK.bat");
			}
			else
			{
				strcpy(fname, "2_MakeBLK.bat");
			}
			fp = fopen(fname, "r");
			if(fp != NULL)
			{
				printf("ファイルが既に存在します。(%s)\n", fname);
				printf("__Clean.bat を実行してください\n");
				/* ファイルを閉じる */
				fclose (fp);
				return -1;
			}
			
			fp = fopen(fname, "w");
			
			/* ファイルに書き込む */
			if(g_nEffect == TRUE)
			{
				for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
				{
					fprintf(fp, "has060 LIST%d_Tx.s\n", cnt);
					
					switch(g_nEffectType)
					{
					case 0:
					case 5:
						break;
					case 1:
					case 2:
					case 3:
					case 4:
						fprintf(fp, "has060 LIST%d_Tp.s\n", cnt);
						break;
					default:
						break;
					}
				}
				fprintf(fp, "has060 LIST_PCM.s\n");
			}
			else
			{
				for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
				{
					fprintf(fp, "blk LIST%d_Tx.blk\n", cnt);
					fprintf(fp, "blk LIST%d_Tp.blk\n", cnt);
				}
				fprintf(fp, "blk LIST_PCM.blk\n");
			}

			/* ファイルを閉じる */
			fclose (fp);

			printf("%s を作成しました。\n",fname);
			
			printf("---------------------------\n");
			return 0;
		}
		default:
			break;
	}
	
	do
	{
		/* ファイルを開ける */
		switch(mode)
		{
			default:
			case 0:
			{
				if(g_nEffect == TRUE)
				{
					sprintf(fname, "LIST%d_Tx.s", i);
				}
				else
				{
					sprintf(fname, "LIST%d_Tx.blk", i);
				}
				strcpy(str, "Tx");
				break;
			}
			case 1:
			{
				if(g_nEffect == TRUE)
				{
					sprintf(fname, "LIST%d_Tp.s", i);
				}
				else
				{
					sprintf(fname, "LIST%d_Tp.blk", i);
				}
				strcpy(str, "Tp");
				break;
			}
		}
		fp = fopen(fname, "r");
		if(fp != NULL)
		{
			printf("ファイルが既に存在します。(%s)\n", fname);
			printf("__Clean.bat を実行してください\n");
			/* ファイルを閉じる */
			fclose (fp);
			return -1;
		}
		
		fp = fopen(fname, "w");
		do
		{
			if(g_nEffect == TRUE)
			{
				/* ファイルに書き込む */
				if(mode == 0)
				{
					fprintf(fp, "%s%05d:: .insert %s%05d.lze\n", str, cnt, str, cnt);
				}
				else
				{
					fprintf(fp, "%s%05d:: .insert %s%05d\n", str, cnt, str, cnt);
				}
			}
			else
			{
				/* ファイルに書き込む */
				fprintf(fp, "%s%05d:%s%05d\n", str, cnt, str, cnt);
			}
			
			cnt += inc_val;
			
			j++;
			if(j >= 500)
			{
				j = 0;
				i++;
				break;
			}
			
			if(cnt > end_cnt)
			{
				flag = 1;
				break;
			}
			
		}while(1);
		
		/* ファイルを閉じる */
		fclose (fp);
		
		printf("%s を作成しました。\n",fname);
		
	}while(flag == 0);

	printf("---------------------------\n");
	
	ret = i;
	
	return ret;
}

int16_t MACScnv(int16_t start_cnt, int16_t inc_val, int16_t end_cnt, int16_t mode, int16_t sec)
{
	FILE *fp;
	int8_t fname[256];
	int16_t ret = 0;
	int16_t frame;
	int16_t cnt, r_count, b_frame;
	float f_frame, f_fps, f_spf, f_time, f_r_diff, f_r_diff_old;
	float r_time, r_time_old;
	float f1, f2;
	int8_t sStingCMD[6][10] = {	"_G64K",
								"_G256",
								"_G384",
								"_T512_C16",
								"_T512_C4",
								"_T768",
							};

	f_time = (float)0;
	f_r_diff = (float)0;
	f_r_diff_old = (float)0;
	r_time = (float)0;
	r_time_old = (float)0;
	
	frame = end_cnt + 1;	/* 素材の総数 */
	
	
	switch(mode)
	{
		case 0:
		{
			/* ファイルを開ける */
			strcpy(fname, "MACSsrc.s");
			fp = fopen(fname, "r");
			if(fp != NULL)
			{
				printf("ファイルが既に存在します。(%s)\n", fname);
				printf("__Clean.bat を実行してください\n");
				/* ファイルを閉じる */
				fclose (fp);
				return -1;
			}
			
			fp = fopen(fname, "w");
			
			fprintf(fp, ".include MACS_SCH.h\n\n");

			fprintf(fp, "SET_OFFSET\n\n");
			
			if(g_nEffect == TRUE)
			{
				fprintf(fp, "USE_DUALPCM 'S44'\n");
				fprintf(fp, "TITLE ''\n");
				fprintf(fp, "COMMENT ''\n\n");

				fprintf(fp, "SCREEN_ON%s\n\n", sStingCMD[g_nEffectType]);
				
				fprintf(fp, "CHANGE_POSITION%s\n\n", sStingCMD[g_nEffectType]);
			}
			
			f_frame = (float)frame / inc_val;		/* 演算対象フレーム総数の算出[f] */
			f_fps = f_frame / (float)sec;			/* 一秒あたり何フレームなのか？算出[fps] */
			f_spf = (float)sec / f_frame;			/* 一フレームあたり何秒なのか？算出[spf] */
			b_frame = (int16_t)((60 / f_fps) + 0.999999);	/* ざっくりWAIT値 */
			
#if 0
			printf("%f[fps], %f[sec/f], %f[sec], %d\n", f_fps, f_spf, VSYNC_sec, b_frame);
#endif
			for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
			{
				if(g_nEffectFixedWait == 0)
				{
					f_time = ((float)cnt) * f_spf;	/* 現フレームの時間[sec] Alpha */
					
					if(cnt == 0)	/* 初回が０の場合 */
					{
						r_count = (float)0;
						r_time = (float)0;
						f_r_diff = (float)0;
					}
					else
					{
						f1 = f_time - (r_time + f_r_diff + (((float)(b_frame-0)) * VSYNC_sec));
						
						f2 = f_time - (r_time + f_r_diff + (((float)(b_frame-1)) * VSYNC_sec));

						if(fabs(f1) < fabs(f2))
						{
							r_count = b_frame;
						}
						else
						{
							r_count = b_frame - 1;
						}
						r_time_old = r_time;
						r_time = r_time_old + (((float)r_count) * VSYNC_sec);	/* 68で表示した場合何フレーム目の時間 Beta */
						f_r_diff_old = f_r_diff;
						f_r_diff = r_time - f_time + f_r_diff_old;
					}
#if 0
					printf("c=%4d, r_c=%d, f_t=%f, r_t=%f, fd=%f, f1=%f, f2=%f\n", cnt, r_count, f_time, r_time, f_r_diff, fabs(f1), fabs(f2) );
#endif
#if 0
					printf("%4d, %f, %f, %f\n", cnt, f_time, r_time, f_r_diff);
#endif
				}
				else
				{
					r_count = g_nEffectFixedWait;
					b_frame = g_nEffectFixedWait;
				}
				
				if( (r_count > 0) || (cnt == start_cnt) )
				{
					if(g_nEffect == TRUE)
					{
						fprintf(fp, "DRAW%s Tx%05d\n", sStingCMD[g_nEffectType], cnt);
						if(cnt != start_cnt)
						{
							fprintf(fp, "WAIT %d\n", (int16_t)r_count);
						}
						else
						{
							fprintf(fp, "WAIT %d\n", (int16_t)b_frame);
						}
						
						switch(g_nEffectType)
						{
						case 0:
						case 5:
							/* none */
							break;
						case 1:
						case 2:
							fprintf(fp, "PALETTE256 Tp%05d\n", cnt);
							break;
						case 3:
						case 4:
							fprintf(fp, "PALETTE16 Tp%05d\n", cnt);
							break;
						default:
							break;
						}
						
						fprintf(fp, "CHANGE_POSITION%s\n\n", sStingCMD[g_nEffectType]);
					}
					else
					{
						fprintf(fp, "DRAW Tx%05d\n", cnt);
						if(cnt != start_cnt)
						{
							fprintf(fp, "WAIT %d\n", (int16_t)r_count);
						}
						fprintf(fp, "PALETTE Tp%05d\n", cnt);
						fprintf(fp, "CHANGE_POSITION\n\n");
					}
				}
				
				if(cnt == start_cnt)
				{
					if(g_nEffect == TRUE)
					{
						fprintf(fp, "PCM_PLAY_S44 pcmdat,pcmend-pcmdat\n");
						fprintf(fp, "PCM_PLAY_SUBADPCM adpcmdat,adpcmend-adpcmdat\n\n");
					}
					else
					{
						fprintf(fp, "SCREEN_ON\n\n");
						fprintf(fp, "PCM_PLAY pcmdat,pcmend-pcmdat\n\n");
					}
				}
				
			}
			
			fprintf(fp, "WAIT 45\n");
			fprintf(fp, "PCM_STOP\n");
			fprintf(fp, "EXIT\n");
			
			/* ファイルを閉じる */
			fclose (fp);

			printf("%s を作成しました。\n",fname);
			
			break;
		}
		case 1:
		{
			int8_t sStingMes[6][32] = {	"has -u MACSsrc",
										"hlk -r MACSsrc",
										"MakeMCS MACSsrc",
										"has060 -u MACSsrc",
										"060high 1",
										"060high 0"
									};
			
			/* ファイルを開ける */
			if(g_nEffect == TRUE)
			{
				strcpy(fname, "4_MakeMACS.bat");
			}
			else
			{
				strcpy(fname, "3_MakeMACS.bat");
			}
			fp = fopen(fname, "r");
			if(fp != NULL)
			{
				printf("ファイルが既に存在します。(%s)\n", fname);
				printf("__Clean.bat を実行してください\n");
				/* ファイルを閉じる */
				fclose (fp);
				return -1;
			}
			
			fp = fopen(fname, "w");
			
			/* ファイルに書き込む */
			if(g_nEffect == TRUE)
			{
				fprintf(fp, "%s\n", sStingMes[3]);
				fprintf(fp, "%s\n", sStingMes[4]);
			}
			else
			{
				fprintf(fp, "%s\n", sStingMes[0]);
			}
			fprintf(fp, "%s ", sStingMes[1]);
			for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
			{
				fprintf(fp, "LIST%d_Tx ", cnt);
				switch(g_nEffectType)
				{
				case 0:
				case 5:
					/* none */
					break;
				case 1:
				case 2:
				case 3:
				case 4:
					fprintf(fp, "LIST%d_Tp ", cnt);
					break;
				default:
					break;
				}
			}
			fprintf(fp, "LIST_PCM\n\n");
			fprintf(fp, "%s\n", sStingMes[2]);
			if(g_nEffect == TRUE)
			{
				fprintf(fp, "%s\n", sStingMes[5]);
			}

			/* ファイルを閉じる */
			fclose (fp);

			printf("%s を作成しました。\n",fname);
			break;
		}
		default:
			break;
	}
	printf("---------------------------\n");

	return ret;
}

int16_t main(int16_t argc, int8_t** argv)
{
	int16_t ret = 0;
	int16_t cnt[4];
	
	puts("MACSデータ作成補助ツール「MACScnv.x」 v1.03 (c)2022 カタ.");
	
	if(argc > 1)	/* オプションチェック */
	{
		int16_t i;
		
		for(i = 1; i < argc; i++)
		{
			int8_t	bOption;
			int8_t	bFlag, bFlag2;
			
			bOption	= ((argv[i][0] == '-') || (argv[i][0] == '/')) ? TRUE : FALSE;

			if(bOption == TRUE)
			{
				bFlag	= ((argv[i][1] == '?') || (argv[i][1] == 'h') || (argv[i][1] == 'H')) ? TRUE : FALSE;
				
				if(bFlag == TRUE)
				{
					HelpMessage();	/* ヘルプ */
					HelpMessage2();	/* ヘルプ2 */
					ret = -1;
				}
				
				bFlag	= ((argv[i][1] == 's') || (argv[i][1] == 'S')) ? TRUE : FALSE;
				bFlag2	= strlen(&argv[i][2]);
				if((bFlag == TRUE) && (bFlag2 > 0))
				{
					g_nEffectType = atoi(&argv[i][2]);
					if(g_nEffectType < 0)			g_nEffectType = 255;
					else if(g_nEffectType > 255)	g_nEffectType = 255;
					
					if(bFlag == TRUE)
					{
						g_nEffect = Mbset(g_nEffect, 0x7F, Bit_0);
						puts("拡張グラフィックモード");
						switch(g_nEffectType)
						{
						case 0:
								puts("Mode=0 as 256x256 color 65536");
							break;
						case 1:
								puts("Mode=1 as 256x256 color 256");
							break;
						case 2:
								puts("Mode=2 as 384x256 color 256");
							break;
						case 3:
								puts("Mode=3 as 512x512 color 16");
							break;
						case 4:
								puts("Mode=4 as 512x512 color 8");
							break;
						case 5:
								puts("Mode=5 as 768x512 color 1");
							break;
						default:
							break;
						}
					}
					continue;
				}

				bFlag	= ((argv[i][1] == 'w') || (argv[i][1] == 'W')) ? TRUE : FALSE;
				if(bFlag == TRUE)
				{
					bFlag2	= strlen(&argv[i][2]);
					g_nEffectFixedWait = atoi(&argv[i][2]);
					if(g_nEffectType <= 0)		g_nEffectFixedWait = 1;
					else if(g_nEffectType > 10)	g_nEffectFixedWait = 10;

					continue;
				}
				
				HelpMessage();	/* ヘルプ */
				ret = -1;
				break;
			}
			else
			{
				cnt[0] = atoi(argv[i+0]);
				cnt[1] = atoi(argv[i+1]);
				cnt[2] = atoi(argv[i+2]);
				cnt[3] = atoi(argv[i+3]);
				
				if((cnt[0] == cnt[1]) && (cnt[0] == cnt[2]))
				{
					ret = -1;
					break;
				}
				else
				{
					if(cnt[1] <= 0)
					{
						ret = -1;
						break;
					}
					else
					{
						printf("CG%05d ～ CG%05d まで %d ずつ処理するバッチファイルを作成します(合計：%d枚)\n", cnt[0], cnt[2], cnt[1],((cnt[2]+1)-(cnt[0]))/cnt[1]);
						printf("動画の長さは %d分%d秒 です\n", cnt[3] / 60, cnt[3] % 60);
						printf("---------------------------\n");
						/* 画像変換 */
						CGcnv(cnt[0], cnt[1], cnt[2]);
						/* データObj生成 */
						ret = BLTcnv(cnt[0], cnt[1], cnt[2], 0);
						BLTcnv(cnt[0], cnt[1], cnt[2], 1);
						BLTcnv(0, 0, 0, 2);
						BLTcnv(0, 1, ret, 3);
						/* データ変換 */
						MACScnv(cnt[0], cnt[1], cnt[2], 0, cnt[3]);
						MACScnv(0, 1, ret, 1, 0);
						break;
					}
				}
			}
		}
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}


