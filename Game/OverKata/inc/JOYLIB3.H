/* JOYDRV3 C����ͯ�� version 2.00 */
/* Copyright 2023 by HUYE */
int joydrv_djoyget(int joyno);					/* �֐���(�f�W�^���W���C�X�e�B�b�N�ǂݍ���) */
int joydrv_ajoyget(int joyno, short *analog_data);		/* �֐���(�A�i���O�W���C�X�e�B�b�N�ǂݍ���)  analog_data[5]*/
int joydrv_getadj(int joyno);					/* �֐���(�W���C�X�e�B�b�N���[�h�擾(JOY�ԍ��w��)) */
int joydrv_getadd(int devid, int portno);			/* �֐���(�W���C�X�e�B�b�N���[�h�擾(�f�o�C�XID�w��)) */
int joydrv_ctrlmotor(int joyno, int motor1, int motor2);	/* �֐���(�U���R���g���[��) */
int joydrv_setmode(int joyno, int devid, int portno);		/* �֐���(�f�o�C�XID�A�|�[�g�ԍ��ݒ�) */
int joydrv_getmode(int joyno, int *devid, int *portno);		/* �֐���(�f�o�C�XID�A�|�[�g�ԍ��擾) */
