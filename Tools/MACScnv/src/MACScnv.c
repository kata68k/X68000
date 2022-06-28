#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "usr_style.h"
#include "usr_macro.h"

#define VSYNC_sec	(0.01803177)/*(1.0f/55.45767f)*/

int16_t CGcnv(int16_t, int16_t, int16_t);
int16_t BLTcnv(int16_t, int16_t, int16_t, int16_t);
int16_t MACScnv(int16_t, int16_t, int16_t, int16_t, int16_t);
int16_t main(int16_t, int8_t**);

int16_t CGcnv(int16_t start_cnt, int16_t inc_val, int16_t end_cnt)
{
	FILE *fp;
	int8_t fname[256];
	int16_t ret = 0;
	int16_t cnt;
	
	int8_t sStingDef[16] = "screen 1,3";
	int8_t sStingMes[4][16] = {	"APICG",
								"YGMC /T4 /Z /B",
								"txs -p"
							};

	/* ファイルを開ける */
	strcpy(fname, "0_MakeTxTp.bat");
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
	fprintf(fp, "%s\n\n", sStingDef);
	
	for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
	{
		/* ファイルに書き込む */
		fprintf(fp, "%s CG%05d.bmp\n", sStingMes[0], cnt);
		fprintf(fp, "%s\n", sStingMes[1]);
		fprintf(fp, "%s Tx%05d Tp%05d\n\n", sStingMes[2], cnt, cnt);
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
	/* ファイルを閉じる */
	fclose (fp);
	
	printf("%s を作成しました。\n",fname);
	printf("---------------------------\n");
	
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
	
	switch(mode)
	{
		case 2:
		{
			/* ファイルを開ける */
			strcpy(fname, "LIST_PCM.blk");
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
			fprintf(fp, "pcmdat:AD.PCM\n");
			fprintf(fp, "pcmend:\n");

			/* ファイルを閉じる */
			fclose (fp);
			
			printf("%s を作成しました。\n",fname);

			printf("---------------------------\n");
			return 0;
		}
		case 3:
		{
			/* ファイルを開ける */
			strcpy(fname, "2_MakeBLK.bat");
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
			for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
			{
				fprintf(fp, "blk LIST%d_Tx.blk\n", cnt);
				fprintf(fp, "blk LIST%d_Tp.blk\n", cnt);
			}
			fprintf(fp, "blk LIST_PCM.blk\n");

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
				sprintf(fname, "LIST%d_Tx.blk", i);
				strcpy(str, "Tx");
				break;
			}
			case 1:
			{
				sprintf(fname, "LIST%d_Tp.blk", i);
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
			/* ファイルに書き込む */
			fprintf(fp, "%s%05d:%s%05d\n", str, cnt, str, cnt);
			
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
			
			f_frame = (float)frame / inc_val;	/* 演算対象フレーム総数の算出[f] */
			f_fps = f_frame / (float)sec;		/* 一秒あたり何フレームなのか？算出[fps] */
			f_spf = (float)sec / f_frame;		/* 一フレームあたり何秒なのか？算出[spf] */
			b_frame = (60 / (int16_t)f_fps);	/* ざっくりWAIT値 */
			
#if 0
			printf("%f[fps], %f[sec/f], %f[sec], %d\n", f_fps, f_spf, VSYNC_sec, b_frame);
#endif
			for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
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
#if 0
					printf("c=%4d, r_c=%d, f_t=%f, r_t=%f, fd=%f, f1=%f, f2=%f\n", cnt, r_count, f_time, r_time, f_r_diff, fabs(f1), fabs(f2) );
#endif
					r_time_old = r_time;
					r_time = r_time_old + (((float)r_count) * VSYNC_sec);	/* 68で表示した場合何フレーム目の時間 Beta */
					f_r_diff_old = f_r_diff;
					f_r_diff = r_time - f_time + f_r_diff_old;
				}
				
#if 0
				printf("%4d, %f, %f, %f\n", cnt, f_time, r_time, f_r_diff);
#endif
				
				if( (r_count > 0) || (cnt == start_cnt) )
				{
					fprintf(fp, "DRAW Tx%05d\n", cnt);
					if(cnt != start_cnt)
					{
						fprintf(fp, "WAIT %d\n", (int16_t)r_count);
					}
					fprintf(fp, "PALETTE Tp%05d\n", cnt);
					fprintf(fp, "CHANGE_POSITION\n\n");
				}
				
				if(cnt == start_cnt)
				{
					fprintf(fp, "SCREEN_ON\n\n");
					fprintf(fp, "PCM_PLAY pcmdat,pcmend-pcmdat\n\n");
				}
			}
			
			fprintf(fp, "WAIT 120\n");
			fprintf(fp, "PCM_STOP\n");
			fprintf(fp, "EXIT\n");
			
			/* ファイルを閉じる */
			fclose (fp);

			printf("%s を作成しました。\n",fname);
			
			printf("---------------------------\n");
			return 0;
		}
		case 1:
		{
			int8_t sStingMes[3][32] = {	"has -u MACSsrc",
										"hlk -r MACSsrc",
										"MakeMCS MACSsrc"
									};
			
			/* ファイルを開ける */
			strcpy(fname, "3_MakeMACS.bat");
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
			fprintf(fp, "%s\n\n", sStingMes[0]);
			fprintf(fp, "%s ", sStingMes[1]);
			for(cnt = start_cnt; cnt <= end_cnt; cnt += inc_val)
			{
				fprintf(fp, "LIST%d_Tx ", cnt);
				fprintf(fp, "LIST%d_Tp ", cnt);
			}
			fprintf(fp, "LIST_PCM\n\n");
			fprintf(fp, "%s\n", sStingMes[2]);

			/* ファイルを閉じる */
			fclose (fp);

			printf("%s を作成しました。\n",fname);
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
	
	if (argc == 5)
	{
		cnt[0] = atoi(argv[1]);
		cnt[1] = atoi(argv[2]);
		cnt[2] = atoi(argv[3]);
		cnt[3] = atoi(argv[4]);
		
		if((cnt[0] == cnt[1]) && (cnt[0] == cnt[2]))
		{
			return -1;
		}
		else
		{
			if(cnt[1] <= 0)
			{
				return -1;
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
			}
		}
	}
	else
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
		printf("ver1.0.2\n");
	}
	
#if 0
	if (strcmp(argv[1], "help") == 0)
	{
	}
	else if (strcmp(argv[1], "add") == 0 && argc == 4)
	{
		printf("add:%d\n", atoi(argv[2]) + atoi(argv[3]));
	}
	else if (strcmp(argv[1], "sub") == 0 && argc == 4)
	{
		printf("sub:%d\n", atoi(argv[2]) - atoi(argv[3]));
	}
	else
	{
		printf("サポートされていないコマンドライン引数です\n");
	}
#endif
	return 0;
}


