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
#include "img2b64.h"

#define SIZE_ERR_BUFF 	128
// Match 1 is 'src' parameter
#define RE_PATTERN_SRC 	        ".*src[[:blank:]]*=[[:blank:]]*['\"][[:blank:]]*([^'\"]+[^'\"[:blank:]]+)[[:blank:]]*['\"].*"
//                                                                              ^ avoid trailing whitespaces
#define RE_PATTERN_B64_SRC 	    "data:image/(png|jpg|bmp|...);(charset=...;|)base64,[[:blank:]]*[a-zA-Z0-9+/]+"
//base64 data string format:    data:image/{type}[;charset=<charset>];base64,{encoded data}

int
strinstr( char **here, char *haystack, const char needle[] );

int
strip_nl( char * str );

int
extract_substr( char str[], const char *off[2], size_t maxlen );

int
extract_src( char src[], const char * img, size_t maxlen );

// process_buf() status
enum Process_buf_status {
    PB_NO_MATCH                 = 0x0,
    PB_MATCH                    = 0x1,
    PB_INCOMPLETE_MATCH_START   = 0x2,
    PB_INCOMPLETE_MATCH_END     = 0x4
};

int
process_buf( char * buf );

int
parse_file_print( const struct Open_file inf, struct Open_file outf );

#endif
