#ifndef __FFJPEG_JFIF_H__
#define __FFJPEG_JFIF_H__

// ����ͷ�ļ�
#include "bmp.h"
#include<stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* �������� */
void* jfif_load(char *file);
int   jfif_save(void *ctxt, char *file);
void  jfif_free(void *ctxt);

int   jfif_decode(void *ctxt, BMP *pb);
int   jfif_decode_bgr(void *ctxt, uint8_t **data, int *w, int *h);
int   jfif_decode_rgb(void *ctxt, uint8_t **data, int *w, int *h);
void* jfif_encode(BMP *pb);

#ifdef __cplusplus
}
#endif

#endif

