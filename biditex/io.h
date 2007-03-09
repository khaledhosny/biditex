#ifndef _IO_H_
#define _IO_H_

#include <stdio.h>
#include <fribidi/fribidi.h>
#include "defines.h"

int io_read_line(FriBidiChar *text,int encoding,FILE *f);
void io_write_line(FriBidiChar *text,int encoding,FILE *f);

#endif
