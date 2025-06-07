#ifndef	MAIN_C
#define	MAIN_C

#include <iocslib.h>
#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <io.h>
#include <math.h>
#include <time.h>
#include <interrupt.h>

#include <usr_macro.h>
#include <apicglib.h>

#include "main.h"

#include "BIOS_CRTC.h"
#include "BIOS_DMAC.h"
#include "BIOS_MFP.h"
#include "BIOS_PCG.h"
#include "BIOS_MPU.h"
#include "IF_Draw.h"
#include "IF_FileManager.h"
#include "IF_Graphic.h"
#include "IF_Input.h"
#include "IF_MACS.h"
#include "IF_Math.h"
#include "IF_Memory.h"
#include "IF_Mouse.h"
#include "IF_Music.h"
#include "IF_PCG.h"
#include "IF_Text.h"
#include "IF_Task.h"
#include "APL_PCG.h"
#include "APL_Score.h"

#include "APL_ZooKeeper.h"

//#define 	W_BUFFER	/* �_�u���o�b�t�@�����O���[�h */
//#define	FPS_MONI	/* FPS�v�� */
//#define	CPU_MONI	/* CPU�v�� */
#define	MEM_MONI	/* MEM�v�� */

#define		GAMEPLAY_TIME				(90000)
#define		GAME_STOP_COUNT				(32)

#define		DEMO_LOGO_INTERVAL_TIME		(1000)
#define		DEMO_START_END_TIME			(30000)
#define		DEMO_GAMEOVER_TIME			(5000)

/* �O���[�o���ϐ� */
uint32_t	g_unTime_cal = 0u;
uint32_t	g_unTime_cal_PH = 0u;
uint32_t	g_unTime_Pass = 0xFFFFFFFFul;
int32_t		g_nCrtmod = 0;
int32_t		g_nMaxUseMemSize;
int16_t		g_CpuTime = 0;
uint8_t		g_mode;
uint8_t		g_mode_rev;
uint8_t		g_Vwait = 1;
uint8_t		g_bFlip = FALSE;
uint8_t		g_bFlip_old = FALSE;
uint16_t	g_unFrameCount;
#ifdef FPS_MONI
uint8_t		g_bFPS_PH = 0u;
#endif
uint8_t		g_bExit = TRUE;
int16_t		g_GameSpeed;
static uint32_t s_PassTime;
static uint32_t s_DemoTime;
static uint16_t s_music_tempo;
static uint16_t s_music_tempo_old;
uint8_t		g_ubDemoPlay = FALSE;
uint8_t		g_ubPhantomX = FALSE;


/* �O���[�o���\���� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
uint16_t	g_uDebugNum = 0;
//uint16_t	g_uDebugNum = (Bit_7|Bit_4|Bit_0);
#endif

/* �֐��̃v���g�^�C�v�錾 */
int16_t main(int16_t, int8_t**);
static void App_Init(void);
static void App_exit(void);
int16_t	BG_main(uint32_t);
void App_TimerProc( void );
int16_t App_RasterProc( uint16_t * );
void App_VsyncProc( void );
void App_HsyncProc( void );
int16_t	App_FlipProc( void );
int16_t	SetFlip(uint8_t);
int16_t	GetGameMode(uint8_t *);
int16_t	SetGameMode(uint8_t);

void (*usr_abort)(void);	/* ���[�U�̃A�{�[�g�����֐� */

