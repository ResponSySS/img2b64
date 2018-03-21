/*
 * =====================================================================================
 *    Description:  Misc functions
 *
 *        Created:  03/17/2018 05:24:27 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@systemicresponse.com
 * =====================================================================================
 */

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

#include "img2b64.h"
#include "utils.h"


void
err_print( const char *err_str, const char *opt )
{
	fprintf( stderr, "%s: %s%s%s\n", PROGRAM_NAME, err_str, *opt ? ": " : "", opt );
}

void
xmalloc_failed( size_t size )
{
	char *err_str;
	asprintf( &err_str, XMALLOC_ERR_STR, size );
	err_print( err_str, "" );

	exit( EXIT_FAILURE );
}

void *
xmalloc( size_t size )
{
	void *newmem;

	if (size == 0) 	{ size = 1; }
	newmem = malloc (size);
	if (!newmem) 	{ xmalloc_failed (size); }

	return (newmem);
}

void
version_print( void )
{
	fprintf( stdout, "%s v%s\n\
Written by Sylvain Saubier (<http://SystemicResponse.com>)\n\
Report bugs to: <feedback@systemicresponse.com>\n", 
		PROGRAM_NAME, VERSION);
}

noreturn void
usage( int status )
{
	FILE *out = status ? stderr : stdout;

	fprintf(out, "%s v%s\nUsage: %s [OPTION]... {FILE}\n",
		PROGRAM_NAME, VERSION, PROGRAM_NAME);
	fputs("\
Convert all images with base64 encoding.\n\
Changes <img> tags of HTML file 'FILE' with their base64 strings and print result\n\
to standard output.\n\n\
OPTIONS\n\
    -i, --in-place [SUFFIX]\n\
                           edit files in place (makes backup if SUFFIX supplied)\n\
    -n, --quiet            suppress automatic printing to standard output\n\
    -h, --help             display this help and exit\n\
        --version          output version information and exit\n\
EXAMPLES\n", out);
	fprintf(out, "    %s page.html -i.old --quiet\n", PROGRAM_NAME);
	fputs
	    ("        makes backup to 'page.html.old' and silently parse 'page.html'.\n",
	     out);
	fputs("\
CONTACT\n\
    Written by Sylvain Saubier (<http://SystemicResponse.com>)\n", out);
	if (status)
		fputs("\
    Report bugs to: <feedback@systemicresponse.com>\n", out);

	exit(status);
}

void
opt_print( const int argc, char *argv[] )
{
	puts( "---" );
	printf( "optind==%d; argc==%d\n", optind, argc);
	printf( "argv==\"");
	int i = 0;
	while (i < argc) {
		printf( "%s\t", argv[i] );
		i++;
	}
	printf( "\"\n");
	printf( "QUIET is %sSET\n", quiet ? "" : "UN" );
	printf( "SUFFIX is %s\n", in_place_suffix );
	printf( "FILE COUNT is %d\n", argc-optind );
	puts( "---" );
	//be sure to flush stdout before resuming so it doesn't mix up things
	fflush( stdout );
}
