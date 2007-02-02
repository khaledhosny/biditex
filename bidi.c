#include "defines.h"
#include "bidi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DEFIS_UGLY_HACK

/***********************/
/* Global Data */

enum { MODE_BIDIOFF, MODE_BIDION };

static int bidi_mode;
static FriBidiLevel bidi_embed[MAX_LINE_SIZE];

/* only ASCII charrecters mirroring are
 * supported - the coded below 128 according to
 * http://www.unicode.org/Public/4.1.0/ucd/BidiMirroring.txt */

/* TODO: implement full mirroring list according to unicode 
 * standard */
static const char *bidi_mirror_list[][2] = 
{
	{"(",  ")"},
	{")",  "("},
	{"<",  ">"},
	{">",  "<"},
	{"[",  "]"},
	{"]",  "["},
	{"\\{","\\}"},
	{"\\}","\\{"},
#ifdef DEFIS_UGLY_HACK	
	{"---","\\L{---}"},
	{"--","\\L{--}"},
#endif
	{NULL,NULL}
};

/********/
/* TAGS */
/********/

#define TAG_BIDI_ON		"%BIDION"
#define TAG_BIDI_OFF	"%BIDIOFF"

#define TAG_RTL			"\\R{"
#define TAG_LTR			"\\L{"
#define TAG_CLOSE		"}"

/***********************/

/* Compares begining of string U and C 
 * for example "Hello!" == "Hel" */
static int bidi_strieq_u_a(const FriBidiChar *u,const char *c)
{
	while(*u && *c) {
		if(*u!=*c)
			return 0;
		u++; c++;
	}
	if(*u ==0 && *c!=0) {
		return 0;
	}
	return 1;
}

static int bidi_strlen(FriBidiChar *in)
{
	int len;
	for(len=0;*in;len++) in++;
	return len;
}

/* Safe functions for adding charrecters to buffer 
 * if the line is too long the program just exits */

static void bidi_add_char_u(FriBidiChar *out,int *new_len,
							int limit,FriBidiChar ch)
{
	if(*new_len+2<limit){
		out[*new_len]=ch;
		++*new_len;
		out[*new_len]=0;
	}
	else {
		fprintf(stderr,"The line is too long\n");
		exit(1);
	}
}
static void bidi_add_str_c(FriBidiChar *out,int *new_len,
							int limit,const char *str)
{
	while(*str){
		bidi_add_char_u(out,new_len,limit,*str);
		str++;
	}
}

/* Function looks if the first charrecter or sequence 
 * in "in" is mirrored charrecter and returns its mirrot
 * in this case. If it is not mirrored then returns NULL */

static const char *bidi_mirror(FriBidiChar *in,int *size)
{
	int pos=0;
	while(bidi_mirror_list[pos][0]) {
		if(bidi_strieq_u_a(in,bidi_mirror_list[pos][0])) {
			*size=strlen(bidi_mirror_list[pos][0]);
			return bidi_mirror_list[pos][1];
		}
		pos++;
	}
	return NULL;
}


void bidi_init(void)
{
	bidi_mode = MODE_BIDIOFF;
}

/* Returns the minimal embedding level for required direction */
int bidi_basic_level(int is_heb)
{
	if(is_heb)
		return 1;
	return 0;
}

/* The function that parses line and adds required \R \L tags */
void bidi_add_tags(FriBidiChar *in,FriBidiChar *out,int limit,int is_heb)
{
	int len,new_len,level,new_level,brakets;
	int i,size;
	
	const char *tag;
	
	FriBidiCharType direction;

	direction = is_heb ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
	
	len=bidi_strlen(in);
	
	fribidi_log2vis_get_embedding_levels(in,len,&direction,bidi_embed);

	level=bidi_basic_level(is_heb);
	
	new_len=0;
	out[0]=0;
	brakets=0;
	for(i=0,new_len=0;i<len;i++){
		new_level=bidi_embed[i];

		if(new_level>level) {
			/* LTR Direction according to odd/even value of level */
			if((new_level & 1) == 0) {
				tag=TAG_LTR;
			}
			else {
				tag=TAG_RTL;
			}
			brakets++;
			bidi_add_str_c(out,&new_len,limit,tag);
		}
		else if(new_level<level) {
			bidi_add_str_c(out,&new_len,limit,TAG_CLOSE);
			brakets--;
		}
		if((new_level & 1)!=0 && (tag=bidi_mirror(in+i,&size))!=NULL){
			/* Replace charrecter with its mirror only in case
			 * we are in RTL direction */
			
			/* Note this can be a sequence like "\{" */
			bidi_add_str_c(out,&new_len,limit,tag);
			i+=size-1;
		}
		else {
			bidi_add_char_u(out,&new_len,limit,in[i]);
		}
		level=new_level;
	}
	/* Fill all missed brakets */
	while(brakets){
		bidi_add_str_c(out,&new_len,limit,TAG_CLOSE);
		brakets--;
	}
}

/* Main line processing function */
int bidi_process(FriBidiChar *in,FriBidiChar *out)
{
	int i;
	if(bidi_strieq_u_a(in,TAG_BIDI_ON)) {
		bidi_mode = MODE_BIDION;
		return 0;
	}
	
	if(bidi_strieq_u_a(in,TAG_BIDI_OFF)) {
		bidi_mode = MODE_BIDIOFF;
		return 0;
	}
	
	if(bidi_mode == MODE_BIDION) {
		bidi_add_tags(in,out,MAX_LINE_SIZE,TRUE);
	}
	else {
		for(i=0;in[i];i++){
			out[i]=in[i];
		}
		out[i]=0;
	}
	return 1;
}

void bidi_finish(void)
{
	if(bidi_mode == MODE_BIDION) {
		fprintf(stderr,"Warning: No %%BIDIOFF Tag at the end of the file\n");
	}
}
