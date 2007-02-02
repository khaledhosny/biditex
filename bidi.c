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

/***********************/
/* Global Data */

#ifdef SMART_FRIBIDI
typedef struct COMMAND {
	char *name;
	struct COMMAND *next;
} 
	bidi_cmd_t;

bidi_cmd_t *bidi_command_list;

#endif

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

#define TAG_BIDI_ON			"%BIDION"
#define TAG_BIDI_OFF		"%BIDIOFF"
#define TAG_BIDI_NEW_TAG	"%BIDITAG"

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


/* Returns the minimal embedding level for required direction */
int bidi_basic_level(int is_heb)
{
	if(is_heb)
		return 1;
	return 0;
}

#ifdef SMART_FRIBIDI

void bidi_add_command(char *name)
{
	bidi_cmd_t *new_cmd;
	char *new_text;
	if(!(new_cmd=(bidi_cmd_t*)malloc(sizeof(bidi_cmd_t)))
		|| !(new_text=(char*)malloc(strlen(name)+1))) {
		fprintf(stderr,"Out of memory\n");
		exit(1);
	}
	new_cmd->next=bidi_command_list;
	new_cmd->name=new_text;
	strcpy(new_text,name);
	bidi_command_list = new_cmd;
}

int bidi_is_cmd_char(FriBidiChar ch)
{
	if( ('a'<= ch && ch <='z') || ('A' <=ch && ch <= 'Z') )
		return 1;
	return 0;
}

void bidi_add_command_u(FriBidiChar *text)
{
	char buffer[256];
	int len=0;
	if(*text!=' ' && *text!='\t') {
		fprintf(stderr,"Tag definition should have space\n");
		exit(1);
	}
	while(*text==' ' || *text=='\t') {
		text++;
	}
	if(!bidi_is_cmd_char(*text)) {
		fprintf(stderr,"Syntaxis error \n");
	}
	while(bidi_is_cmd_char(*text) && len < 255) {
		buffer[len]=*text;
		text++;
		len++;
	}
	if(len>=255) {
		fprintf(stderr,"Tag is too long\n");
		exit(1);
	}
	buffer[len]=0;
	bidi_add_command(buffer);
}

int bidi_in_cmd_list(FriBidiChar *text,int len)
{
	int i;
	bidi_cmd_t *p;
	for(p=bidi_command_list;p;p=p->next) {
		for(i=0;i<len;i++) {
			if(text[i]!=p->name[i]) 
				break;
		}
		if(i==len && p->name[len]==0){
			return 1;
		}
	}
	return 0;
}

int bidi_is_command(FriBidiChar *text,int *command_length)
{
	int len;
	if(*text!='\\'){
		return 0;
	}
	if(!bidi_is_cmd_char(text[1])) {
		return 0;
	}
	text++;
	len=0;
	while(bidi_is_cmd_char(text[len])){
		len++;
	}
	
	if(!bidi_in_cmd_list(text,len) || text[len]!='{') {
		*command_length=len+1;
		return 1;
	}
	len++;
	while(text[len] && text[len]!='}') {
		len++;
	}
	if(text[len]=='}') len++;
	*command_length=len+1;
	return 1;
}

void bidi_tag_tolerant_fribidi_l2v(	FriBidiChar *in,int len,
									FriBidiCharType *direction,
									FriBidiLevel *embed)
{
	int in_pos,out_pos,length,i;
	FriBidiChar *in_tmp;
	FriBidiLevel *embed_tmp;
	FriBidiLevel fill_level;
	
	fill_level=bidi_basic_level( *direction == FRIBIDI_TYPE_RTL);
	
	in_tmp=(FriBidiChar*)malloc(sizeof(FriBidiChar)*(len+1));
	embed_tmp=(FriBidiLevel*)malloc(sizeof(FriBidiLevel)*len);
	
	if(!in_tmp || !embed_tmp) {
		fprintf(stderr,"Out of memory\n");
		exit(1);
	}
	
	/* Copy all the data without tags */
	in_pos=0;
	out_pos=0;

	while(in_pos<len) {
		if(bidi_is_command(in+in_pos,&length)){
			in_pos+=length;
			continue;
		}
		/* Copy to buffer */
		in_tmp[out_pos]=in[in_pos];
		out_pos++;
		in_pos++;
	}
	
	/* Note - you must take the new size for firibidi */
	fribidi_log2vis_get_embedding_levels(in_tmp,out_pos,direction,embed_tmp);

	/* Return the tags (or neutral embedding) */
	in_pos=0;
	out_pos=0;

	while(in_pos<len) {
		if(bidi_is_command(in+in_pos,&length)){
			for(i=0;i<length;i++){
				embed[in_pos]=fill_level;
				in_pos++;
			}
			continue;
		}
		/* Retrieve embedding */
		embed[in_pos]=embed_tmp[out_pos];
		out_pos++;
		in_pos++;
	}

	/* Not forget to free */
	free(embed_tmp);
	free(in_tmp);
}

#endif

/* The function that parses line and adds required \R \L tags */
void bidi_add_tags(FriBidiChar *in,FriBidiChar *out,int limit,int is_heb)
{
	int len,new_len,level,new_level,brakets;
	int i,size;
	
	const char *tag;
	
	FriBidiCharType direction;

	direction = is_heb ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
	
	len=bidi_strlen(in);
	
#ifdef SMART_FRIBIDI	
	bidi_tag_tolerant_fribidi_l2v(in,len,&direction,bidi_embed);
#else
	fribidi_log2vis_get_embedding_levels(in,len,&direction,bidi_embed);
#endif

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
#ifdef SMART_FRIBIDI	
	if(bidi_strieq_u_a(in,TAG_BIDI_NEW_TAG)) {
		bidi_add_command_u(in+strlen(TAG_BIDI_NEW_TAG));
		return 0;
	}
#endif
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
#ifdef SMART_FRIBIDI
	bidi_cmd_t *tmp;
	while(bidi_command_list) {
		tmp=bidi_command_list;
		bidi_command_list=bidi_command_list->next;
		free(tmp->name);
		free(tmp);
	}
#endif
	if(bidi_mode == MODE_BIDION) {
		fprintf(stderr,"Warning: No %%BIDIOFF Tag at the end of the file\n");
	}
}

void bidi_init(void)
{
	bidi_mode = MODE_BIDIOFF;
#ifdef SMART_FRIBIDI
	bidi_add_command("begin");
	bidi_add_command("end");	
#endif
}
