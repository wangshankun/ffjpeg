// ����ͷ�ļ�
#include <stdlib.h>
#include "stdefine.h"
#include "bitstr.h"

//+++ memory bitstr +++//

/* �ڲ����Ͷ��� */
typedef struct {
    int   type;
    int   bitbuf;
    int   bitnum;
    BYTE *membuf;
    int   memlen;
    int   curpos;
} MBITSTR;

/* ����ʵ�� */
static void* mbitstr_open(void *buf, int len)
{
    MBITSTR *context = calloc(1, sizeof(MBITSTR));
    if (!context) return NULL;
    context->type   = MEM_BITSTR;
    context->membuf = buf;
    context->memlen = len;
    return context;
}

static int mbitstr_close(void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context) return EOF;
    free(context);
    return 0;
}

static int mbitstr_getc(void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context || context->curpos >= context->memlen) return EOF;
    return context->membuf[context->curpos++];
}

static int mbitstr_putc(int c, void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context || context->curpos >= context->memlen) return EOF;
    return (context->membuf[context->curpos++] = c);
}

static int mbitstr_seek(void *stream, long offset, int origin)
{
    MBITSTR *context = (MBITSTR*)stream;
    int      newpos  = 0;
    if (!context) return EOF;

    switch (origin) {
    case SEEK_SET: newpos = offset; break;
    case SEEK_CUR: newpos = context->curpos + offset; break;
    case SEEK_END: newpos = context->memlen + offset; break;
    }
    if (newpos < 0 || newpos > context->memlen) return EOF;

    context->curpos = newpos;
    context->bitbuf = 0;
    context->bitnum = 0;
    return 0;
}

static long mbitstr_tell(void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context) return EOF;
    return context->curpos > context->memlen ? EOF : context->curpos;
}

static int mbitstr_getb(void *stream)
{
    int bit;
    MBITSTR *context = (MBITSTR*)stream;
    if (!context) return EOF;

    if (context->bitnum == 0) {
        context->bitbuf = mbitstr_getc(context);
        context->bitnum = 8;
        if (context->bitbuf == EOF) {
            return EOF;
        }
    }
    
    bit = context->bitbuf & (1 << 0);
    context->bitbuf >>= 1;
    context->bitnum--;
    return bit;
}

static int mbitstr_putb(int b, void *stream)
{
    MBITSTR *context = (MBITSTR*)stream;
    if (!context) return EOF;

    if (context->bitnum == 8) {
        if (EOF == mbitstr_putc(context->bitbuf & 0xff, context)) {
            return EOF;
        }
        context->bitbuf = 0;
        context->bitnum = 0;
    }

    b &= (1 << 0);
    context->bitbuf |= (b << context->bitnum);
    context->bitnum++;
    return b;
}

static int mbitstr_flush(void *stream) { return stream ? 0 : EOF; }

//--- memory bitstr ---//



//+++ file bitstr +++//

/* �ڲ����Ͷ��� */
typedef struct {
    int   type;
    int   bitbuf;
    int   bitnum;
    FILE *fp;
} FBITSTR;

/* ����ʵ�� */
static void* fbitstr_open(char *file, char *mode)
{
    FBITSTR *context = calloc(1, sizeof(FBITSTR));
    if (!context) return NULL;

    context->type = FILE_BITSTR;
    context->fp   = fopen(file, mode);
    if (!context->fp) {
        free(context);
        return NULL;
    }
    else return context;
}

static int fbitstr_close(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    fclose(context->fp);
    free  (context);
    return 0;
}

static int fbitstr_getc(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    return fgetc(context->fp);
}

static int fbitstr_putc(int c, void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    return fputc(c, context->fp);
}

static int fbitstr_seek(void *stream, long offset, int origin)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    context->bitbuf = 0;
    context->bitnum = 0;
    return fseek(context->fp, offset, origin);
}

static long fbitstr_tell(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;
    return ftell(context->fp);
}

static int fbitstr_getb(void *stream)
{
    int bit;
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum == 0) {
        context->bitbuf = fgetc(context->fp);
        context->bitnum = 8;
        if (context->bitbuf == EOF) {
            return EOF;
        }
    }
    
    bit = context->bitbuf & (1 << 0);
    context->bitbuf >>= 1;
    context->bitnum--;
    return bit;
}

static int fbitstr_putb(int b, void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum == 8) {
        if (EOF == fputc(context->bitbuf & 0xff, context->fp)) {
            return EOF;
        }
        context->bitbuf = 0;
        context->bitnum = 0;
    }

    b &= (1 << 0);
    context->bitbuf |= (b << context->bitnum);
    context->bitnum++;
    return b;
}

static int fbitstr_flush(void *stream)
{
    FBITSTR *context = (FBITSTR*)stream;
    if (!context || !context->fp) return EOF;

    if (context->bitnum != 0) {
        if (EOF == fputc(context->bitbuf & 0xff, context->fp)) {
            return EOF;
        }
        context->bitbuf = 0;
        context->bitnum = 0;
    }
    return fflush(context->fp);
}

//--- file bitstr --//



// ����ʵ��
void* bitstr_open(int type, char *file, char *mode)
{
    switch (type) {
    case MEM_BITSTR : return mbitstr_open((void*)file, (int  )mode);
    case FILE_BITSTR: return fbitstr_open((char*)file, (char*)mode);
    }
    return NULL;
}

int bitstr_close(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_close(stream);
    case FILE_BITSTR: return fbitstr_close(stream);
    }
    return EOF;
}

int bitstr_getc(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_getc(stream);
    case FILE_BITSTR: return fbitstr_getc(stream);
    }
    return EOF;
}

int bitstr_putc(int c, void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_putc(c, stream);
    case FILE_BITSTR: return fbitstr_putc(c, stream);
    }
    return EOF;
}

int bitstr_seek(void *stream, long offset, int origin)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_seek(stream, offset, origin);
    case FILE_BITSTR: return fbitstr_seek(stream, offset, origin);
    }
    return EOF;
}

long bitstr_tell(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_tell(stream);
    case FILE_BITSTR: return fbitstr_tell(stream);
    }
    return EOF;
}

int bitstr_getb(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_getb(stream);
    case FILE_BITSTR: return fbitstr_getb(stream);
    }
    return EOF;
}

int bitstr_putb(int b, void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_putb(b, stream);
    case FILE_BITSTR: return fbitstr_putb(b, stream);
    }
    return EOF;
}

int bitstr_flush(void *stream)
{
    int type = *(int*)stream;
    switch (type) {
    case MEM_BITSTR : return mbitstr_flush(stream);
    case FILE_BITSTR: return fbitstr_flush(stream);
    }
    return EOF;
}








