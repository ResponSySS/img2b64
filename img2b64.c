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
#include <assert.h>


struct Opt opt = { 0x0, NULL };


// Search for nth or last char in string
// @str     string to search
// @c       char to search for
// @wh      if negative, returns last occurence of char, otherwise return nth
// Returns pointer to char position
char *
where_char( char *restrict str, const char c, const int wh )
{
    assert( str );
    if (wh == 0)
        return NULL;

    int i = 0;
    char *p = str, *here = NULL;
    for (; *p != '\0'; p++) {
        if (*p == c) {
            here = p;
            i++;
            if (wh > 0 && i == wh)
                return here;
        }
    }
    return here;
}

// Split path into dirname and basename
// @path    path to split
// @dname   dirname string to fill if present (MUST BE LARGE ENOUGH) or NULL
// @bname   basename string to fill if present (MUST BE LARGE ENOUGH) or NULL
// Returns 1 upon success, 0 otherwise
int
split_path( char *restrict path, char *restrict dname, char *restrict bname )
{
    assert( path );

    char *slash = where_char( path, '/', -1 );
    if (slash) {
        size_t size = slash - path + 1; // +1 for null byte
        if (dname) {
            strncpy( dname, path, size );
            dname [--size] = '\0';
            while (dname [--size] == '/') // remove trailing '/'
                dname[size] = '\0';
        }
        if (bname)
            strcpy( bname, ++slash );
        DEBUG_PRINTF( "Splitted path '%s' into '%s' and '%s'\n", path, dname, bname );
        return 1;
    } else // no '/' found in path
        return 0;
}

// Cycle through two directories
// @path    path to directory to change to, if NULL change to previous dir
// Returns 1 if directory was changed to another one, 2 if directory was changed
// to the previous one, exit with errno otherwise
int
pushd( char *path )
{
    assert( path );
    static char *dir_prev;  // There is no actual workable limit for pathnames,
                            // as it's OS-dependent in a very twisted fashion

    DEBUG_PRINTF( "In directory '%s'\n", getcwd( NULL, 0 ) );
    if (path) { // cd to path
        DEBUG_PRINTF( "Moving towards '%s'\n", path );
        if (dir_prev)
            free( dir_prev );
        dir_prev = strdup( getcwd( NULL, 0 ) );
        if (chdir( path ) != 0) {
                err_print( "Can't change directory: %s: %s", strerror(errno), path );
                exit( errno );
        }
        return 1;
    } else { // cd to previous directory
        DEBUG_PRINTF( "Moving to previous directory\n" );
        if (chdir( dir_prev ) != 0) {
            err_print( "Can't change directory: %s: %s", strerror(errno), dir_prev );
            exit( errno );
        }
        return 2;
    }
}

// Change working directory to basedir of file and change path to basename of file
// @path    path to file in a different directory
// Return 1 on success, 0 otherwise
int
basedir_change( char *path )
{
    char d[strlen( path )], b[strlen( path )];
    if (split_path( path, d, b )) {
        if (chdir( d ) != 0) {
            err_print( "Can't change directory: %s: %s", strerror(errno), d );
            exit( errno );
        }
        DEBUG_PRINTF( "New directory is '%s', new path is '%s'\n", d, b );
        strcpy( path, b );
        return 1;
    } else
        return 0;
}

// Open a file
// Returns filled open file structure
struct Open_file
file_open( const char *restrict pathname, const char *restrict mode )
{
    assert( pathname && mode );
	struct Open_file f = { NULL, NULL };
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
file_close( struct Open_file of )
{
	if (fclose( of.fp ) != 0) { // this implicitly flushes the buffer too
        err_print( "%s: %s", strerror(errno), of.path );
        exit( errno );
    }
    free( of.path );
    return 1;
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
        				opt.in_place_suffix = xmalloc( strlen( optarg )+1 ); // TODO: is this all okay? null byte okay? string okayly terminated?
        				strcpy( opt.in_place_suffix, optarg );
                        DEBUG_PRINTF( "in-place option enabled, suffix set to '%s'\n", opt.in_place_suffix );
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
        char inp [ strlen( argv[optind] )+1 ];
        char d[ strlen( argv[optind] )+1 ], b[ strlen( argv[optind] )+1 ];
        strcpy( inp, argv[optind] );
        //basedir_change( inp );
        split_path( inp, d, b );
        pushd( d );
        struct Open_file inf  = file_open( b, "r" );
        struct Open_file outf = file_open( opt.in_place_suffix ? OUTFILE_PATH : OUTFILE_STDOUT ,  "w" );
        parse_file_print( inf, outf );
        file_close( inf );
        file_close( outf );
        pushd( NULL );
    }
    exit( EXIT_SUCCESS );
	// FILE *infile;
	// regex_t re;
	FILE *test_file1;
	FILE *test_file2;

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
