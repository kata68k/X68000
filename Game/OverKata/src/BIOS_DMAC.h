#ifndef	BIOS_DMAC_H
#define	BIOS_DMAC_H

#include <usr_define.h>

/* externêÈåæ */
extern int16_t DMAC_INIT(void);
extern int16_t DMAC_COPY(void *, void *, size_t);
extern int16_t DMAC_COPY_A(void *, struct _chain *, size_t);
extern int16_t DMAC_Image_To_VRAM(uint8_t, int16_t, int16_t);
extern int16_t DMAC_Image_To_VRAM_X(uint8_t, int16_t, int16_t);

#endif	/* BIOS_DMAC_H */
