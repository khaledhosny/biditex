#ifndef _BIDI_H_
#define _BIDI_H_
#include <fribidi/fribidi.h>

void bidi_init(void);
int bidi_process(FriBidiChar *in,FriBidiChar *out,int replace_minus);
void bidi_finish(void);

#endif
