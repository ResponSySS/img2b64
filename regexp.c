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
int
extract_src( char src[], const char * img, ssize_t maxlen )
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
