/*
 * =====================================================================================
 *    Description:  Regex matching functions
 *
 *        Created:  03/17/2018 05:15:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@systemicresponse.com
 * =====================================================================================
 */

#include "regexp.h"
#include "img2b64.h"

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


char *
regex_extract_src( const char * str )
{
	char *src = NULL;
	int reti = 0;
	regex_t preg;
	//[0] matchs the whole regex, [1] is first match, etc.
	regmatch_t pm[2];
	char *err_buf = malloc( sizeof(char) * SIZE_ERR_BUFF );

	printf( "STRING:\n\t%s\nREGEX:\n\t%s\n", str, RE_PATTERN );
	return NULL;
	// Doing da regex things
	reti = regcomp( &preg, RE_PATTERN, REG_ICASE | REG_EXTENDED );
	if (reti != 0) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		printf( "ERROR: %s\n", err_buf );
		return NULL;
	}
	reti = regexec( &preg, str, 2, pm, 0 );
	if (reti != 0) {
		regerror( reti, &preg, err_buf, SIZE_ERR_BUFF );
		printf( "ERROR: %s\n", err_buf );
		return NULL;
	}

	// Printinf matches
	{
		int i = 0;
		while (i < (sizeof pm / sizeof(regmatch_t))) {
			printf( "[%d]rm_so == %d\n[%d]rm_eo == %d\n",
				i, pm[i].rm_so, i, pm[i].rm_eo );
			printf( "MATCH N°%d:\n\t<<%.*s>>\n", i,
				(int)(pm[i].rm_eo - pm[i].rm_so), str + pm[i].rm_so );
			i++;
		}
	}

	// No need freeing for stack vars
	//regfree( &preg );
	puts( "SUCCESS!" );
	//TODO: fill it before!
	return src;
}
