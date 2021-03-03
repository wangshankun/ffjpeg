// ����ͷ�ļ�
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jfif.h"
#include "bmp.h"

#define savefile(name, buffer, size) do\
{\
  FILE *out = fopen(name, "wb");\
  if(out != NULL)\
  {\
        fwrite (buffer , sizeof(char), size, out);\
        fclose (out);\
  }\
} while(0)

int main(int argc, char *argv[])
{
    void *jfif = NULL;
    BMP   bmp  = {0};

    if (argc < 3) {
        printf(
            "jfif test program\n"
            "usage: ffjpeg -d filename decode jpg file to decode.bmp\n"
            "       ffjpeg -e filename encode bmp file to encode.jpg\n"
        );
        return 0;
    }

    if (strcmp(argv[1], "-d") == 0) {
        jfif = jfif_load(argv[2]);
        jfif_decode(jfif, &bmp);
        jfif_free  (jfif);
        bmp_save(&bmp, "decode.bmp");
        bmp_free(&bmp);
    } else if (strcmp(argv[1], "-e") == 0) {
        bmp_load(&bmp, argv[2]);
        jfif = jfif_encode(&bmp);
        bmp_free(&bmp);
        jfif_save(jfif, "encode.jpg");
        jfif_free(jfif);
    }
    else if (strcmp(argv[1], "-db") == 0) {
        uint8_t* data;
        int w = 0;
        int h = 0;
        jfif = jfif_load(argv[2]);
        jfif_decode_rgb(jfif, &data, &w, &h);
        jfif_free  (jfif);
        savefile("decode.bin", data, w*h*3);
        free(data);
    }

    return 0;
}

