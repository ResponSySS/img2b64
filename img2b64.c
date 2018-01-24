/*
 * =====================================================================================
 *    Description:  
 *
 *        Version:  0.6
 *        Created:  12/06/2017 09:30:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@systemicresponse.com
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <stdnoreturn.h>
#include <errno.h>


#define PROGRAM_NAME 	"img2b64"
#define VERSION 	"0.8"

noreturn void
usage( int status )
{
	FILE *out = status ? stderr : stdout;

	fprintf( out, "%s v%s\nUsage: %s [OPTION]... {FILE}\n", 
			PROGRAM_NAME, VERSION, PROGRAM_NAME );
	fputs( "\
Convert all <img> tags of HTML file 'FILE' with base64 encoding and print result\n\
to standard output.\n\n\
OPTIONS\n\
    -i, --in-place [SUFFIX]\n\
                           edit files in place (makes backup if SUFFIX supplied)\n\
    -n, --quiet            suppress automatic printing to standard output\n\
        --help             display this help and exit\n\
        --version          output version information and exit\n\
EXAMPLES\n", out );
	fprintf( out, 
"    %s page.html -i.old --quiet\n", PROGRAM_NAME );
	fputs(
"        makes backup to 'page.html.old' and silently parse 'page.html'.\n", out );
	fputs( "\
CONTACT\n\
    Written by Sylvain Saubier (<http://SystemicResponse.com>)\n", out );
	if (status)
		fputs( "\
    Report bugs to: <feedback@systemicresponse.com>\n", out );

	exit( status );
}

int 
main( int argc, char *argv[] )
{
	//regex_t re;
	FILE * test_file1;
	FILE * test_file2;

	usage( 127 );

	fprintf( stderr, "Opening files...\n" );
	test_file1 = fopen( "./Tests/file1.txt", "r" );
	test_file2 = fopen( "./Tests/file2.txt", "a" );

	fprintf( stderr, "Writing files...\n" );
	fputs( "lol\n", test_file2 );

	fprintf( stderr, "Closing files...\n" );
	fclose( test_file1 );
	fclose( test_file2 );

	return EXIT_SUCCESS;
}				/* ----------  end of main  ---------- */
