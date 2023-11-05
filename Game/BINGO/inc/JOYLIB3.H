/* JOYDRV3 C言語ﾍｯﾀﾞ version 2.00 */
/* Copyright 2023 by HUYE */
int joydrv_djoyget(int joyno);					/* 関数名(デジタルジョイスティック読み込み) */
int joydrv_ajoyget(int joyno, short *analog_data);		/* 関数名(アナログジョイスティック読み込み)  analog_data[5]*/
int joydrv_getadj(int joyno);					/* 関数名(ジョイスティックモード取得(JOY番号指定)) */
int joydrv_getadd(int devid, int portno);			/* 関数名(ジョイスティックモード取得(デバイスID指定)) */
int joydrv_ctrlmotor(int joyno, int motor1, int motor2);	/* 関数名(振動コントロール) */
int joydrv_setmode(int joyno, int devid, int portno);		/* 関数名(デバイスID、ポート番号設定) */
int joydrv_getmode(int joyno, int *devid, int *portno);		/* 関数名(デバイスID、ポート番号取得) */
