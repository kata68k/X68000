#ifndef	IF_GRAPHIC_H
#define	IF_GRAPHIC_H

#include <usr_define.h>

enum
{
	POS_TOP=0,
	POS_CENTER,
	POS_BOTTOM,
	POS_LEFT,
	POS_MID,
	POS_RIGHT,
};

#define G_COL_OFST	(0xF0)
#define G_BG		(0x00)
#define G_BLACK		(0x01 + G_COL_OFST)
#define G_D_GRAY	(0x02 + G_COL_OFST)
#define G_GRAY		(0x03 + G_COL_OFST)
#define G_D_WHITE	(0x04 + G_COL_OFST)
#define G_WHITE		(0x05 + G_COL_OFST)
#define G_D_GREEN	(0x06 + G_COL_OFST)
#define G_D_RED		(0x07 + G_COL_OFST)
#define G_D_BLUE	(0x08 + G_COL_OFST)
#define G_INDIGO	(0x09 + G_COL_OFST)
#define G_RED		(0x0A + G_COL_OFST)
#define G_GREEN		(0x0B + G_COL_OFST)
#define G_YELLOW	(0x0C + G_COL_OFST)
#define G_BLUE		(0x0D + G_COL_OFST)
#define G_L_PINK	(0x0E + G_COL_OFST)
#define G_L_BLUE	(0x0F + G_COL_OFST)

#define F_MOJI		(G_WHITE)
#define F_MOJI_BAK	(G_D_GRAY)

#define	TRANS_PAL		(0x00)
#define	CG_MAX			(27)		/* �O���t�B�b�N�p���b�g���Ɉˑ� */

#define FILE_TYPE			(0x424D)	/* "BM"�l */
#define FILE_HEADER_SIZE	(14)		/* BMP�t�@�C���w�b�_�T�C�Y */
#define INFO_HEADER_SIZE	(40)		/* Windows�w�b�_�T�C�Y */
#define DEFAULT_HEADER_SIZE	(FILE_HEADER_SIZE + INFO_HEADER_SIZE)	/* �W���̃w�b�_�T�C�Y */

//�r�b�g�}�b�v�o�C�g���̎Z�o�}�N��
#ifndef WIDTHBYTES
#define WIDTHBYTES(bits)    (((bits)+31)/32*4)
#endif//WIDTHBYTES

//YUV���ϒl�����߂�ۂ̍�ƃe�[�u��
typedef struct tagAVGYUV{
	double	y;
	double	u;
	double	v;
	int32_t cnt;
}AVGYUV,*PAVGYUV;

//�q�f�a�ϊ��Ŏg�p
typedef struct tagYUV{
	float	y;
	float	u;
	float	v;
	int32_t no;
} YUV,*PYUV;

#pragma pack (push, 1)   //�p�f�B���O���Pbyte�ɕύX

typedef struct tagBITMAPFILEHEADER {
	uint16_t	bfType;			/* �t�@�C���^�C�v */
	uint64_t	bfSize;			/* �t�@�C���T�C�Y */
	uint16_t	bfReserved1;	/* �\��̈�P */
	uint16_t	bfReserved2;	/* �\��̈�Q */
	uint64_t	bfOffBits;		/* �擪����摜�f�[�^�܂ł̃I�t�Z�b�g */
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	uint64_t	biSize;			/* �w�b�_�T�C�Y */
	int64_t		biWidth;		/* �摜�̕�(px) */
	int64_t		biHeight;		/* �摜�̍���(px) */
	uint16_t	biPlanes;		/* �v���[���� ��ɂP */
	uint16_t	biBitCount;		/* �P��f������̃f�[�^�T�C�Y */
	uint64_t	biCompression;	/* ���k�`�� */
	uint64_t	biSizeImage;	/* �摜�f�[�^���̃T�C�Y */
	int64_t		biXPixPerMeter;	/* �������𑜓x(dot/m) */
	int64_t		biYPixPerMeter;	/* �c�����𑜓x(dot/m) */
	uint64_t	biClrUsed;		/* �i�[����Ă���p���b�g��(�g�p�F��) */
	uint64_t	biClrImportant;	/* �d�v�ȃp���b�g�̃C���f�b�N�X */
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	uint8_t	rgbBlue;		/* �O�`�Q�T�T */
	uint8_t	rgbGreen;		/* �΂O�`�Q�T�T */
	uint8_t	rgbRed;			/* �ԂO�`�Q�T�T */
	uint8_t	rgbReserved;	/* �\��̈�O */
} RGBQUAD;

