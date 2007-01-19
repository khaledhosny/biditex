#ifndef _DEFINES_H_
#define _DEFINES_H_

enum { ENC_UTF_8 , ENC_ISO_8859_8 };

/* Defailt encoding is UTF-8*/

#ifdef USE_8BIT_DEF_ENC
#	define ENC_DEFAULT		ENC_ISO_8859_8
#else
#	define ENC_DEFAULT		ENC_UTF_8
#endif

#define MAX_LINE_SIZE 32768

#endif /* _DEFINES_H_ */
