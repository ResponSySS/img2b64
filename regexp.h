/*
 * =====================================================================================
 *        Created:  03/17/2018 05:51:07 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#ifndef _REGEXP_H
#define _REGEXP_H


#include <stdio.h>


#define SIZE_ERR_BUFF 	100
// Match 1 is 'src' parameter
#define RE_PATTERN 	".*src[[:blank:]]*=[[:blank:]]*['\"][[:blank:]]*([^'\"]+)[[:blank:]]*['\"].*"

int
extract_src( char src[], const char * img, ssize_t len );


#endif
