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
#include "IF_MACS.h"
#include "IF_Graphic.h"
#include "IF_MUSIC.h"
#include "IF_Memory.h"
#include "IF_PCG.h"

/* �O���[�o���ϐ� */
int8_t		g_data_list[DATA_MAX][256]	=	{0};
uint32_t	d_list_max	=	0u;
int8_t		g_data_list_sel	=	0u;
//static int32_t		nMaxFreeMem;

/* �֐��̃v���g�^�C�v�錾 */
int16_t Init_FileList_Load(void);
size_t File_Load(int8_t *, void *, size_t, size_t);
int16_t File_Save(int8_t *, void *, size_t, size_t);
int16_t File_Load_CSV(int8_t *, uint16_t *, uint16_t *, uint16_t *);
int16_t PCG_SP_dataload(int8_t *);
int16_t PCG_PAL_dataload(int8_t *);
int16_t Load_Music_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_SE_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
int16_t Load_CG_List(	int8_t *, int8_t *, CG_LIST *, uint32_t *);
int16_t Load_MACS_List(	int8_t *, int8_t *, int8_t (*)[256], uint32_t *);
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Init_FileList_Load(void)
{
	int16_t ret = 0;
	uint8_t	sListFilePath[255] = {0};
	uint8_t	sListFileName[255] = {0};
	uint32_t	i;
	
	/* �f�[�^���X�g�L�� */
	if(GetFileLength("data\\datalist.txt", &i) == -1)
	{
		strcpy( g_data_list[0], "data" );
		Set_DataList_Num(0);
		d_list_max = 1;
	}
	else
	{
		puts("==============< datalist.txt >=============================================");
		Load_DATA_List("data\\", "datalist.txt", g_data_list, &d_list_max);
		puts("===========================================================================");
		printf("Input Data Number [0 - %d] = ", d_list_max - 1);
		if(d_list_max <= 1)
		{
			ret = 0;
		}
		else
		{
			scanf("%hd", &ret);
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
				/* �������Ȃ� */
			}
		}
		Set_DataList_Num(ret);
	}
#ifdef DEBUG
	printf("Load_DATA_List=%s(%d)\n", Get_DataList_Path(), ret);
//	printf("Load_DATA_List=0x%p\n", Get_DataList_Path());
//	KeyHitESC();	/* �f�o�b�O�p */
#endif
	printf("Load_DATA_List Max %d\n", d_list_max);

	/* �O���t�B�b�N */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "cg");
	Load_CG_List(sListFilePath, "g_list.txt", g_stCG_LIST, &g_CG_List_Max);			/* �O���t�B�b�N���X�g�̓ǂݍ��� */
	for(i = 0; i < g_CG_List_Max; i++)
	{
		printf("PIC File %2d = %s\n", i, g_stCG_LIST[i].bFileName);
	}
	printf("PIC File Max %d\n", g_CG_List_Max);

	/* ����(MACS) */
#if CNF_MACS	
 #ifdef 	MACS_MOON
 	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "mov");
	Load_MACS_List(sListFilePath, "mov_list.txt", g_mov_list, &g_mov_list_max);	/* ����(MACS)���X�g�̓ǂݍ��� */
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
	
	/* ���ʉ�(FM) */
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
	
	/* ���ʉ�(ADPCM) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "se");
	Load_SE_List(sListFilePath, "p_list.txt", adpcm_list, &p_list_max);
	for(i = 0; i < p_list_max; i++)
	{
		printf("ADPCM File %2d = %s\n", i, adpcm_list[i]);
	}
	printf("ADPCM File Max %d\n", p_list_max);
	
	/* �X�v���C�g(PCG) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "sp");
	Load_SP_List(sListFilePath, "sp_list.txt", sp_list, &sp_list_max);
	for(i = 0; i < sp_list_max; i++)
	{
		printf("PCG File %2d = %s\n", i, sp_list[i]);
	}
	printf("PCG File Max %d\n", sp_list_max);

	/* �X�v���C�g(PAL) */
	sprintf(sListFilePath, "%s\\%s\\", Get_DataList_Path(), "sp");
	pal_list_max = 1;
	Load_List(sListFilePath, "pal_list.txt", pal_list, &pal_list_max);
	for(i = 0; i < pal_list_max; i++)	/* 0�̓e�L�X�g���p�p���b�g�Ȃ̂�1�X�^�[�g */
	{
		printf("PAL File %2d = %s\n", i+1, pal_list[i+1]);
	}
	printf("PAL File Max %d\n", pal_list_max);

	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* �t�@�C���ǂݍ��� */
