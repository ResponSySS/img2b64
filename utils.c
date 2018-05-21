/*
 * =====================================================================================
 *    Description:  Misc functions
 *
 *        Created:  03/17/2018 05:24:27 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#define _GNU_SOURCE // needed by asprintf

#include "utils.h"
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
#include <stdarg.h>


// Print standard-formatted error message
void
err_print( const char * fmt, ... )
{
    va_list args;

    va_start( args, fmt );
	fprintf( stderr, PRINT_PREFIX "ERROR: " );
    vfprintf( stderr, fmt, args );
	fprintf( stderr, "\n" );
    va_end( args );
}

// Print xmalloc failure message
static inline noreturn void
xmalloc_failed( size_t size )
{
	char *err_str;
	asprintf( &err_str, XMALLOC_ERR_STR, size );
	err_print( err_str );

	exit( EXIT_FAILURE );
}

// Memory allocator with checking
void *
xmalloc( size_t size )
{
	void *newmem;

	if (size == 0) 	{ size = 1; }
	newmem = malloc( size );
	if (!newmem) 	{ xmalloc_failed( size ); }

	return (newmem);
}

// Print version
// @PROGRAM_NAME    string macro for program name
// @VERSION         string macro for version
void
print_version( void )
{
	fprintf( stdout, "%s v%s\n\
Written by Sylvain Saubier (<http://SystemicResponse.com>)\n\
Report bugs to: <feedback@sylsau.com>\n", 
		PROGRAM_NAME, VERSION);
}

// Print usage/help message
noreturn void
usage( const int status )
{
	FILE *out = status ? stderr : stdout;

	fprintf(out, "%s v%s\nUsage: %s [OPTION]... {FILE}\n",
		PROGRAM_NAME, VERSION, PROGRAM_NAME);
	fputs("\
Encodes all images with base64.\n\
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
    Report bugs to: <feedback@sylsau.com>\n", out);

	exit(status);
}

// Print options
void
opt_print( const int argc, char *argv[] )
{
	fputs( "---\n", stderr );
	fprintf( stderr, "optind==%d; argc==%d\n", optind, argc);
	fprintf( stderr, "argv==\"");
    {
        int i;
        for (i=0 ; i < argc ; i++) {
            fprintf( stderr, "%s\t", argv[i] );
	    }
    }
	fprintf( stderr, "\"\n");
	fprintf( stderr, "OPT FIELD: 0x%x\n", opt.opt );
	fprintf( stderr, "QUIET: (%sset)\n", opt.opt & OPT_QUIET ? "" : "un" );
	fprintf( stderr, "SUFFIX: %s\n", opt.in_place_suffix );
	fprintf( stderr, "FILE COUNT: %d\n", argc - optind );
	fputs( "---\n", stderr );
	
	fflush( stderr ); // Flush stream buffer before resuming so it doesn't mix up things
}
