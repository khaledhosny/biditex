#ifndef _DEFINES_H_
#define _DEFINES_H_

enum { ENC_UTF_8 , ENC_ISO_8859_8, ENC_CP1255 };

/*******************/
/* PROJECT DEFINES */
/*******************/

#define SMART_FRIBIDI
/* This prevents from fribidi process sequences like "\section"
 * it force libfribidi to ignore them. This is temporary hack
 * that allows easier work on _this stage_ only. In future
 * normal commands parsing should be added. */

#define MAX_LINE_SIZE	32768
/* Maximal size of input/output line in the text */

#define MAX_COMMAND_LEN	256

#define ENC_DEFAULT		ENC_UTF_8
/* The default IO encoding - UTF-8 */

/*******************/

#endif /* _DEFINES_H_ */