/* *fname	�t�@�C���� */
/* *ptr		�i�[��̐擪�A�h���X */
/* size		�f�[�^�̃T�C�Y */
/* n		�f�[�^�̌� */
size_t File_Load(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	size_t ret = 0;

	/* �t�@�C�����J���� */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)
	{
		/* �t�@�C�����ǂݍ��߂܂��� */
		printf("ERR:%s�t�@�C����������܂���I\n", fname);
		ret = -1;
	}
	else
	{
		/* �f�[�^�����w�肵�Ȃ��ꍇ */
		if(n == 0)
		{
			/* �t�@�C���T�C�Y���擾 */
			n = filelength(fileno(fp));
		}
		
		/* �t�@�C���ǂݍ��� */
		ret = fread (ptr, size, n, fp);

		if (ret != n) {
	        if (feof(fp)) {
				fprintf(stderr, "Premature end of file reached\n");
			} else if (ferror(fp)) {
				perror("Error while reading from file");
			}
		}

		/* �t�@�C������� */
		fclose (fp);
	}

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* �t�@�C���ۑ� */
/* *fname	�t�@�C���� */
/* *ptr		�i�[��̐擪�A�h���X */
/* size		�f�[�^�̃T�C�Y */
/* n		�f�[�^�̌� */
int16_t File_Save(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* �t�@�C�����J���� */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)	/* �t�@�C�������� */
	{
		/* �t�@�C�����J���� */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	else
	{
		/* �t�@�C�������݂���ꍇ�͉������Ȃ� */
	}
	/* �t�@�C������� */
	fclose (fp);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* �t�@�C���ۑ� */
/* *fname	�t�@�C���� */
/* *ptr		�i�[��̐擪�A�h���X */
/* size		�f�[�^�̃T�C�Y */
/* n		�f�[�^�̌� */
int16_t File_Save_OverWrite(int8_t *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int16_t ret = 0;

	/* �t�@�C�����J���� */
	fp = fopen(fname, "rb");
	
	if(fp == NULL)	/* �t�@�C�������� */
	{
		/* �t�@�C�����J���� */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	else
	{
		/* �t�@�C�������݂���ꍇ�͉������Ȃ� */
		fp = fopen(fname, "wb");
		fwrite(ptr, size, n, fp);
	}
	/* �t�@�C������� */
	fclose (fp);

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* CSV�t�@�C���ǂݍ��� */
/* *fname	�t�@�C���� */
/* *ptr		�i�[��̐擪�A�h���X */
/* *Col		�f�[�^�̍s���̃A�h���X */
/* *Row		�f�[�^�̗񐔂̃A�h���X */
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
					/* ������̏I��肾���� */
					if(flag==0)
					{
						*Col = x;
					}
					flag = 1;
					break;
				}
				else if(p != end)
				{
					/* �ϊ��ł��� */
					*ptr = (uint16_t)num;
					ptr++;

					p = end + 1;
					
					cnv_flag = 1;
					x++;
				}
				else
				{
					/* �ϊ��ł��Ȃ����� */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
/* CSV �R�[�X�f�[�^�t�@�C���ǂݍ��� */
/* *fname	�t�@�C���� */
/* *st_ptr	�i�[��̐擪�A�h���X */
/* *Col		�f�[�^�̍s���̃A�h���X */
/* *Row		�f�[�^�̗񐔂̃A�h���X */
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
					/* ������̏I��肾���� */
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
					/* �ϊ��ł��� */
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
					/* �ϊ��ł��Ȃ����� */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static uint8_t		pcg_dat[PCG_MAX][SP_16_SIZE];		/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */

int16_t PCG_SP_dataload(int8_t *fname)
{
	int16_t ret = 0;
	
	FILE *fp;
	/*-----------------[ �o�b�f�f�[�^�ǂݍ��� ]-----------------*/

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
		pcg_size = Mmin(pcg_size, PCG_MAX * SP_16_SIZE);	/* BG�̐ݒ��ԂŃT�C�Y���قȂ�H�H */

		pat = pcg_size / SP_16_SIZE;
		
		j = fread( &pcg_dat[0][0]
			,  SP_16_SIZE				/* 1PCG = 128byte */
			,  pat	/* PCG */
			,  fp
			) ;
		fclose( fp ) ;

		for( i = 0; i < pat; i++ )
		{
			_iocs_sp_defcg( i, 1, &pcg_dat[i][0] );
		}
	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static uint16_t	pal_dat[ 128 ];	/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */

int16_t PCG_PAL_dataload(int8_t *fname)
{
	int16_t ret = 0;

	FILE *fp;
	uint32_t i;

	/*--------[ �X�v���C�g�p���b�g�f�[�^�ǂݍ��݂ƒ�` ]--------*/

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

		/* �X�v���C�g�p���b�g�ɓ]�� */
		for( i = 0 ; i < 256 ; i++ )
		{
			_iocs_spalet( (i&15) | (1<<0x1F) , Mdiv16(i)+1 , pal_dat[i] ) ;
		}
	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
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
//				KeyHitESC();	/* �f�o�b�O�p */
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
						g_stST_PCG_LIST[i].hit_width = width * SP_W;
					}

					if(hit_height != 0)
					{
						g_stST_PCG_LIST[i].hit_height = hit_height;
					}
					else
					{
						g_stST_PCG_LIST[i].hit_height = height * SP_H;
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
//					KeyHitESC();	/* �f�o�b�O�p */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
			{

			}
			else if(size <= 3)
			{

			}
			else
			{
				memset(z_name, 0, sizeof(z_name));
				sscanf(p,"%d= %s %d %d", &num, z_name, &nType, &nTransPal);	/* format:�ԍ� = �t�@�C���p�X���t�@�C����,�O���t�B�b�N�̃^�C�v,���ߐF�̃p���b�g�ԍ� */
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
//				KeyHitESC();	/* �f�o�b�O�p */
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Load_MACS_List(int8_t *fpath, int8_t *fname, int8_t (*macs_list)[256], uint32_t *list_max)
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
			if(*p == ';')	/* �R�����g */
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
					sprintf(macs_list[i], "%s%s", fpath, z_name);
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
			if(*p == ';')	/* �R�����g */
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
					printf("No.(%d) = %s\n", i, z_name);
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
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
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
		pBuf = &sBuf[0];	/* �擪�A�h���X */

		/* PIC */
		fread( pBuf, sizeof(uint8_t), 3, fp );
		if(strncmp( pBuf, "PIC", 3 ) == 0)
		{
			/* ��v */
//			printf("%s�t�@�C���ł�\n", pBuf );
			bFlag = TRUE;
		}
		else
		{
			bFlag = FALSE;
			printf("error:PIC�t�@�C���ł͂���܂��� = %s\n", pBuf );
		}
		
		memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
		pBuf = &sBuf[0];	/* �擪�A�h���X */

		/* �g���w�b�_���H */
		fread( pBuf, sizeof(uint8_t), 3, fp );
		if(strncmp( pBuf, "/MM", 3 ) == 0)
		{
			/* ��v */
//			printf("�g���t�@�C���ł�\n", pBuf );

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
			pBuf = &sBuf[0];	/* �擪�A�h���X */

			for(i=0; i < 128; i++)
			{
				/* �g���w�b�_���� */
				fread( pBuf, sizeof(uint8_t), 1, fp );
				if(strncmp( pBuf, "/", 1 ) == 0)
				{
					*pBuf = 0x00;
//					printf("%s\n", &sBuf[0]);
					
					memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
					pBuf = &sBuf[0];	/* �擪�A�h���X */

					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* �g���w�b�_�I������ */
					if(strncmp( &sBuf[0], ":", 1 ) == 0)
					{
						break;
					}
					else
					{
						pBuf++;
					}
					
					fread( pBuf, sizeof(uint8_t), 1, fp );
					/* �摜�f�[�^�̍�Ҕ��� */
					if(strncmp( &sBuf[0], "AU", 2 ) == 0)
					{
//						printf("�摜�f�[�^�̍��:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
						pBuf = &sBuf[0];	/* �擪�A�h���X */
					}
					/* �摜�̃��[�h���W */
					else if(strncmp( &sBuf[0], "XY", 2 ) == 0)
					{
//						printf("XY:");
						memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
						pBuf = &sBuf[0];	/* �擪�A�h���X */
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
			
			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
			pBuf = &sBuf[0];	/* �擪�A�h���X */

			/* �R�����g�I���܂œǂݍ��� */
			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
				if(*pBuf == 0x1Au)
				{
					if(i != 0u)
					{
//						printf("�R�����g(%d)�F%s\n", i, sBuf[0] );
					}
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}
		else
		{
			/* ���ʂ̃w�b�_�t�@�C���ł��� */
			pBuf += 3;
			bFlag = FALSE;

			for(i=0; i < 128; i++)
			{
				fread( pBuf, sizeof(uint8_t), 1, fp);
				if(*pBuf == 0x1Au)
				{
//					printf("�R�����g(%d)�F%s\n", i, sBuf[0] );
					bFlag = TRUE;
					break;
				}
				pBuf++;
			}
		}

		/* �_�~�[ */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;

			memset(sBuf, 0, sizeof(uint8_t) * 128);	/* �o�b�t�@�N���A */
			pBuf = &sBuf[0];	/* �擪�A�h���X */
			
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

		/* �\�񕶎�(0) */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
				bFlag = TRUE;
			}
		}

		/* �^�C�v�E���[�h */
		if(bFlag == TRUE)
		{
			bFlag = FALSE;
			fread( pBuf, sizeof(uint8_t), 1, fp);
			if(*pBuf == 0x00u)
			{
//				printf("Type/Mode�F%d\n", *pBuf );
				bFlag = TRUE;
			}
		}
		
		if(bFlag == TRUE)
		{
			/* �F�̃r�b�g�� */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biBitCount = uWord;
//			printf("color bit�F%d\n", uWord );

			/* �w�����̃T�C�Y */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biWidth	= (int64_t)uWord;
//			printf("X,Y�F(%4d,", uWord );

			/* �x�����̃T�C�Y */
			fread( &uWord, sizeof(uint16_t), 1, fp);
			info->biHeight	= (int64_t)uWord;
//			printf("%4d)\n", uWord );
		}

		fclose( fp ) ;
	}
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetRectangleSise(uint16_t *uSize, uint16_t uWidth, uint16_t uHeight, uint16_t uMaxSize)
{
	int16_t ret = 0;
	
	*uSize = Mmax( ((((uWidth+ 7u) & 0xFFF8u) * uHeight) << uMaxSize),  (512u << uMaxSize) );
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	Get_DataList_Num(uint16_t *p_uNum)
{
	int16_t ret = 0;
	
	*p_uNum = g_data_list_sel;
	
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	Set_DataList_Num(uint16_t uNum)
{
	int16_t ret = 0;
	
	g_data_list_sel = uNum;
	
	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int8_t *Get_DataList_Path(void)
{
	return &g_data_list[g_data_list_sel][0];
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Get_FileAlive(const int8_t *sFile)
{
	int16_t ret = 0;
    // �t�@�C����ǂݍ��݃��[�h�ŊJ��
    FILE *fp;
	fp = fopen(sFile, "r");
    // �t�@�C�������݂��邩�ǂ������m�F
    if (fp) {
 //       printf("�t�@�C�� '%s' �͑��݂��܂��B\n", sFile);
        fclose(fp); // �t�@�C�������
    } else {
//        printf("�t�@�C�� '%s' �͑��݂��܂���B\n", sFile);
		ret = -1;
    }
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t Get_ZPDFileName(const int8_t *sFile, int8_t *sZpdFile)
{
	int16_t found_63 = 0;
	int16_t found_2E = 0;
	uint8_t	buffer[1];
	size_t i=0;
    size_t capacity = 1024; // �o�b�t�@�̏����e��
    size_t size = 0; // ���݂̃f�[�^�̃T�C�Y
    uint8_t *data;
    FILE *file;

	file = fopen(sFile, "rb"); // �o�C�i�����[�h�Ńt�@�C�����J��
    if (file == NULL)
	{
        return 1;
    }

    // 0x63��T��
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

    // 0x63��������Ȃ������ꍇ
    if (found_63 == 0)
	{
//        printf("Pattern 0x63 not found.\n");
        fclose(file);
        return 2;
    }

    // ���̃o�C�g�f�[�^��0x2E��T��
    data = (uint8_t *)malloc(capacity);
    if (data == NULL)
	{
        fclose(file);
        return 1;
    }

	i = 0;

    // �f�[�^��ǂݍ��݂Ȃ���0x2E��T��
    while (!feof(file)) {
        if (fread(buffer, sizeof(uint8_t), 1, file) != 1) {
            break;
        }
        if (buffer[0] == 0x00) {
            found_2E = 1;
            break;
        }
        // �f�[�^���o�b�t�@�ɒǉ�
        if (size >= capacity) {
			uint8_t *new_data;
            capacity *= 2; // �o�b�t�@�̗e�ʂ�{�Ɋg��
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

    // 0x2E��������Ȃ������ꍇ
    if (found_2E == 0) {
        fclose(file);
        free(data);
        return 2;
    }

    // ���������f�[�^��\��
    for (i = 0; i < size; ++i) {
        *sZpdFile = data[i];
		sZpdFile++;
    }

    // �t�@�C�������
    fclose(file);
    // �����������
    free(data);

    return 0;
}

#endif	/* IF_FILEMANAGER_C */

