/*
 * =====================================================================================
 *        Created:  03/17/2018 05:51:07 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#ifndef _PARSE_H
#define _PARSE_H


#include <stdio.h>


#define SIZE_ERR_BUFF 	100
// Match 1 is 'src' parameter
#define RE_PATTERN 	".*src[[:blank:]]*=[[:blank:]]*['\"][[:blank:]]*([^'\"]+)[[:blank:]]*['\"].*"

int
strip_nl ( char * str );

int
extract_substr( char str[], const char *off[2], size_t maxlen );

int
extract_src( char src[], const char * img, size_t maxlen );

int
process_buf( char * buf );

#endif
