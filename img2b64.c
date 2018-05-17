/*
 * =====================================================================================
 *        Created:  12/06/2017 09:30:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#include "img2b64.h"
#include "utils.h"
#include "parse.h"

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


struct opt_s opt = OPT_S_DEF;

// Search for nth or last char in string
// @str string to search
// @c   char to search for
// @n   if negative, returns last occurence of char, otherwise return nth
// Returns pointer to char position
char *
where_char( char * str, const char c, const int n )
{
    if (n == 0)
        return NULL;
    int i;
    char *p, *here;
    for (p = str, i = 0, here = NULL ; *p != '\0'; p++) {
        if (*p == c) {
            here = p;
            i++;
            if (n > 0 && i == n)
                return here;
        }
    }
    return here;
}

// Change working directory to basedir of file and change path to basename of file
// @path    path to file in a different directory
// Return 1 on success, 0 otherwise
int
basedir_change( char *path )
{
    char *slash = where_char( path, '/', -1 );
    if (slash != NULL) {
        size_t size = slash - path + 1; // +1 for null byte
        char dirpath [size];
        char *path_tmp;
        strncpy( dirpath, path, size );
        dirpath [size-1] = '\0';
        if (chdir( dirpath ) != 0) {
            err_print( "Can't change directory: %s: %s", strerror(errno), dirpath );
            exit( errno );
        }
        path_tmp = strdup( path );
        strcpy( path, path_tmp + size );
        DEBUG_PRINTF( "from '%s', new directory is '%s', new path is '%s'\n", path_tmp, dirpath, path );
        free( path_tmp );
        return 1;
    }
    return 0;
}

// Open a file
// Returns filled open file structure
struct open_file_s
file_open( const char *pathname, const char *mode )
{
	struct open_file_s f = OPEN_FILE_S_DEF;
	f.path = strdup( pathname ); // freed with file_close()

 	f.fp = fopen( f.path, mode );
	if (! f.fp) {
		//err_print( ERR_MSG_BAD_FILE, f.path );
        err_print( "Can't open file: %s: %s", strerror(errno), pathname );
        exit( errno );
	}
    return f;
}

// Close an open file
// Returns 1 on success
int
file_close( struct open_file_s of )
{
	if (fclose( of.fp ) != 0) { // this implicitly flushes the buffer too
        err_print( "%s: %s", strerror(errno), of.path );
        exit( errno );
    }
    free( of.path );
    return 1;
}

// Parse a file
// Returns number of replacement made in file
// @inf     input file
// @outf    output
int
file_parse_print( const struct open_file_s inf, struct open_file_s outf )
{
    int i = 0;
    size_t read_size = 0;
    // TODO: Why turning this to an array causes 3 times more valgrind errors??
    char * buf = xmalloc( SIZE_FREAD_BUFF + 1);

    DEBUG_PRINTF( "Parsing '%s', printing to '%s'\n", inf.path, outf.path );
    // get next string from file
    while ( (read_size = fread( buf, 1, SIZE_FREAD_BUFF, inf.fp )) != 0) {
        buf [SIZE_FREAD_BUFF] = '\0';
        // parse string
        i += process_buf( buf );
        // print to outfile
        fwrite( buf, 1, read_size, outf.fp );
    }
    fprintf( stderr, "\n" );
    // EOF or error?
    if (feof( inf.fp )) {
        fprintf( stderr, PRINT_PREFIX "%s: made %d replacements\n", inf.path, i );
    } else if (ferror( inf.fp )) {
        err_print( ERR_MSG_FILE_STREAM );
        exit( EBADF );
    }
    free( buf );
    return i;
}


int
main( int argc, char *argv[] )
{
	int count_infiles = 0;

	if (argc < 2)   goto GTNoInput;

    // Parse arguments
    {
        int gopt;
        static const struct option longopts[] = {
            { "in-place",   2, NULL, 'i' },
            { "quiet",      0, NULL, 'n' },
            { "help",       0, NULL, 'h' },
            { "version",    0, NULL, 'v' },
            { NULL,         0, NULL,  0  }  // needed for marking the end of longopts,
                                            // otherwise getopt_long() illegally
                                            // reads memory beyond the end of longopts
        };
        while ((gopt = getopt_long( argc, argv, SHORTOPTS, longopts, NULL )) != EOF) {
        	switch (gopt) {
        		case 'i':
                    opt.opt |= OPT_INPLACE;
        			// Checking backup suffix
        			if (optarg != NULL) {
        				opt.in_place_suffix = xmalloc( strlen(optarg) ); // sizeof(char) is always 1
        				strcpy( opt.in_place_suffix, optarg );
                        DEBUG_PRINTF( "in-place option enabled, suffix set\n" );
        			}
        			else {
                        DEBUG_PRINTF( "in-place option enabled, no suffix set\n" );
        			}
        			break;
        		case 'h':
        			usage( EXIT_SUCCESS );
                    break;
        		case 'n':
                    opt.opt |= OPT_QUIET;
                    DEBUG_PRINTF( "quiet option enabled\n" );
        			break;
        		case 'v':
        			print_version();
        			exit( EXIT_SUCCESS );
                    break;
        		default:
        			usage( EXIT_FAILURE );
        			break;
        	}				/* -----  end switch  ----- */
        }
        if (DEBUG)      opt_print(argc, argv);
    }

	if ((count_infiles = argc - optind) <= 0) {
        goto GTNoInput;
    }
    else if (opt.opt & OPT_QUIET && ! (opt.opt & OPT_INPLACE)) {
        fputs( PRINT_PREFIX "No file to edit and no printing to stdout, so... nothing to be done!", stderr );
        exit( EXIT_SUCCESS );
    }
    // Process files
    for (; count_infiles > 0 ; optind++, count_infiles-- ) {
        char inp [ sizeof argv[optind] ];
        strcpy( inp, argv[optind] );
        basedir_change( inp );
        struct open_file_s inf  = file_open( inp,           "r" );
        struct open_file_s outf = file_open( opt.in_place_suffix ? OUTFILE_PATH : OUTFILE_STDOUT ,  "w" );
        file_parse_print( inf, outf );
        file_close( inf );
        file_close( outf );
    }
    exit( EXIT_SUCCESS );
	// FILE *infile;
	//
	// arg parsing
	// record all infiles to an array


	// regex_t re;
	FILE *test_file1;
	FILE *test_file2;

	exit(0);

	fprintf(stderr, PRINT_PREFIX "Opening files...\n");
	test_file1 = fopen("./Tests/file1.txt", "r");
	test_file2 = fopen("./Tests/file2.txt", "a");

	fprintf(stderr, PRINT_PREFIX "Writing files...\n");
	fputs("lol\n", test_file2);

	fprintf(stderr, PRINT_PREFIX "Closing files...\n");
	fclose(test_file1);
	fclose(test_file2);

	return EXIT_SUCCESS;
GTNoInput:
    err_print( ERR_MSG_NO_INPUT );
    exit( 127 );
}				/* ----------  end of main  ---------- */
