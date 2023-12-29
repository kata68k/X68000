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
	*�y���́za1.l = �t�@�C����������ւ̃|�C���^

	*�y�o�́zd0.l = �X�e�[�^�X
	*		  -1 : �o�^�p�̃o�b�t�@�ɋ󂫂��Ȃ�
	*		  -2 : FAT �̕��f����������
	*		 -64 : �o�b�t�@�T�C�Y���t�@�C�����傫��
	*		 -65 : �t�@�C����������Ȃ�
	*		-128 : �f�B�X�N I/O �̍Œ�������
	*		0�ȏ�: ����I��
	
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
	*	.MCS ���Đ�����
	*�y���́z
	*		a1.l = �t�@�C����������ւ̃|�C���^
	*		d3.w = �A�{�[�h�t���O
	*				���� MACS �f�[�^���Đ����̎��ɂ���𒆒f���čĐ����邩�ǂ���
	*		d3.w = 0 : �Đ����Ȃ�
	*		d3.w = 1 : ���f���čĐ�
	*
	*		d4.l = ������ʃt���O�iMACS�p�����[�^�j
	*
	*�y�o�́z
	*		d0.l = �X�e�[�^�X
	*			  -1�`-8 : MACS �G���[�R�[�h
	*				 -64 : �o�b�t�@�T�C�Y���t�@�C�����傫��
	*				 -65 : �t�@�C���̃I�[�v���Ɏ��s
	*				-128 : �f�B�X�N I/O �̍Œ�������
	*				0�ȏ�: ����I��
	*
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

*	lea.l	filename,a1
	movea.l	8(a6),a1
	moveq.l	#1,d3			* stop and play
*	moveq.l	#0,d4			* normal play
	moveq.l	#4,d4			* ������ʃt���O b0000 0100 
*	moveq.l	#$64,d4			* ������ʃt���O b0110 0100 
	moveq.l	#MOON_PLAY,d1
	IOCS	_MOON
	tst.l	d0
*	bmi	error
*			:
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts

_MoonPlay2:
	*	.MCS ���Đ�����
	*�y���́z
	*		a1.l = �t�@�C����������ւ̃|�C���^
	*		d3.w = �A�{�[�h�t���O
	*				���� MACS �f�[�^���Đ����̎��ɂ���𒆒f���čĐ����邩�ǂ���
	*		d3.w = 0 : �Đ����Ȃ�
	*		d3.w = 1 : ���f���čĐ�
	*
	*		d4.l = ������ʃt���O�iMACS�p�����[�^�j
	*
	*�y�o�́z
	*		d0.l = �X�e�[�^�X
	*			  -1�`-8 : MACS �G���[�R�[�h
	*				 -64 : �o�b�t�@�T�C�Y���t�@�C�����傫��
	*				 -65 : �t�@�C���̃I�[�v���Ɏ��s
	*				-128 : �f�B�X�N I/O �̍Œ�������
	*				0�ȏ�: ����I��
	*
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

*	lea.l	filename,a1
	movea.l	8(a6),a1
	moveq.l	#1,d3			* stop and play
*	moveq.l	#0,d4			* normal play
*	moveq.l	#4,d4			* ������ʃt���O b0000 0100 
	move.l	12(a6),d4		* ������ʃt���O ������
*	moveq.l	#$64,d4			* ������ʃt���O b0110 0100 
	moveq.l	#MOON_PLAY,d1
	IOCS	_MOON
	tst.l	d0
*	bmi	error
*			:
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts
	
_MACS_Play:
*	Input:	d1.w > �R�}���h�R�[�h		Output:  d0.l > Status
*		d2.l > �o�b�t�@�T�C�Y
*		d3.w > �A�{�[�g�t���O
*		d4.l > ������ʃt���O
*		a1.l > �����A�h���X
*	------------------------------------------------------------------------------
*		d1.w = 0	�A�j���[�V�����Đ�
*					(a1)����n�܂�A�j���f�[�^���Đ����܂��B
*		d2.l�͒ʏ�-1($FFFFFFFF)�ɂ��ĉ������B
*				�i�A�j���f�[�^�Đ����Ad2.l�̃G���A���z���悤�Ƃ�����,�Đ��𒆎~���܂��j
	link	a6,#0
	movem.l	d1-d4/a0-a1,-(sp)

	movea.l	8(a6),a1
	moveq.l	#0,d1			* play
	moveq.l	#-1,d2			* 
	moveq.l	#1,d3			* stop and play
	moveq.l	#4,d4			* ������ʃt���O b0000 0100 
*	moveq.l	#$64,d4			* ������ʃt���O b0110 0100 
	IOCS	_MACS
	tst.l	d0
	
	movem.l	(sp)+,d1-d4/a0-a1
	unlk	a6
	rts
	
_MACS_Vsync:
*	Input:	d1.w > �R�}���h�R�[�h		Output:  d0.l > Status
*			d2.w > �J�E���^
*			a1.l > �����A�h���X
*
*	d1.w = 10	�����������荞�݂�o�^���܂�
*				a1.l�Ɋ��荞�݃��[�`���̃A�h���X�A
*				d2.w�Ƀt���O*256+�J�E���^���Z�b�g���ăR�[�����܂��B
*				�t���O�͌��ݎg�p����Ă��܂���B0�ɂ��Ă����ĉ������B

	link	a6,#0
	movem.l	d1-d2/a0-a1,-(sp)

	move.w	sr,-(sp)		* �X�e�[�^�Xreg�ۑ�
	ori.w	#$0700,sr		* �S���荞�ݎ~��

	move.w	#$0*256+$1,d2
	move.w	#10,d1
	movea.l	8(a6),a1
	IOCS	_MACS
*	tst.l	d0

	tst.b	$E9A001		* 8255(�ޮ��è��)�̋�ǂ�
	nop			* ���O�܂ł̖��߃p�C�v���C���̓���
				* �����b�A���̖��ߏI���܂łɂ�
				* ����ȑO�̃o�X�T�C�N���Ȃǂ�
				* �������Ă��邱�Ƃ��ۏ؂����B
				
	move.w	(sp)+,sr		*���荞�݊J�n

	movem.l	(sp)+,d1-d2/a0-a1
	unlk	a6
	rts

_MACS_Vsync_R:
*	Input:	d1.w > �R�}���h�R�[�h		Output:  d0.l > Status
*			a1.l > �����A�h���X
*
*	d1.w = 11	�����������荞�݂��������܂�
*				a1.l�ɓo�^�������荞�݃��[�`���̃A�h���X���Z�b�g����
*				�R�[�����܂��B

	link	a6,#0
	movem.l	d1/a0-a1,-(sp)

	move.w	sr,-(sp)		* �X�e�[�^�Xreg�ۑ�
	ori.w	#$0700,sr		* �S���荞�ݎ~��

	move.w	#11,d1
	movea.l	8(a6),a1
	IOCS	_MACS
*	tst.l	d0

	tst.b	$E9A001		* 8255(�ޮ��è��)�̋�ǂ�
	nop			* ���O�܂ł̖��߃p�C�v���C���̓���
				* �����b�A���̖��ߏI���܂łɂ�
				* ����ȑO�̃o�X�T�C�N���Ȃǂ�
				* �������Ă��邱�Ƃ��ۏ؂����B
				
	move.w	(sp)+,sr		* ���荞�݊J�n

	movem.l	(sp)+,d1/a0-a1
	unlk	a6
	rts

_MACS_Sleep:
*	Input:	d1.w > �R�}���h�R�[�h		Output:  d0.l > Status
*			d2.w > �J�E���^
*
*			d1.w = 13	�X���[�v��Ԃ�ݒ肵�܂�
*			d2.w��0�Ȃ�Đ��\��ԁC0�ȊO�Ȃ�X���[�v���܂��B

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
*������ʃt���O (d4.l)
*
*bit 0	�n�[�t�g�[�� (1�̎��p���b�g��50%�Â�����)
*bit 1	PCM�J�b�g (1�̎�PCM�̍Đ������Ȃ�)
*bit 2	�L�[�Z���X�A�{�[�gOFF (1�̎��L�[���͎��I�����Ȃ�)
*bit 3	��ʍď������}���t���O (1�̎��A�j���I������ʂ����������Ȃ�)
*bit 4	�X���[�v��Ԗ��� (1�̎�MACSDRV���X���[�v���Ă��鎞�ł��Đ�����)
*bit 5	�O���t�B�b�N�u�q�`�l�\���t���O (1�̎����̨����\�������܂܍Đ�����)
*bit 6	�X�v���C�g�\���t���O (1�̎����ײĂ�\�������܂܍Đ�����)
*
*bit 7
*  :		Reserved. (�K��0�ɂ��邱��)
*bit31
*
*------------------------------------------------------------------------------
*�G���[�R�[�h�ꗗ (d0.l)
*
*-1	MACS�f�[�^�ł͂���܂���
*-2	MACS�̃o�[�W�������Â����܂�
*-3	�f�[�^���o�b�t�@�ɑS�����͂���Ă��܂���
*-4	�A�{�[�g����܂���
*-5	���Ɍ��݃A�j���[�V�����Đ����ł�
*-6	�����ȃt�@���N�V�������w�肵�܂���
*-7	���[�U�[���X�g�������͊��荞�݃��X�g����t�ł�
*-8	���[�U�[���X�g�������͊��荞�݃��X�g�ɓo�^����Ă��܂���
*==============================================================================
