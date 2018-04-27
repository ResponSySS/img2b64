/*
 * =====================================================================================
 *    Description:  String processing
 *
 *        Created:  03/17/2018 05:15:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#define _GNU_SOURCE // needed by strcasestr

#include "parse.h"
#include "img2b64.h"
#include "encode.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <stdnoreturn.h>
#include <errno.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>


// Removes newline characters inside string
// Returns number of newlines removed
int
strip_nl ( char * str )
{
    int i;
    char *nl;
    for ( i = 0, nl = NULL ; (nl = strchr( str, '\n' )) != NULL ; *nl=' ', i++)
        ;
    return i;
}

// Extract substring from string with start and end offsets
// @str     string to fill
// @off     array of start offset ([0]) and end offset ([1])
// @maxlen  max length of string to fill
// Returns 1 on success, 0 otherwise
int
extract_substr( char str[], const char *off[2], size_t maxlen )
{
    size_t len = off[1] - off[0] + 1; // +1 for terminating null byte
    if (len > maxlen) {
        DEBUG_PRINTF( "%s:%d: substring {%10s} too big to be extracted from {%s}", __FILE__, __LINE__, off[0], str );
        return 0;
    }
    strncpy( str, off[0], len - 1 );
    str[len-1] = '\0';
    return 1;
}

// Extract 'src' parameter from <img> tag in string
// @src     string to fill
// @img     <img> string to extract from
// @maxlen  max length of string to fill
// Returns 1 on success, 0 otherwise
int
extract_src( char src[], const char * img, size_t maxlen )
{
	int reti = 0;
	regex_t preg;
	// [0] matchs the whole regex, [1] is first match, etc.
	regmatch_t pm[2];
	char err_buf [SIZE_ERR_BUFF];

	// Compile RE pattern
	reti = regcomp( &preg, RE_PATTERN, REG_ICASE | REG_EXTENDED );
	if (reti) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		err_print( "Can't compile regex pattern", err_buf );
		return 0;
	}
    // Find RE pattern
	reti = regexec( &preg, img, 2, pm, 0 );
	if (reti) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		err_print( "Can't execute regex pattern", err_buf );
		return 0;
	}
	// Printing matches
	if (DEBUG) {
		int i;
		for ( i=0 ; i < (sizeof pm / sizeof(regmatch_t)) ; i++) {
            DEBUG_PRINTF( "    match %s [%d-%d]: {%.*s}\n", i ? "1 ('src')" : "0 (<img>)",
                    pm[i].rm_so, pm[i].rm_eo,
                (int) pm[i].rm_eo - pm[i].rm_so, img + pm[i].rm_so );
		}
	}

    size_t len = pm[1].rm_eo - pm[1].rm_so + 1; // + 1 for terminating null byte
    strncpy( src, img + pm[1].rm_so, len - 1 );
    src[len-1] = '\0';
	return 1;
}

// TODO: divide this in tinier function
// TODO:    1) what if the buffer contains the start of <img> but not its end?
//          2) what if it ends like "[...] <im"
//          solutions:
//              1) if end of <img> not found, refetch from start of <img>
//              2) always refetch next buffer 3 characters before end of previous buffer
// Search and replace <img> tag with base64
// Returns number of replacements made
// @str         string to parse
int
process_buf( char * buf )
{
    char *img_off[2]; // [0]: start offset, [1]: end offset
    int i; // Number of matches

    // Find start offset and end offset of <img> in str
	for (i = 0; (img_off[0] = strcasestr( buf, "<img" )) != NULL ;
            buf = ++(img_off[0]), i++) {
	    if ((img_off[1] = strcasestr( img_off[0], "/>" )) != NULL) {
            DEBUG_PRINTF( "<img> #%d:\n", i );
            char img_full [SIZE_FREAD_BUFF];
            char src [SIZE_FREAD_BUFF];
            char * b64_str;
            // Extract <img> tag from whole string
            extract_substr( img_full, (const char **)img_off, SIZE_FREAD_BUFF );
            // Strip newlines
            int j = strip_nl( img_full );
            DEBUG_PRINTF( "  %d newlines stripped\n", j );
            // Extract 'src' parameter
	    	extract_src( src, img_full, SIZE_FREAD_BUFF );
            // Process file from 'src' string
            b64_str = b64_process_resource( src );
            DEBUG_PRINTF( "base64 result: %s", b64_str );
            // Change original <img> tag
	    } else {
            err_print( "Incomplete <img> tag" , NULL );
        }
	}
// while (fgets( buf, SIZE_FREAD_BUFF, infile.fp ) != NULL) {
// 	// looking for '<img'
//  	if ((ptr_img_start = strcasestr( buf, "<img" )) != NULL) {
// 		ptr_img_full = ptr_img_start;
// 		printf( "IMG TAG STARTS AT: %s", ptr_img_start );
// 		// looking for 'src'
// 		int nl_count = 0;
// 		while ((ptr_img_end = strcasestr( buf, "/>" )) == NULL) {
// 			fgets( buf, SIZE_FREAD_BUFF, infile.fp );
// 			// ensure buf is not empty for strtok
// 			if (buf[0] != '\0') {
// 				puts( "ok" );
// 				strtok( buf, "\n" );
// 				strcpy( buf_cp, buf );
// 			}
// 			nl_count++;
// 			// strcat( ptr_img_full, buf_cp );
// 		}
// 		printf( "  IMG ENDS %d LINE(S) UNDER AT: %s", nl_count, ptr_img_end );
// 		printf( "    FULL: %s", buf_cp );
// 	}
// }
	return i;
}