/* �֐� */
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t main_Task(void)
{
	int16_t	ret = 0;

	int16_t	rank = -1;
	int16_t	reset = -1;
	uint16_t	uFreeRunCount=0u;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	uint32_t	unTime_cal = 0u;
	uint32_t	unTime_cal_PH = 0u;
#endif
	static int32_t y1 = 3, x1 = 3;
	static int32_t y2 = 3, x2 = 4;

	int16_t	loop;
	int16_t	pushCount = 0;

	int8_t	bStage = 0;
	uint8_t	bMode;
	
	ST_TASK		stTask = {0}; 
	ST_CRT		stCRT;
	
	/* �ϐ��̏����� */
#ifdef W_BUFFER
	SetGameMode(1);
#else
	SetGameMode(0);
#endif	
	loop = 1;
	g_unFrameCount = 0;
	
	s_music_tempo = 300;
	s_music_tempo_old = 300;

	/* ���� */
	srandom(TIMEGET());	/* �����̏����� */
	
	do	/* ���C�����[�v���� */
	{
		uint32_t time_st;
#if 0
		ST_CRT	stCRT;
#endif
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		uint32_t time_now;
		GetNowTime(&time_now);
#endif

#ifdef DEBUG
//		Draw_Box(	stCRT.hide_offset_x + X_POS_MIN - 1,
//					stCRT.hide_offset_y + Y_POS_BD + 1,
//					stCRT.hide_offset_x + X_POS_MAX - 1,
//					stCRT.hide_offset_y + g_TB_GameLevel[g_nGameLevel] + 1,
//					G_PURPLE, 0xFFFF);
#endif
		PCG_START_SYNC(g_Vwait);	/* �����J�n */

		/* �I������ */
		if(loop == 0)
		{
			break;
		}
		
		/* �����ݒ� */
		GetNowTime(&time_st);	/* ���C�������̊J�n�������擾 */
		SetStartTime(time_st);	/* ���C�������̊J�n�������L�� */
		
		/* �^�X�N���� */
		TaskManage();				/* �^�X�N���Ǘ����� */
		GetTaskInfo(&stTask);		/* �^�X�N�̏��𓾂� */

		/* ���[�h�����n�� */
		GetGameMode(&bMode);
		GetCRT(&stCRT, bMode);	/* ��ʍ��W�擾 */

#if CNF_VDISP
		/* V-Sync�œ��� */
#else
		if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
		{
		}
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
//		DirectInputKeyNum(&g_uDebugNum, 3);	/* �L�[�{�[�h���琔������� */
#endif

		if((GetInput() & KEY_b_ESC ) != 0u)	/* �d�r�b�L�[ */
		{
			/* �I�� */
			pushCount = Minc(pushCount, 1);
			if(pushCount >= 5)
			{
				SetTaskInfo(SCENE_EXIT);	/* �I���V�[���֐ݒ� */
			}
		}
		else if((GetInput() & KEY_b_HELP ) != 0u)	/* HELP�L�[ */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* ���Z�b�g */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 6)
				{

				}
				else if(pushCount >= 5)
				{
					ADPCM_Play(SE_OK);	/* ���艹 */

					Music_Stop();	/* Stop */

					SetTaskInfo(SCENE_INIT);	/* �I���V�[���֐ݒ� */
				}
			}
		}		
		else if((GetInput() & KEY_b_Q ) != 0u)	/* Q�L�[ */
		{
			if( (stTask.bScene != SCENE_GAME_OVER_S) && (stTask.bScene != SCENE_GAME_OVER) )
			{
				/* ���Z�b�g */
				pushCount = Minc(pushCount, 1);
				if(pushCount >= 6)
				{

				}
				else if(pushCount >= 5)
				{
					ADPCM_Play(SE_OK);	/* ���艹 */

					S_Init_Score();
					S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */

					S_Set_ScoreMode(FALSE);	/* �_���v�Z���� */
				}
			}
		}		
		else
		{
			pushCount = 0;
		}

		switch(stTask.bScene)
		{
			case SCENE_INIT:	/* �������V�[�� */
			{
				T_Clear();		/* �e�L�X�g�N���A */

				SetTaskInfo(SCENE_TITLE_S);	/* �V�[��(�J�n����)�֐ݒ� */
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_TITLE_S:
			{
				int8_t sStr[12] = {0};

				Music_Play(BGM_TITLE);

				Set_CRT_Contrast(0);	/* �^���Âɂ��� */
				G_VIDEO_PRI(3);	/* TX>SP>GR */
				G_PAGE_SET(0b0010);	/* GR1 */
			    PCG_VIEW(0x00);        /* SP OFF / BG0 OFF / BG1 OFF */

				if(G_Load(1, 0, 0, 1) < 0)	/* 256�F �y�[�W1 */
				{
					T_Clear();		/* �e�L�X�g�N���A */
				}

				S_Init_Score();
				S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */
				S_Set_ScoreMode(TRUE);	/* �_���v�Z�L�� */
				
				BG_TextPut("PUSH A BUTTON TO START!", 40, 224);

				BG_TextPut("X68ZKEEPER 2025", 0, 248);
				sprintf(sStr, "VER%d.%d.%d", MAJOR_VER, MINOR_VER, PATCH_VER);
				BG_TextPut(sStr, 192, 248);

				Set_CRT_Contrast(-1);	/* ���Ƃɖ߂� */

				SetTaskInfo(SCENE_TITLE);	/* �V�[��(����)�֐ݒ� */
				break;
			}
			case SCENE_TITLE:
			{
				static int8_t s_bRelese = FALSE;
				if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
					((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
					((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
				{
					s_bRelese = TRUE;
				}
				else
				{
					if(s_bRelese == TRUE)
					{
						ADPCM_Play(SE_OK);	/* ���艹 */

						s_bRelese = FALSE;
						SetTaskInfo(SCENE_TITLE_E);	/* �V�[���ݒ� */
					}
				}

				break;
			}
			case SCENE_TITLE_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					BG_TextPut("                       ", 40, 224);

					G_PAGE_SET(0b0010);	/* GR1 */
					if(G_Load(2, 0, 0, 1) < 0)	/* 256�F �y�[�W1*/
					{
						T_Clear();		/* �e�L�X�g�N���A */
					}
					G_Palette_HALF();	/* �p���b�g��ݒ�l���甼���ɂ��܂� */

					PCG_VIEW(0x07);        /* SP ON / BG0 ON / BG1 ON */
			
					APL_ZKP_SP_init();

					bStage = 0;
					reset = 0;
					APL_ZKP_set_GameLv(bStage);	/* �Q�[�����x�����Z�b�g */

					SetTaskInfo(SCENE_START_S);	/* �V�[���ݒ� */
				}
				break;
			}
			case SCENE_START_S:
			{
				Music_Play(BGM_STAGE_1);

				SetTaskInfo(SCENE_START);	/* �V�[���ݒ� */
				break;
			}
			case SCENE_START:
			{
				ADPCM_Play(SE_GAME_START);

#if CNF_MACS
				PCG_OFF();			/* SP OFF */
				/* ���� */
				MOV_Play2(MOV_GAME_START, 0x64);		/* �J�b�g�C�� */
				/* ��ʃ��[�h�Đݒ� */
				CRTC_INIT(0x100 + 0x0A);	/* ��ʃ��[�h�������@�O���t�B�b�N�������Ȃ� */
				PCG_ON();			/* SP ON */
#else
#endif
				SetTaskInfo(SCENE_START_E);	/* �V�[���ݒ� */
				break;
			}
			case SCENE_START_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* �V�[���ݒ� */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME1_S:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					switch(reset)
					{
						case 0:	/* �z�u�A�^�C�}�[�ƃJ�E���^���ƃ��Z�b�g */
						{
							APL_ZKP_init_board();
							//if (APL_ZKP_swap_if_valid(0, 0, 0, 1))
							{
								// ���������ꍇ�͏����𑱍s�i����?����?��[�j
								APL_ZKP_process_all_matches();
							}

							APL_ZKP_init_animal_counter();

							GetNowTime(&s_PassTime);	/* �^�C���A�E�g�J�E���^���Z�b�g */
							break;
						}
						case 1:	/* �^�C�}�[�ƃJ�E���^���Z�b�g */
						{
							APL_ZKP_init_animal_counter();

							GetNowTime(&s_PassTime);	/* �^�C���A�E�g�J�E���^���Z�b�g */
							break;
						}
						case 2:	/* �z�u���Z�b�g */
						{
							APL_ZKP_init_board();
							//if (APL_ZKP_swap_if_valid(0, 0, 0, 1))
							{
								// ���������ꍇ�͏����𑱍s�i����?����?��[�j
								APL_ZKP_process_all_matches();
							}
							break;
						}
						default:
						{
							break;
						}
					}

					if(APL_ZKP_has_valid_moves() == 0){	/* �L�����T�� */
						reset = 2;
					}
					else
					{
						reset = 3;
						APL_ZKP_print_board();

						SetTaskInfo(SCENE_GAME1);	/* �V�[���ݒ� */
					}
				}
				break;
			}
			case SCENE_GAME1:
			{
				uint32_t cal;
				uint32_t time_now_pos;

				GetNowTime(&time_now_pos);
			    if(S_Get_ScoreMode() == FALSE)s_PassTime = time_now_pos;  /* �_���v�Z��������� */
				cal = Mmax(0, (time_now_pos - s_PassTime));

				if(APL_ZKP_KEY_Input(&y1, &x1, &y2, &x2) != 0u)
				{
					ADPCM_Play(SE_EXCHANGE);

					if (APL_ZKP_swap_if_valid(y1, x1, y2, x2)) {
						// ����
						SetTaskInfo(SCENE_GAME2_S);	/* �V�[���ݒ� */
					} else {
						// ���s�Ȃ�Ֆʂ��̂܂܂Ń��X�^�C��
						SetTaskInfo(SCENE_GAME3_S);	/* �V�[���ݒ� */
					}
				}

				APL_ZKP_print_board();
				APL_ZKP_SP_cursor_view(y1, x1, y2, x2);	/* �J�[�\���\�� */

				if( GetPassTime(GAMEPLAY_TIME, &s_PassTime) != 0u)	/* �^�C���I�[�o�[ */
				{
					if(bStage > 5)	/* +5�ʃN���A(+5) */
					{
						Music_Play(BGM_GAME_CLEAR);

						S_Clear_Score();	/* �X�R�A�F�X�e�[�W�N���A�{�[�i�X */
					}
					else
					{
					}
					SetTaskInfo(SCENE_GAME4_S);	/* �V�[��(���{����)�֐ݒ� */
				}
				else if(APL_ZKP_stage_clear_chk() != 0u)
				{
					reset = 0;
					if(bStage <= (BGM_STAGE_5 - BGM_STAGE_1))
					{
						bStage++;
					}
					APL_ZKP_set_GameLv(bStage);  /* 1,2,3,4*/
					
					ADPCM_Play(SE_GAME_CLEAR);

					Music_Play(BGM_STAGE_1 + bStage);  /* 1,2,3,4 */

					S_Add_Score_Point((GAMEPLAY_TIME - cal) / 10);	/* �c�莞��[s]*100 */

#if CNF_MACS
					PCG_OFF();			/* SP OFF */
					/* ���� */
					MOV_Play2(MOV_GAME_CLEAR, 0x64);		/* �J�b�g�C�� */
					/* ��ʃ��[�h�Đݒ� */
					CRTC_INIT(0x100 + 0x0A);	/* ��ʃ��[�h�������@�O���t�B�b�N�������Ȃ� */
					PCG_ON();			/* SP ON */
#else
#endif
					SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(���{����)�֐ݒ� */
				}
				else
				{
					/* �������Ȃ� */
				}

				break;
			}
			case SCENE_GAME1_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);	/* �V�[���ݒ� */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME2_S:
			{
				// ���������ꍇ�͏����𑱍s�i����?����?��[�j
				if(APL_ZKP_SP_Swap_view(TRUE) != 0u)
				{
					SetTaskInfo(SCENE_GAME2);	/* �V�[���ݒ� */
				}

				break;
			}
			case SCENE_GAME2:
			{
				APL_ZKP_check_matches();

				if(APL_ZKP_check_matches_chk() != 0u)
				{
					uint32_t time_now_pos;
					GetNowTime(&time_now_pos);
					s_PassTime = Mmin(time_now_pos, s_PassTime + 2000);	/* +2s ���Z */

					ADPCM_Play(Mmin(SE_COMBO_1 + S_Get_Combo(), SE_COMBO_8));

					S_Set_Combo(S_Get_Combo() + 1);

					SetTaskInfo(SCENE_GAME2_0);	/* �V�[���ݒ� */
				}
				else
				{
					SetTaskInfo(SCENE_GAME2_E);	/* �V�[���ݒ� */
				}
				break;
			}
			case SCENE_GAME2_0:
			{
				static int32_t s_Clear_StepCount = 0;
		//      APL_ZKP_print_clear_map();
				/* �����ĂO�ɂ��� */
				if(APL_ZKP_apply_clear_step() == 0u)// �A������{���Ɏg��
				{
					if(ADPCM_SNS() == 0){	/* ���ʉ���~�� */
						s_Clear_StepCount++;
					}

					if(s_Clear_StepCount >= 20)
					{
						s_Clear_StepCount = 0;
					
						SetTaskInfo(SCENE_GAME2_1);	/* �V�[���ݒ� */
					}
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_1:
			{
				/* �l�ߍ��� */
				if(APL_ZKP_apply_gravity_step() == 0u)
				{
					SetTaskInfo(SCENE_GAME2_2);	/* �V�[���ݒ� */
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_2:
			{
				/* �󂫂𖄂߂� */
				if(APL_ZKP_refill_board_step() == 0u)
				{
					SetTaskInfo(SCENE_GAME2_3);	/* �V�[���ݒ� */
				}
				APL_ZKP_print_board();
				break;
			}
			case SCENE_GAME2_3:
			{
#if 0				
				/* ��������ւ� st */
				if(APL_ZKP_KEY_Input(&y1, &x1, &y2, &x2) != 0u)
				{
					ADPCM_Play(SE_EXCHANGE);
				}
				APL_ZKP_swap_force(y1, x1, y2, x2);
				APL_ZKP_SP_cursor_view(y1, x1, y2, x2);	/* �J�[�\���\�� */
				/* ��������ւ� ed */
#endif
				APL_ZKP_print_board();

				SetTaskInfo(SCENE_GAME2);	/* �V�[���ݒ� */
				break;
			}
			case SCENE_GAME2_E:
			{

				if(APL_ZKP_has_valid_moves() == 0){

					S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */

					S_Add_Score_Point(1000);	/* 1000�_ */

#if CNF_MACS
					PCG_OFF();			/* SP OFF */
					/* ���� */
					MOV_Play2(MOV_NOMOREMOVE, 0x64);		/* �J�b�g�C�� */
					/* ��ʃ��[�h�Đݒ� */
					CRTC_INIT(0x100 + 0x0A);	/* ��ʃ��[�h�������@�O���t�B�b�N�������Ȃ� */
					PCG_ON();			/* SP ON */
#else
#endif
					ADPCM_Play(SE_NOMOREMOVE);

					reset = 2;
				}
				else
				{
					if(S_Get_Combo() >= 3)
					{
#if CNF_MACS
						PCG_OFF();			/* SP OFF */
						/* ���� */
						MOV_Play2(MOV_EXCELLENT, 0x64);		/* �J�b�g�C�� */
						/* ��ʃ��[�h�Đݒ� */
						CRTC_INIT(0x100 + 0x0A);	/* ��ʃ��[�h�������@�O���t�B�b�N�������Ȃ� */
						PCG_ON();			/* SP ON */
#else
#endif
					}
					S_Set_Combo(0);	/* �R���{�J�E���^ ���Z�b�g */
					ADPCM_Play(SE_EXCELLENT);
					APL_ZKP_print_board();
				}
				SetTaskInfo(SCENE_GAME1_S);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME3_S:
			{
				APL_ZKP_print_board();

				ADPCM_Play(SE_MISS);

				SetTaskInfo(SCENE_GAME3);	/* �V�[���ݒ� */
				break;
			}
			case SCENE_GAME3:
			{
				if(APL_ZKP_SP_Swap_view(FALSE) != 0u)
				{
					SetTaskInfo(SCENE_GAME3_E);	/* �V�[���ݒ� */
				}
				break;
			}
			case SCENE_GAME3_E:
			{
				APL_ZKP_print_board();
				
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					SetTaskInfo(SCENE_GAME1_S);	/* �V�[���ݒ� */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME4_S:
			{
				APL_ZKP_print_board();

				Music_Play(BGM_STOP);	/* Stop */

				ADPCM_Play(SE_MISS);

				SetTaskInfo(SCENE_GAME4);	/* �V�[���ݒ� */
				break;
			}
			case SCENE_GAME4:
			{
				static int16_t s_wait = GAME_STOP_COUNT;

				APL_ZKP_print_valid_moves();	/* ���\�� */
				s_wait--;
				
				if(s_wait <= 0)	/* ���ʉ���~�� */
				{
					s_wait = GAME_STOP_COUNT;
					SetTaskInfo(SCENE_GAME4_E);	/* �V�[���ݒ� */
				}
				break;
			}
			case SCENE_GAME4_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					APL_ZKP_ALL_clear();
					APL_ZKP_print_board();
					APL_ZKP_SP_cursor_view_off();	/* �J�[�\��OFF */
					
					T_Fill(48, 40, 208, 4, 0xFFFF, 0);	/* ���� */

					if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
					{
						SetTaskInfo(SCENE_HI_SCORE_S);	/* �V�[��(���{����)�֐ݒ� */
					}
					else
					{
						SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
					}
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_HI_SCORE_S:	/* �n�C�X�R�A�����L���O�V�[��(�J�n����) */
			{
				T_Clear();	/* �e�L�X�g�N���A */

				G_VIDEO_PRI(0);	/* GR>TX>SP */

				G_PAGE_SET(0b0001);	/* GR0 */
				
				G_PaletteSetZero();
				G_CLR_AREA(0, 256, 0, 256, 0);	/* �y�[�W 0 */
				G_CLR_AREA(0, 256, 0, 256, 1);	/* �y�[�W 1 */

				rank = S_Score_Board();	/* �n�C�X�R�A�\�� */
				
				if(rank >= 0)
				{
					Music_Play(BGM_HISCORE);	/* Hi-SCORE */
				}
				else
				{
					Music_Play(BGM_SCORE);	/* SCORE */
				}

				SetTaskInfo(SCENE_HI_SCORE);	/* �n�C�X�R�A�����L���O�V�[���֐ݒ� */
				break;
			}
			case SCENE_HI_SCORE:	/* �n�C�X�R�A�����L���O�V�[�� */
			{
				if(rank >= 0)
				{
					if(S_Score_Name_Main(rank) >= 3)	/* �X�R�A�l�[������ */
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
					}
				}
				else
				{
					if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
					{
						if(	((GetInput_P1() & JOY_A ) != 0u)	||		/* A */
							((GetInput() & KEY_b_Z) != 0u)		||		/* A(z) */
							((GetInput() & KEY_b_SP ) != 0u)		)	/* �X�y�[�X�L�[ */
						{
							ADPCM_Play(SE_OK);	/* ���艹 */
							SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
						}
					}
					else
					{
						SetTaskInfo(SCENE_HI_SCORE_E);	/* �n�C�X�R�A�����L���O�V�[��(�I������)�֐ݒ� */
					}
				}
				break;
			}
			case SCENE_HI_SCORE_E:	/* �n�C�X�R�A�����L���O�V�[��(�I������) */
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					SetTaskInfo(SCENE_GAME_OVER_S);		/* �Q�[���I�[�o�[�V�[��(�J�n����)�֐ݒ� */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			case SCENE_GAME_OVER_S:
			{
				int16_t x, y;

				APL_ZKP_SP_cursor_view_off();	/* �J�[�\��OFF */

				T_Clear();	/* �e�L�X�g�N���A */

				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					Music_Play(BGM_GAME_OVER);	/* Game Over */
					ADPCM_Play(SE_GAME_OVER);
				}
				else
				{
					G_VIDEO_PRI(0);	/* GR>TX>SP */

					G_PAGE_SET(0b0001);	/* GR0 */

					G_CLR_AREA(0, 255, NAME_INP_Y, 255, 0);	/* �y�[�W 0 */
					G_CLR_AREA(0, 256, 0, 256, 1);	/* �y�[�W 1 */
					
					S_Score_Board();	/* �n�C�X�R�A�\�� */

					GetNowTime(&s_DemoTime);	/* �f���J�n�J�E���^���Z�b�g */
				}

				x = stCRT.view_offset_x + 76;
				y = stCRT.view_offset_y + 0;
				Draw_Fill( x, y + 8, x + 12*9, y + 24*1 + 6, G_BLACK);	/* �h��Ԃ� */
				Draw_Box( x, y + 8, x + 12*9, y + 24*1 + 6, G_D_RED, 0xFFFF);	/* �g */
				PutGraphic_To_Symbol24("GAME OVER", x + 1, y + 1 + 8, G_RED);
				PutGraphic_To_Symbol24("GAME OVER", x, y + 8, G_L_PINK);

				SetTaskInfo(SCENE_GAME_OVER);	/* �V�[��(���{����)�֐ݒ� */
				break;
			}
			case SCENE_GAME_OVER:
			{
				/* �w�i���� */
				if(g_ubDemoPlay == FALSE)	/* �f���ȊO */
				{
					if(	((GetInput_P1() & JOY_A ) != 0u)	||	/* A�{�^�� */
						((GetInput() & KEY_b_Z) != 0u)		||	/* KEY A(z) */
						((GetInput() & KEY_b_SP ) != 0u)	)	/* �X�y�[�X�L�[ */
					{
						ADPCM_Play(SE_OK);	/* ���艹 */
						SetTaskInfo(SCENE_GAME_OVER_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}
				else
				{
					if(	GetPassTime(DEMO_GAMEOVER_TIME, &s_DemoTime) != 0u)	/* 5000ms�o�� */
					{
						SetTaskInfo(SCENE_GAME_OVER_E);	/* �V�[��(�I������)�֐ݒ� */
					}
				}

				break;
			}
			case SCENE_GAME_OVER_E:
			{
				if(ADPCM_SNS() == 0)	/* ���ʉ���~�� */
				{
					G_CLR_AREA(0, 255, 0, 255, 0);	/* �y�[�W 0 */
					G_CLR_AREA(0, 256, 0, 256, 1);	/* �y�[�W 1 */
					SetTaskInfo(SCENE_TITLE_S);	/* �V�[��(����������)�֐ݒ� */
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////////
			default:	/* �ُ�V�[�� */
			{
				ADPCM_Play(SE_GAME_END);

				loop = 0;	/* ���[�v�I�� */
				break;
			}
		}
		SetFlip(TRUE);	/* �t���b�v���� */

		PCG_END_SYNC(g_Vwait);	/* �X�v���C�g�̏o�� */

		uFreeRunCount++;	/* 16bit �t���[�����J�E���^�X�V */
		g_unFrameCount++;	/* 16bit �t���[�����J�E���^�X�V */

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
		/* �������Ԍv�� */
		GetNowTime(&time_now);
		unTime_cal = time_now - time_st;	/* LSB:1 UNIT:ms */
		g_unTime_cal = unTime_cal;
		if( stTask.bScene == SCENE_GAME1 )
		{
			unTime_cal_PH = Mmax(unTime_cal, unTime_cal_PH);
			g_unTime_cal_PH = unTime_cal_PH;
		}
#endif

#ifdef MEM_MONI	/* �f�o�b�O�R�[�i�[ */
		GetFreeMem();	/* �󂫃������T�C�Y�m�F */
#endif

	}
	while( loop );

	g_bExit = FALSE;

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void App_Init(void)
{
	uint32_t nNum;
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �J�n");
#endif
#ifdef MEM_MONI	/* �f�o�b�O�R�[�i�[ */
	g_nMaxUseMemSize = GetFreeMem();
	printf("FreeMem(%d[kb])\n", g_nMaxUseMemSize);	/* �󂫃������T�C�Y�m�F */
	puts("App_Init ������");
#endif

	Set_SupervisorMode();	/* �X�[�p�[�o�C�U�[���[�h */
	/* MFP */
	MFP_INIT();	/* V-Sync���荞�ݓ��̏��������� */
	Set_UserMode();			/* ���[�U�[���[�h */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("App_Init MFP(%d)\n", Get_CPU_Time());
#endif
	if(SetNowTime(0) == FALSE)
	{
		/*  */
	}
	nNum = Get_CPU_Time();	/* 300 = 10MHz� */
	if(nNum  < 400)
	{
		g_GameSpeed = 0;
		printf("Normal Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 640)
	{
		g_GameSpeed = 2;
		printf("XVI Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 800)
	{
		g_GameSpeed = 4;
		printf("RedZone Speed(%d)\n", g_GameSpeed);
	}
	else if(nNum < 2000)
	{
		g_GameSpeed = 7;
		printf("030 Speed(%d)\n", g_GameSpeed);
	}
	else
	{
		g_GameSpeed = 10;
		printf("Another Speed(%d)\n", g_GameSpeed);
	}
	puts("App_Init Input");
	Input_Init();			/* ���͏����� */

	puts("App_Init FileList");
	Init_FileList_Load();	/* ���X�g�t�@�C���̓ǂݍ��� */

	puts("App_Init Music");
	/* ���y */
	Init_Music();	/* ������(�X�[�p�[�o�C�U�[���[�h���O)	*/

	Music_Play(BGM_INIT);	/* Init */
	Music_Play(BGM_STOP);	/* Stop */
	Music_Play(BGM_LOAD);	/* Loading */

	/* �X�[�p�[�o�C�U�[���[�h�J�n */
	Set_SupervisorMode();

	/* ��� */
//	g_nCrtmod = CRTC_INIT(0);	/* mode=0 512x512 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(1);	/* mode=1 512x512 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(2);	/* mode=2 256x256 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(3);	/* mode=3 256x256 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(4);	/* mode=4 512x512 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(5);	/* mode=5 512x512 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(6);	/* mode=6 256x256 col:16/16 31kHz */
//	g_nCrtmod = CRTC_INIT(7);	/* mode=7 256x256 col:16/16 15kHz */
//	g_nCrtmod = CRTC_INIT(8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(0x100+8);	/* mode=8 512x512 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(9);	/* mode=9 512x512 col:16/256 15kHz */
	g_nCrtmod = CRTC_INIT(10);	/* mode=10 256x256 col:16/256 31kHz */
//	g_nCrtmod = CRTC_INIT(11);	/* mode=11 256x256 col:16/256 15kHz */
//	g_nCrtmod = CRTC_INIT(12);	/* mode=12 512x512 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(13);	/* mode=13 512x512 col:16/65536 15kHz */
//	g_nCrtmod = CRTC_INIT(14);	/* mode=14 256x256 col:16/65536 31kHz */
//	g_nCrtmod = CRTC_INIT(15);	/* mode=15 256x256 col:16/65536 15kHz */
//	g_nCrtmod = CRTC_INIT(16);	/* mode=16 764x512 col:16 31kHz */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init CRTC(���)");
//	KeyHitESC();	/* �f�o�b�O�p */
#endif

	/* �O���t�B�b�N */
	G_INIT();			/* ��ʂ̏����ݒ� */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */
	G_HOME(0);			/* �z�[���ʒu�ݒ� */
	G_VIDEO_INIT();		/* �r�f�I�R���g���[���[�̏����� */
	G_PaletteSetZero();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �O���t�B�b�N");
#endif

	/* �X�v���C�g�^�a�f */
	PCG_INIT(PCG_NUM_MAX);	/* �X�v���C�g�^�a�f�̏����� */
	PCG_INIT_CHAR();	/* �X�v���C�g���a�f��`�Z�b�g */
    PCG_VIEW(1);        /* SP ON / BG0 OFF / BG1 OFF */
	BG_OFF(0);				
	BG_OFF(1);				
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �X�v���C�g�^�a�f");
#endif

	g_Vwait = 1;

#if CNF_MACS
	/* ���� */
	MOV_INIT();		/* ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Movie(MACS)");
#endif
#endif

	/* Task */
	TaskManage_Init();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init Task");
#endif
	
	/* �}�E�X������ */
	Mouse_Init();	/* �}�E�X������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �}�E�X");
#endif
	
	/* �e�L�X�g */
	T_INIT();	/* �e�L�X�g�u�q�`�l������ */
	T_PALET();	/* �e�L�X�g�p���b�g������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init T_INIT");
#endif

#if 1
	/* �X�R�A������ */
	S_All_Init_Score();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init S_All_Init_Score");
#endif
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_Init �I��");
#endif
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
static void App_exit(void)
{
	int16_t	ret = 0;

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �J�n");
#endif
	
	if(g_bExit == TRUE)
	{
		puts("�G���[���L���b�`�I ESC to skip");
		KeyHitESC();	/* �f�o�b�O�p */
	}
	g_bExit = TRUE;

	/* �O���t�B�b�N */
	G_CLR();			/* �N���b�s���O�G���A�S�J������ */

	/* �X�v���C�g���a�f */
	PCG_OFF();		/* �X�v���C�g���a�f��\�� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�v���C�g");
#endif

	/* MFP */
	ret = MFP_EXIT();				/* MFP�֘A�̉��� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("App_exit MFP(%d)\n", ret);
#endif

	/* ��� */
	CRTC_EXIT(0x100 + g_nCrtmod);	/* ���[�h�����Ƃɖ߂� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit ���");
#endif

	Mouse_Exit();	/* �}�E�X��\�� */

	/* �e�L�X�g */
	T_EXIT();				/* �e�L�X�g�I������ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �e�L�X�g");
#endif

	MyMfree(0);				/* �S�Ẵ���������� */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	printf("MaxUseMem(%d[kb])\n", g_nMaxUseMemSize - GetMaxFreeMem());
	puts("App_exit ������");
#endif

	_dos_kflushio(0xFF);	/* �L�[�o�b�t�@���N���A */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �L�[�o�b�t�@�N���A");
#endif

	/*�X�[�p�[�o�C�U�[���[�h�I��*/
	Set_UserMode();
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �X�[�p�[�o�C�U�[���[�h�I��");
#endif
	/* ���y */
	Exit_Music();			/* ���y��~ */
#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit Music");
#endif

#ifdef DEBUG	/* �f�o�b�O�R�[�i�[ */
	puts("App_exit �I��");
#endif
}


/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	App_FlipProc(void)
{
	int16_t	ret = 0;
	
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
	uint32_t time_now;
	static uint8_t	bFPS = 0u;
	static uint32_t	unTime_FPS = 0u;
#endif

	if(g_bFlip == FALSE)	/* �`�撆�Ȃ̂Ńt���b�v���Ȃ� */
	{
		return ret;
	}
	else
	{
#ifdef W_BUFFER
		ST_CRT		stCRT;
		GetCRT(&stCRT, g_mode);	/* ��ʍ��W�擾 */
#endif
		SetFlip(FALSE);			/* �t���b�v�֎~ */
					
#ifdef W_BUFFER
		/* ���[�h�`�F���W */
		if(g_mode == 1u)		/* �㑤���� */
		{
			SetGameMode(2);
		}
		else if(g_mode == 2u)	/* �������� */
		{
			SetGameMode(1);
		}
		else					/* ���̑� */
		{
			SetGameMode(0);
		}

		/* ��\����ʂ�\����ʂ֐؂�ւ� */
		G_HOME(g_mode);

		/* �����N���A */
		G_CLR_AREA(	stCRT.hide_offset_x, WIDTH,
					stCRT.hide_offset_y, HEIGHT, 0);
#endif
		
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
		bFPS++;
#endif
		ret = 1;
	}

#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
	GetNowTime(&time_now);
	if( (time_now - unTime_FPS) >= 1000ul )
	{
		g_bFPS_PH = bFPS;
		unTime_FPS = time_now;
		bFPS = 0;
	}
#endif

	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetFlip(uint8_t bFlag)
{
	int16_t	ret = 0;

	Set_DI();	/* ���荞�݋֎~ */
	g_bFlip_old = g_bFlip;	/* �t���b�v�O��l�X�V */
	g_bFlip = bFlag;
	Set_EI();	/* ���荞�݋��� */

	return ret;
}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void App_TimerProc( void )
{
	ST_TASK stTask;

	TaskManage();
	GetTaskInfo(&stTask);	/* �^�X�N�擾 */

	/* ������ ���̊Ԃɏ��������� ������ */
	if(stTask.b96ms == TRUE)	/* 96ms���� */
	{
		if(GetJoyAnalogMode() == TRUE)	/* �A�i���O���� */
		{
			if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
			{
				g_ubDemoPlay = FALSE;	/* �f������ */

				SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
			}
		}
		else	/* �f�W�^������ */
		{
		}
#if 1
		switch(stTask.bScene)
		{
			case SCENE_GAME1_S:
			case SCENE_GAME1:
			case SCENE_GAME1_E:
			case SCENE_GAME2_S:
			case SCENE_GAME2:
			case SCENE_GAME2_0:
			case SCENE_GAME2_1:
			case SCENE_GAME2_2:
			case SCENE_GAME2_3:
			case SCENE_GAME2_4:
			case SCENE_GAME2_E:
			case SCENE_GAME3_S:
			case SCENE_GAME3:
			case SCENE_GAME3_E:
			{
				S_Main_Score();	/* �X�R�A�\�� */

				break;
			}
			default:
				break;
		}
#endif
	}

	if(stTask.b496ms == TRUE)	/* 496ms���� */
	{
		switch(stTask.bScene)
		{
			case SCENE_GAME1_S:
			case SCENE_GAME1:
			case SCENE_GAME1_E:
			case SCENE_GAME2_S:
			case SCENE_GAME2:
			case SCENE_GAME2_0:
			case SCENE_GAME2_1:
			case SCENE_GAME2_2:
			case SCENE_GAME2_3:
			case SCENE_GAME2_4:
			case SCENE_GAME2_E:
			case SCENE_GAME3_S:
			case SCENE_GAME3:
			case SCENE_GAME3_E:
			{
#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
//				int8_t	sBuf[8];
#endif
				uint32_t cal;
				uint32_t time_now;
				GetNowTime(&time_now);
				cal = (time_now - s_PassTime);
				cal = Mmax(0, cal);
				T_Fill(48, 40, 160 - ((160 * cal) / GAMEPLAY_TIME), 4, 0xFFFF, T_YELLOW);
				T_Fill(Mmax(48,(208 - ((160 * cal) / GAMEPLAY_TIME))), 40, ((160 * cal) / GAMEPLAY_TIME), 4, 0xFFFF, T_RED);

#ifdef FPS_MONI	/* �f�o�b�O�R�[�i�[ */
				memset(sBuf, 0, sizeof(sBuf));
				sprintf(sBuf, "%3d", g_bFPS_PH);
				Draw_Message_To_Graphic(sBuf, 24, 24, F_MOJI, F_MOJI_BAK);	/* �f�o�b�O */
#endif
				break;
			}
			default:
				break;
		}
	}
	/* ������ ���̊Ԃɏ��������� ������ */

	/* �^�X�N���� */
	UpdateTaskInfo();		/* �^�X�N�̏����X�V */

}
/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t App_RasterProc( uint16_t *pRaster_cnt )
{
	int16_t	ret = 0;
#if CNF_RASTER
	RasterProc(pRaster_cnt);	/*�@���X�^�[���荞�ݏ��� */
#endif /* CNF_RASTER */
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
void App_VsyncProc( void )
{
	ST_TASK stTask;
//	puts("App_VsyncProc");
#if CNF_VDISP
	Timer_D_Less_NowTime();

	GetTaskInfo(&stTask);	/* �^�X�N�擾 */
	/* ������ ���̊Ԃɏ��������� ������ */

	if(GetJoyAnalogMode() == TRUE)	/* �A�i���O���� */
	{
	}
	else	/* �f�W�^������ */
	{
		if(Input_Main(g_ubDemoPlay) != 0u) 	/* ���͏��� */
		{
			g_ubDemoPlay = FALSE;	/* �f������ */

			SetTaskInfo(SCENE_INIT);	/* �V�[��(����������)�֐ݒ� */
		}
	}

	switch(stTask.bScene)
	{
		case SCENE_TITLE:
		{
			{
				int8_t x;
				int8_t y;
				int8_t left;
				int8_t right;
				
				Mouse_Cur_Off();
				Mouse_GetDataPos(&x, &y, &left, &right);

				if(x == 0)
				{

				}
				else if(x > 0)
				{
					SetInput(KEY_RIGHT);
				}
				else{
					SetInput(KEY_LEFT);
				}

				if(y == 0)
				{

				}
				else if(y > 0)
				{
					SetInput(KEY_LOWER);
				}
				else{
					SetInput(KEY_UPPER);
				}

				if(left != 0)
				{
					SetInput(KEY_b_X);
				}

				if(right != 0)
				{
					SetInput(KEY_b_Z);
				}


				//Mouse_SetPos(g_stPlayer.x, g_stPlayer.y);
			}
			break;
		}
		default:	/* �ُ�V�[�� */
		{
			break;
		}
	}
	/* ������ ���̊Ԃɏ��������� ������ */
	
	App_FlipProc();	/* ��ʐ؂�ւ� */

#endif /* CNF_VDISP */
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	GetGameMode(uint8_t *bMode)
{
	int16_t	ret = 0;
	
	*bMode = g_mode;
	
	return ret;
}

/*===========================================================================================*/
/* �֐���	�F	*/
/* ����		�F	*/
/* �߂�l	�F	*/
/*-------------------------------------------------------------------------------------------*/
/* �@�\		�F	*/
/*===========================================================================================*/
int16_t	SetGameMode(uint8_t bMode)
{
	int16_t	ret = 0;
	
	g_mode = bMode;
	if(bMode == 1)
	{
		g_mode_rev = 2;
	}
	else
	{
		g_mode_rev = 1;
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
int16_t main(int16_t argc, int8_t** argv)
{
	int16_t	ret = 0;

	/* COPY�L�[������ */
	init_trap12();
	/* ��O�n���h�����O���� */
	usr_abort = App_exit;	/* ��O�����ŏI�����������{���� */
	init_trap14();			/* �f�o�b�O�p�v���I�G���[�n���h�����O */
#if 0	/* �A�h���X�G���[���� */
	{
		char buf[10];
		int *A = (int *)&buf[1];
		int B = *A;
		return B;
	}
#endif

	App_Init();		/* ������ */
	
	main_Task();	/* ���C������ */

	App_exit();		/* �I������ */
	
	return ret;
}

#endif	/* MAIN_C */
