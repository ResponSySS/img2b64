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


// TODO: remake it with va_args seriously wth is that bro
// Print standard-formatted error message
void
err_print( const char *err_str, const char *opt )
{
	fprintf( stderr, (opt ? "%s%s: %s\n" : "%s%s\n"), "ERROR: " PROGRAM_NAME ": ", err_str, opt ); 
    // From C Standard Draft: "If the format is exhausted while arguments remain, the excess arguments are evaluated (as always) but are otherwise ignored"
}

// Print xmalloc failure message
noreturn void
xmalloc_failed( size_t size )
{
	char *err_str;
	asprintf( &err_str, XMALLOC_ERR_STR, size );
	err_print( err_str, "" );

	exit( EXIT_FAILURE );
}

// Memory allocator with checking
void *
xmalloc( size_t size )
{
	void *newmem;

	if (size == 0) 	{ size = 1; }
	newmem = malloc (size);
	if (!newmem) 	{ xmalloc_failed (size); }

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
usage( int status )
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

// TODO: redo that with opt_s struct
// TODO: redo that with _va_args to support various options organization by various programs
// Print options
void
opt_print( const int argc, char *argv[] )
{
	puts( "---" );
	printf( "optind==%d; argc==%d\n", optind, argc);
	printf( "argv==\"");
    {
        int i;
        for (i=0 ; i < argc ; i++) {
            printf( "%s\t", argv[i] );
	    }
    }
	printf( "\"\n");
	printf( "QUIET: [%sSET]\n", opt.opt & OPT_QUIET ? "" : "UN" );
	printf( "SUFFIX: %s\n", opt.in_place_suffix );
	printf( "FILE COUNT: %d\n", argc - optind );
	puts( "---" );
	
	fflush( stdout ); // Flush stdout before resuming so it doesn't mix up things
}
