#include "defines.h"
#include "bidi.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif 

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
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

enum { MODE_BIDIOFF, MODE_BIDION, MODE_BIDILTR };


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
	{NULL,NULL}
};

static const char *bidi_hack_list[][2] = 
{
	{"---","\\L{---}"},
	{"--","\\L{--}"},
	{NULL,NULL}
};

/********/
/* TAGS */
/********/

#define TAG_BIDI_ON			"%BIDION"
#define TAG_BIDI_OFF		"%BIDIOFF"
#define TAG_BIDI_NEW_TAG	"%BIDITAG"
#define TAG_BIDI_LTR		"%BIDILTR"

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
static const char *bidi_one_mirror(FriBidiChar *in,int *size,
								const char *list[][2])
{
	int pos=0;
	while(list[pos][0]) {
		if(bidi_strieq_u_a(in,list[pos][0])) {
			*size=strlen(list[pos][0]);
			return list[pos][1];
		}
		pos++;
	}
	return NULL;
}

static const char *bidi_mirror(FriBidiChar *in,int *size,
								int replace_minus)
{
	const char *ptr;
	if((ptr=bidi_one_mirror(in,size,bidi_mirror_list))!=NULL) {
		return ptr;
	}
	if(replace_minus) {
		return bidi_one_mirror(in,size,bidi_hack_list);
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
	
	new_cmd=(bidi_cmd_t*)utl_malloc(sizeof(bidi_cmd_t));
	new_text=(char*)utl_malloc(strlen(name)+1);
	
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
	char buffer[MAX_COMMAND_LEN];
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
	while(bidi_is_cmd_char(*text) && len < MAX_COMMAND_LEN-1) {
		buffer[len]=*text;
		text++;
		len++;
	}
	if(len>=MAX_COMMAND_LEN-1) {
		fprintf(stderr,"Tag is too long\n");
		exit(1);
	}
	buffer[len]=0;
	bidi_add_command(buffer);
}

/* Verirfies wether then text of length "len" is
 * in command list */
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
/*Verifies wether the next string is command 
 * ie: "\\[a-zA-Z]+" or "\\[a-zA-Z]+\*" */
int bidi_is_command(FriBidiChar *text,int *command_length)
{
	int len;
	if(*text != '\\' || !bidi_is_cmd_char(text[1])) {
		return FALSE;
	}
	len=1;
	while(bidi_is_cmd_char(text[len])) {
		len++;
	}
	if(text[len] == '*') {
		len++;
	}
	*command_length = len;
	return TRUE;
}

/* This is implementation of state machine with stack 
 * that distinguishs between text and commands */

/* STACK VALUES */
enum { 
	EMPTY,
	SQ_BRACKET ,SQ_BRACKET_IGN,
	BRACKET, BRACKET_IGN,
	CMD_BRACKET, CMD_BRACKET_IGN
};
/* STATES */
enum { ST_NO, ST_NORM, ST_IGN };

/* Used for ignore commands */
int bidi_is_ignore(int top)
{
	return	top == SQ_BRACKET_IGN
			|| top== BRACKET_IGN
			|| top == CMD_BRACKET_IGN;
}

int bidi_state_on_left_br(int top,int *after_command_state)
{
	int ign_addon;
	int push,state = *after_command_state;
	if(bidi_is_ignore(top) || state == ST_IGN) {
		ign_addon = 1;
	}
	else {
		ign_addon = 0;
	}
	
	if(state) {
		push = CMD_BRACKET;
	}
	else{
		push = BRACKET;
	}
	
	*after_command_state = ST_NO;
	return push + ign_addon;
}

int bidi_state_on_left_sq_br(int top,int *after_command_state)
{
	int push;
	if(bidi_is_ignore(top) || *after_command_state == ST_IGN) {
		push = SQ_BRACKET_IGN;
	}
	else {
		push = SQ_BRACKET;
	}
	*after_command_state = ST_NO;
	return push;
}

void bidi_state_on_right_br(int top,int *after_command_state)
{
	if(top == CMD_BRACKET) {
		*after_command_state = ST_NORM;
	}
	else if(top == BRACKET || top == BRACKET_IGN) {
		*after_command_state = ST_NO;
	}
	else {/*top == CMD_BRACKET_IGN*/
		*after_command_state = ST_IGN;
	}
}

void bidi_state_on_right_sq_br(int top,int *after_command_state)
{
	if(top == SQ_BRACKET_IGN) {
		*after_command_state = ST_IGN;
	}
	else { /* top == SQ_BRACKET */
		*after_command_state = ST_NORM;
	}
}

/* Support of equations */
int bidi_calc_equation(FriBidiChar *in)
{
	int len=1;
	while(in[len] && in[len]!='$') {
		if(in[len]=='\\' && (in[len+1]=='$' || in[len+1]=='\\')) {
			len+=2;
		}
		else {
			len++;
		}
	}
	if(in[len]=='$')
		len++;
	return len;		
}

/* This function parses the text "in" in marks places that
 * should be ignored by fribidi in "is_command" as true */
void bidi_mark_commands(FriBidiChar *in,int len,char *is_command,int is_heb)
{

	char *parthness_stack;
	int stack_size=0;
	int cmd_len,top;
	int after_command_state=ST_NO;
	int mark,pos,symbol,i,push;

	/* Assumption - depth of stack can not be bigger then text length */
	parthness_stack = utl_malloc(len);
	
	pos=0;
	
	while(pos<len) {
		
		top = stack_size == 0 ? EMPTY : parthness_stack[stack_size-1];
		symbol=in[pos];
#ifdef DEBUG_STATE_MACHINE		
		printf("pos=%d sybol=%c state=%d top=%d\n",
			pos,(symbol < 127 ? symbol : '#'),after_command_state,top);
#endif 
		if(bidi_is_command(in+pos,&cmd_len)) {
			for(i=0;i<cmd_len;i++) {
				is_command[i+pos]=TRUE;
			}
			if(bidi_in_cmd_list(in+pos+1,cmd_len-1)) {
				after_command_state = ST_IGN;
			}
			else {
				after_command_state = ST_NORM;
			}
			pos+=cmd_len;
			continue;
		}
		else if((symbol=='\\' && in[pos+1]=='\\') || symbol=='$' ) {
			if(symbol == '$') {
				cmd_len=bidi_calc_equation(in+pos);
			}
			else {
				cmd_len = 2;
			}
			
			for(i=0;i<cmd_len;i++) {
				is_command[i+pos]=TRUE;
			}
			pos+=cmd_len;
			continue;
		}
		else if( symbol == '{' ) {
			push = bidi_state_on_left_br(top,&after_command_state);
			parthness_stack[stack_size++] = push;
			mark=TRUE;
		}
		else if(symbol == '[' && after_command_state) {
			push = bidi_state_on_left_sq_br(top,&after_command_state);
			parthness_stack[stack_size++] = push;
			mark=TRUE;
		}
		else if(symbol == ']' && (top == SQ_BRACKET || top == SQ_BRACKET_IGN)){
			bidi_state_on_right_sq_br(top,&after_command_state);
			stack_size--;
			mark=TRUE;
		}
		else if(symbol == '}' && (BRACKET <= top && top <= CMD_BRACKET_IGN)) {
			bidi_state_on_right_br(top,&after_command_state);
			stack_size--;
			mark=TRUE;
		}
		else {
			mark = bidi_is_ignore(top);
			after_command_state = ST_NO;
		}
		is_command[pos++]=mark;
	}
	
	utl_free(parthness_stack);
}

/* This function marks embedding levels at for text "in",
 * it ignores different tags */
void bidi_tag_tolerant_fribidi_l2v(	FriBidiChar *in,int len,
									int is_heb,
									FriBidiLevel *embed,
									char *is_command)
{
	int in_pos,out_pos,cmd_len,i;
	FriBidiChar *in_tmp;
	FriBidiLevel *embed_tmp,fill_level;
	FriBidiCharType direction;

	if(is_heb)
		direction = FRIBIDI_TYPE_RTL;
	else
		direction = FRIBIDI_TYPE_LTR;
	
	in_tmp=(FriBidiChar*)utl_malloc(sizeof(FriBidiChar)*(len+1));
	embed_tmp=(FriBidiLevel*)utl_malloc(sizeof(FriBidiLevel)*len);
	
	/**********************************************
	 * This is main parser that marks commands    *
	 * across the text i.e. marks non text        *
	 **********************************************/
	bidi_mark_commands(in,len,is_command,is_heb);
	
	/**********************************************/
	/* Copy all the data without tags for fribidi */
	/**********************************************/
	in_pos=0;
	out_pos=0;

	while(in_pos<len) {
		if(is_command[in_pos]){
			in_pos++;
			continue;
		}
		/* Copy to buffer */
		in_tmp[out_pos]=in[in_pos];
		out_pos++;
		in_pos++;
	}
	
	/***************
	 * RUN FRIBIDI *
	 ***************/
	
	/* Note - you must take the new size for firibidi */
	fribidi_log2vis_get_embedding_levels(in_tmp,out_pos,&direction,embed_tmp);

	/****************************************************
	 * Return the tags and fill missing embedding level *
	 ****************************************************/
	in_pos=0;
	out_pos=0;

	while(in_pos<len) {
		if(is_command[in_pos]){
			/* Find the length of the part that 
			 * has a command/tag */
			for(cmd_len=0;in_pos+cmd_len<len;cmd_len++) {
				if(!is_command[cmd_len+in_pos])
					break;
			}
			
			if(in_pos == 0 || in_pos + cmd_len == len){
				/* When we on start/end assume basic direction */
				fill_level = bidi_basic_level(is_heb);
			}
			else {
				/* Fill with minimum on both sides */
				fill_level = min(embed_tmp[out_pos-1],embed_tmp[out_pos]);
			}
			
			for(i=0;i<cmd_len;i++){
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
	utl_free(embed_tmp);
	utl_free(in_tmp);
}

#endif

/* The function that parses line and adds required \R \L tags */
void bidi_add_tags(FriBidiChar *in,FriBidiChar *out,int limit,
					int is_heb,int replace_minus)
{
	int len,new_len,level,new_level,brakets;
	int i,size;
	
	const char *tag;
	char *is_command;
	
	
	len=bidi_strlen(in);
	
	is_command=(char*)utl_malloc(len);
	
#ifdef SMART_FRIBIDI	
	bidi_tag_tolerant_fribidi_l2v(in,len,is_heb,bidi_embed,is_command);
#else
	{
		FriBidiCharType direction;
		direction = is_heb ? FRIBIDI_TYPE_RTL : FRIBIDI_TYPE_LTR;
		fribidi_log2vis_get_embedding_levels(in,len,&direction,bidi_embed);
	}
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
		if((new_level & 1)!=0 && (tag=bidi_mirror(in+i,&size,replace_minus))!=NULL
			&& !is_command[i])
		{
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
	utl_free(is_command);
}

/* Main line processing function */
int bidi_process(FriBidiChar *in,FriBidiChar *out,int replace_minus)
{
	int i,is_heb;
	if(bidi_strieq_u_a(in,TAG_BIDI_ON)) {
		bidi_mode = MODE_BIDION;
		return 0;
	}
	
	if(bidi_strieq_u_a(in,TAG_BIDI_OFF)) {
		bidi_mode = MODE_BIDIOFF;
		return 0;
	}
	if(bidi_strieq_u_a(in,TAG_BIDI_LTR)) {
		bidi_mode = MODE_BIDILTR;
		return 0;
	}

#ifdef SMART_FRIBIDI	
	if(bidi_strieq_u_a(in,TAG_BIDI_NEW_TAG)) {
		bidi_add_command_u(in+strlen(TAG_BIDI_NEW_TAG));
		return 0;
	}
#endif
	if(bidi_mode != MODE_BIDIOFF) {
		is_heb = (bidi_mode == MODE_BIDION);
		bidi_add_tags(in,out,MAX_LINE_SIZE, is_heb ,replace_minus);
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
		utl_free(tmp->name);
		utl_free(tmp);
	}
#endif
	if(bidi_mode != MODE_BIDIOFF) {
		fprintf(stderr,"Warning: No %%BIDIOFF Tag at the end of the file\n");
	}
}

void bidi_init(void)
{
	bidi_mode = MODE_BIDIOFF;
#ifdef SMART_FRIBIDI
	bidi_add_command("begin");
	bidi_add_command("end");	
	bidi_add_command("R");
	bidi_add_command("L");
#endif
}