#pragma pack (pop)

typedef struct tagPICIMAGE {
	BITMAPFILEHEADER *pBMf;
	BITMAPINFOHEADER *pBMi;
	uint16_t	*pImageData;
} PICIMAGE;

typedef struct tagCG_LIST {
	int8_t	bFileName[256];		/* �p�X */
	int8_t	bType;				/* 0:�ʏ�A1:�X�v���C�g���C�N */
	int16_t	TransPal;			/* ���ߐF�p���b�g�ԍ� */
} CG_LIST;

/* �O���Q�� �ϐ� */
extern	volatile uint16_t *VIDEO_REG1;
extern	volatile uint16_t *VIDEO_REG2;
extern	volatile uint16_t *VIDEO_REG3;

extern	uint32_t	g_CG_List_Max;
extern	CG_LIST		g_stCG_LIST[CG_MAX];

/* �O���Q�� �֐� */
extern	int16_t	Get_CG_FileList_MaxNum(uint32_t *);
extern	uint8_t	*Get_CG_FileBuf(uint8_t);
extern	uint16_t	*Get_PicImageInfo(uint8_t , uint32_t *, uint32_t *, uint32_t *);
extern	int16_t	Get_PicImagePallet(uint8_t);
extern	int16_t	Set_PicImagePallet(uint8_t);
extern	int16_t Set_PicImagePalletALL(void);
extern	void Set_Pallet_Shift(int16_t, int16_t);
extern	int16_t CG_File_Load(uint16_t);
extern	void	G_INIT(void);
extern	void	G_VIDEO_INIT(void);
extern	void	G_VIDEO_PRI(uint8_t);
extern	void	G_HOME(uint8_t);
extern	void	G_VIEW(uint8_t);
extern	void	G_Palette_INIT(void);
extern	void	G_PaletteSetZero(void);
extern	void	G_PaletteSet(int16_t, int16_t);
extern	void	G_Palette_HALF(void);
extern	int16_t	G_Stretch_Pict( int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t, int16_t, uint16_t, uint8_t );
extern	int16_t G_Stretch_Pict_toVRAM(	int16_t, int16_t, uint8_t, uint8_t, uint8_t , uint8_t , uint16_t *, uint16_t, uint16_t, int8_t, uint8_t);
extern	int16_t G_Stretch_Pict_To_Mem( uint16_t	*, uint16_t, uint16_t, uint16_t	*, uint16_t, uint16_t);
extern	int16_t G_Copy_Pict_To_Mem(	uint16_t *, uint16_t , uint16_t , uint16_t *, uint16_t , uint16_t);
extern	int16_t	G_BitBlt(int16_t , uint16_t , int16_t , uint16_t , uint8_t , int16_t , uint16_t , int16_t , uint16_t , uint8_t , uint8_t , uint8_t , uint8_t );
extern	int16_t G_BitBlt_From_Mem(	int16_t, int16_t , uint8_t , uint16_t *, uint16_t , uint16_t , uint8_t , uint8_t , uint8_t, uint8_t );
extern	int32_t G_PAGE_SET(uint16_t);
extern	int32_t	G_CLR(void);
extern	int16_t G_CLR_HS(void);
extern	int16_t	G_CLR_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t);
extern	int16_t	G_CLR_ALL_OFFSC(uint8_t);
extern	int16_t	G_FILL_AREA(int16_t, uint16_t, int16_t, uint16_t, uint8_t, uint8_t);
extern	int16_t	G_Load(uint8_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Load_Mem(uint8_t, int16_t, int16_t, uint16_t);
extern	int16_t	APICG_DataLoad2G(int8_t *, uint64_t, uint64_t, uint16_t);
extern	int16_t	APICG_DataLoad2M(uint8_t, uint64_t, uint64_t, uint16_t, uint16_t *);
extern	int16_t	G_Subtractive_Color(uint16_t *, uint16_t *, uint16_t, uint16_t, uint16_t, uint32_t);
extern	int16_t	PutGraphic_To_Text(uint8_t , uint16_t , uint16_t, uint16_t);
extern	int16_t	PutGraphic_To_Symbol12(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol16(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t	PutGraphic_To_Symbol24(uint8_t *, uint16_t , uint16_t , uint16_t );
extern	int16_t PutGraphic_To_Symbol24_Xn(uint8_t *, uint16_t, uint16_t, uint16_t, uint16_t);
extern	int16_t	G_Scroll(int16_t, int16_t, uint8_t);

#endif	/* IF_GRAPHIC_H */
