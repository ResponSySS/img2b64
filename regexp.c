/*
 * =====================================================================================
 *    Description:  Regex matching functions
 *
 *        Created:  03/17/2018 05:15:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#include "regexp.h"
#include "img2b64.h"
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

// Replace <img> string in given str
// Returns number of replacements made
int
regex_replace( const char * str )
{
    return 0;
}

// Extract 'src' parameter from <img> tag in string
// Returns 'src' string
char *
regex_extract_src( const char * s_img )
{
	char *s_src = NULL;
	int reti = 0;
	regex_t preg;
	// [0] matchs the whole regex, [1] is first match, etc.
	regmatch_t pm[2];
	char *err_buf = xmalloc( SIZE_ERR_BUFF ); // sizeof(char) is, by definition, always 1

	DEBUG_PRINTF( "  <img>: {%s}\n", s_img );
	// Doing da regex things
	reti = regcomp( &preg, RE_PATTERN, REG_ICASE | REG_EXTENDED );
	if (reti) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		printf( "ERROR: %s\n", err_buf );
		return NULL;
	}
	reti = regexec( &preg, s_img, 2, pm, 0 );
	if (reti) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		printf( "ERROR: %s\n", err_buf );
		return NULL;
	}
    size_t size_src = pm[1].rm_eo - pm[1].rm_so;

	// Printing matches
	if (DEBUG) {
		int i;
		for ( i=0 ; i < (sizeof pm / sizeof(regmatch_t)) ; i++) {
            DEBUG_PRINTF( "    match %d [%d-%d]: {%.*s}\n", i, pm[i].rm_so, pm[i].rm_eo,
                (int) size_src, s_img + pm[i].rm_so );
		}
	}
    s_src = xmalloc( size_src + 1 ); // +1 for null byte
    strncpy( s_src, s_img + pm[1].rm_so, size_src );
    *(s_src + size_src) = '\0';
	return s_src;
}
