#ifndef _SYS_INIT_H_
#define _SYS_INIT_H_

#include <png/png.h>

void*
DecodePNGtoRGBA(void *src, int len, int *img_w, int *img_h, int *img_p, int *res_len);

#endif