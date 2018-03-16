/*
 * =====================================================================================
 *    Description:  
 *
 *        Created:  12/06/2017 09:30:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@systemicresponse.com
 * =====================================================================================
 */

#define _GNU_SOURCE 1 /* Required by asprintf */
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

// GLOBALS
#define PROGRAM_NAME 		"img2b64"
#define VERSION 		"0.8"

// ERRORS
// Sometimes require another argument for clarity (ones with 'y')
#define ERR_MSG_NO_INPUT 	"no input file specified" 	
#define ERR_MSG_BAD_ARG 	"unknown argument" 		// y
#define ERR_MSG_NO_FILE 	"file does not exist" 		// y
#define ERR_MSG_BAD_FILE 	"can't open file" 		// y

#define XMALLOC_ERR_STR 	"out of memory allocating %lu bytes\n"

// OPTIONS
#define SHORTOPTS 		"i::nh"
bool quiet = false;
bool in_place = false;
char *in_place_suffix = NULL;


void
err_print( const char *err_str, const char *opt )
{
	fprintf( stderr, "%s: %s%s%s\n", PROGRAM_NAME, err_str, *opt ? ": " : "", opt );
}

void
xmalloc_failed (size_t size)
{
	char *err_str;
	asprintf( &err_str, XMALLOC_ERR_STR, size );
	err_print( err_str, "" );

	exit( EXIT_FAILURE );
}

void *
xmalloc (size_t size)
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

int
infile_parse( char *pathname )
{
	if (access( pathname, R_OK|W_OK ) == -1) {
		err_print ( ERR_MSG_NO_FILE, pathname); exit( errno );
	}

	FILE *infile;
 	infile = fopen( pathname, "r+" );
	if (! infile) {
		err_print( ERR_MSG_BAD_FILE, pathname ); exit( errno );
	}

	fclose( infile );
}

int
main( int argc, char *argv[] )
{
	int opt;
	static const struct option longopts[] = {
		{ "in-place", 2, NULL, 'i' },
		{ "quiet", 0, NULL, 'n'},
		{ "help", 0, NULL, 'h'},
		{ "version", 0, NULL, 'v'}
	};

	int count_infiles = 0;

	if (argc < 2) 	{ err_print( ERR_MSG_NO_INPUT, "" ); usage(127); }
//	err_print( ERR_MSG_NO_INPUT, "" );
//	err_print( ERR_MSG_BAD_ARG, "--fuck-you" );
//	err_print( ERR_MSG_BAD_FILE, "./myfile.tits" );
	
	//TODO: crash unexpectedly when invalid option string (like "--hekeko")
	while ((opt = getopt_long( argc, argv, SHORTOPTS, longopts, NULL )) != EOF) {
		switch (opt) {
			case 'i':
				in_place = true;
				//Checking backup suffix
				if (optarg != NULL) {
					in_place_suffix = xmalloc( strlen(optarg) * sizeof(char) );
					strcpy( in_place_suffix, optarg );
				}
				else {
					puts( "suffix not set" );
				}
				break;
			case 'h':
				usage( EXIT_SUCCESS );
			case 'n':
				quiet = true;
				break;
			case 'v':
				version_print();
				exit( EXIT_SUCCESS );
			default:
				//TODO: not working?
				usage( EXIT_FAILURE );
				break;
		}				/* -----  end switch  ----- */
	}
#ifdef DEBUG
	opt_print(argc, argv);
#endif

	count_infiles = argc - optind;

	if (count_infiles <= 0) 	{ err_print( ERR_MSG_NO_INPUT, "" ) ; }
	while (count_infiles > 0) {
		infile_parse( argv[optind] );
		optind++; count_infiles--;
	}
	//FILE *infile;
	//
	//arg parsing
	// record all infiles to an array


	//regex_t re;
	FILE *test_file1;
	FILE *test_file2;

	exit(1);

	fprintf(stderr, "Opening files...\n");
	test_file1 = fopen("./Tests/file1.txt", "r");
	test_file2 = fopen("./Tests/file2.txt", "a");

	fprintf(stderr, "Writing files...\n");
	fputs("lol\n", test_file2);

	fprintf(stderr, "Closing files...\n");
	fclose(test_file1);
	fclose(test_file2);

	return EXIT_SUCCESS;
}				/* ----------  end of main  ---------- */
