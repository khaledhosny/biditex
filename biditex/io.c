#include <fribidi/fribidi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

static char text_buffer[MAX_LINE_SIZE];

int io_read_line(FriBidiChar *text,int encoding,FILE *f)
{
	int len_char,len_uni;
	if(fgets(text_buffer,MAX_LINE_SIZE-1,f)==NULL) {
		return 0;
	}
	len_char=strlen(text_buffer);
	if(len_char>0 && text_buffer[len_char-1]=='\n') {
		text_buffer[len_char-1]=0;
		len_char--;
	}
	switch(encoding) {
		case ENC_ISO_8859_8:
			len_uni=fribidi_iso8859_8_to_unicode(text_buffer,len_char,text);
			text[len_uni]=0;
			break;
		case ENC_CP1255:
			len_uni=fribidi_cp1255_to_unicode(text_buffer,len_char,text);
			text[len_uni]=0;
			break;
		case ENC_UTF_8:
			len_uni=fribidi_utf8_to_unicode(text_buffer,len_char,text);
			text[len_uni]=0;
			break;
		default:
			fprintf(stderr,"Internal error - wrong encoding\n");
			exit(1);
	}
	return 1;
}

void io_write_line(FriBidiChar *text,int encoding,FILE *f)
{
	int len,char_len,tmp_len;
	/* Find legth of buffer */
	for(len=0;text[len] && len<MAX_LINE_SIZE-1;len++) /* NOTHING */;
		
	if(len>=MAX_LINE_SIZE-1) {
		fprintf(stderr,"Internall error - unterminated line\n");
		exit(1);
	}
	
	if(encoding == ENC_ISO_8859_8) {
		char_len=fribidi_unicode_to_iso8859_8(text,len,text_buffer);
		text_buffer[char_len]=0;
		fprintf(f,"%s\n",text_buffer);
	}
	else if(encoding == ENC_CP1255) {
		char_len=fribidi_unicode_to_cp1255(text,len,text_buffer);
		text_buffer[char_len]=0;
		fprintf(f,"%s\n",text_buffer);
	}
	else if(encoding == ENC_UTF_8) {
		while(len) {
			/* Because maybe |unicode| < |utf8| 
			 * in order to prevetn overflow */
			if(len>MAX_LINE_SIZE/8){
				tmp_len=MAX_LINE_SIZE/8;
			}
			else {
				tmp_len = len;
			}
			char_len=fribidi_unicode_to_utf8(text,tmp_len,text_buffer);
			text_buffer[char_len]=0;
			fprintf(f,"%s",text_buffer);
			len  -= tmp_len;
			text += tmp_len;
		}
		fprintf(f,"\n");
	}
	else {
		fprintf(stderr,"Internal error - wrong encoding\n");
		exit(1);
	}
}
