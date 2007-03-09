/*********************************/
/* Copyright Artyom Tonkikh 2007 */
/* License GPL                   */
/*********************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fribidi/fribidi.h>

#include "defines.h"
#include "io.h"
#include "bidi.h"

void help(void)
{
	fprintf(stderr,
			"usage: biditex [ parameters ] [ inputfilename ]\n"
			"       -o file_name.tex             - output file name\n"
			"       -e utf8 | iso8859-8 | cp1255 - encoding\n"
			"       -m                 replace '--'  &   '---'\n"
			"                          by   '\\L{--} & \\L{'---'}\n"
	);
	exit(1);
}


/* Read cmd line parameters
 * following are supported 
 * -o output file name
 * -e utf8 | iso8859-8 | cp1255
 * inputfilename */
void read_parameters(int argc,char **argv,
					char **fname_in,char **fname_out,
					int *encoding,int *replace_minus)
{
	int i;
	int cnt1=0,cnt2=0,cnt3=0;
	
	for(i=1;i<argc;i++){
		if(strcmp(argv[i],"-h")==0) {
			help();
		}
		else if(strcmp(argv[i],"-m")==0) {
			*replace_minus = 1;
		}
		else if(strcmp(argv[i],"-o")==0) {
			i++;
			if(i>=argc){
				help();
			}
			*fname_out = argv[i];
			cnt1++;
		}
		else if(strcmp(argv[i],"-e")==0) {
			i++;
			if(i>=argc){
				help();
			}
			if(strcmp(argv[i],"utf8")==0) {
				*encoding=ENC_UTF_8;
			}
			else if(strcmp(argv[i],"cp1255")==0) {
				*encoding=ENC_CP1255;
			}
			else if(strcmp(argv[i],"iso8859-8")==0) {
				*encoding=ENC_ISO_8859_8;
			}
			else {
				help();
			}
			cnt2++;
		}
		else {
			*fname_in=argv[i];
			cnt3++;
		}
	}
	if(cnt1>1 || cnt2>1 || cnt3>1){
		help();
	}
}

/* Global buffers */

static FriBidiChar text_line_in[MAX_LINE_SIZE];
static FriBidiChar text_line_out[MAX_LINE_SIZE];

/****************************
 ******** M A I N ***********
 ****************************/
int main(int argc,char **argv)
{
	char *fname_in=NULL,*fname_out=NULL;
	int encoding=ENC_DEFAULT;
	int replace_minus = 0;

	FILE *f_in,*f_out;
	
	/****************** 
	 * Inicialization * 
	 ******************/
		
	read_parameters(argc,argv,&fname_in,&fname_out,&encoding,&replace_minus);

	if(!fname_in) {
		f_in = stdin;
	}
	else {
		if(!(f_in=fopen(fname_in,"r"))) {
			fprintf(stderr,"Failed to open %s for reading\n",fname_in);
			exit(1);
		}
	}

	if(!fname_out) {
		f_out = stdout;
	}
	else {
		if(!(f_out=fopen(fname_out,"w"))) {
			fprintf(stderr,"Failed to open %s for writing\n",fname_out);
			exit(1);
		}
	}
	
	bidi_init();
	
	/*************
	 * Main loop *
	 *************/
	

	while(io_read_line(text_line_in,encoding,f_in)) {
		
		if(bidi_process(text_line_in,text_line_out,replace_minus)) {
			/*If there is something to print */
			io_write_line(text_line_out,encoding,f_out);
		}
		
	}
	
	/**********
	 * Finish *
	 **********/
	
	if(f_out!=stdout) fclose(f_out);
	if(f_in!=stdin) fclose(f_in);
	
	bidi_finish();
	
	return 0;
}
