/*
 * =====================================================================================
 *        Created:  12/06/2017 09:30:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#define _GNU_SOURCE // needed by strcasestr

#include "img2b64.h"
#include "utils.h"
#include "regexp.h"

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

// Open a file
// Returns filled open file structure
struct open_file_s
file_open( const char *pathname, const char *mode )
{
	struct open_file_s f = OPEN_FILE_S_DEF;
	f.path = xmalloc( sizeof pathname );
	strcpy( f.path, pathname );

 	f.fp = fopen( f.path, mode );
	if (! f.fp) {
		//err_print( ERR_MSG_BAD_FILE, f.path );
		err_print( strerror(errno), pathname );
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
		err_print( strerror(errno), of.path );
        exit( errno );
    }
    return 1;
}

// TODO:    1) what if the buffer contains the start of <img> but not its end?
//          2) what if it ends like "[...] <im"
//          solution:
//              1) if end of <img> not found, refetch from start of <img>
//              2) always refetch next buffer 3 characters before end of previous buffer
// Search and replace <img> tag with base64
// Returns number of replacements made
// @str         string to parse
int
parse_str( char * str )
{
	char *ptr_img_start = NULL;
	char *ptr_img_end = NULL;
	char *img_tag_full = NULL;
	char *src = NULL;
    //char sstr = str; // start of str

	for (; (ptr_img_start = strcasestr( str, "<img" )) != NULL ; str = ++ptr_img_start) {
        DEBUG_PRINTF( "<img> start found\n" );
		if ((ptr_img_end = strcasestr( ptr_img_start, "/>" )) != NULL) {
            // Extract <img> tag from whole string
            ssize_t cc = ptr_img_end - ptr_img_start; // char count
            img_tag_full = xmalloc( cc + 1 ); // + 1 for null byte at the end
            strncpy( img_tag_full, ptr_img_start, cc );
            *(img_tag_full + cc) = '\0';
			// Removes newline characters inside <img> tag
            {
                char *nl;
                for ( nl = NULL ; (nl = strchr( img_tag_full, '\n' )) != NULL ; *nl=' ')
                    ;
            }
			DEBUG_PRINTF( "  length: %ld\n", cc );
            // Extract 'src' string parameter
			src = regex_extract_src( img_tag_full );
            // Process 'src' string
            // b64_enc ( src );
            DEBUG_PRINTF( "  extracted 'src': \"%s\"\n", src );
            // Change original <img> tag
			free( img_tag_full );
		} else {
            err_print( "incomplete <img> tag" , NULL );
        }
	}
// while (fgets( buf, SIZE_FREAD_BUFF, infile.fp ) != NULL) {
// 	// looking for '<img'
//  	if ((ptr_img_start = strcasestr( buf, "<img" )) != NULL) {
// 		ptr_img_full = ptr_img_start;
// 		printf( "IMG TAG STARTS AT: %s", ptr_img_start );
// 		// looking for 'src'
// 		int nl_count = 0;
// 		while ((ptr_img_end = strcasestr( buf, "/>" )) == NULL) {
// 			fgets( buf, SIZE_FREAD_BUFF, infile.fp );
// 			// ensure buf is not empty for strtok
// 			if (buf[0] != '\0') {
// 				puts( "ok" );
// 				strtok( buf, "\n" );
// 				strcpy( buf_cp, buf );
// 			}
// 			nl_count++;
// 			// strcat( ptr_img_full, buf_cp );
// 		}
// 		printf( "  IMG ENDS %d LINE(S) UNDER AT: %s", nl_count, ptr_img_end );
// 		printf( "    FULL: %s", buf_cp );
// 	}
// }
	return EXIT_SUCCESS;
}

// Parse a file
// Returns number of replacement made in file
// @inf     input file
// @outf    output
int
file_parse( const struct open_file_s inf, struct open_file_s outf )
{
	DEBUG_PRINTF( "Parsing '%s', printing to '%s'\n", inf.path, outf.path );
    // get next str from file
        // parse_str (str)
        // print to outfile
    int i = 0;
    size_t read_size = 0;
	char buf    [SIZE_FREAD_BUFF];
	//char buf_res[SIZE_FREAD_BUFF];
	while ( (read_size = (fread( buf, 1, SIZE_FREAD_BUFF, inf.fp ))) != 0) {
        i += parse_str( buf );
        fwrite( buf, 1, read_size, outf.fp );
    }
    // EOF or error?
    if (feof( inf.fp )) { // EOF reached
        DEBUG_PRINTF( "EOF reached!\n" );
    } else if (ferror( inf.fp )) { // error
        err_print( "error processing file stream", NULL );
    }
    DEBUG_PRINTF( "Made %d replacements\n", i );
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
        fputs( "No file to edit and no printing to stdout, so... nothing to be done!", stderr );
        exit( EXIT_SUCCESS );
    }
    // Process files
    for (; count_infiles > 0 ; optind++, count_infiles-- ) {
        struct open_file_s inf  = file_open( argv[optind], "r" );
        struct open_file_s outf = file_open( OUTFILE_PATH, "w" );
        file_parse( inf, outf );
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

	fprintf(stderr, "Opening files...\n");
	test_file1 = fopen("./Tests/file1.txt", "r");
	test_file2 = fopen("./Tests/file2.txt", "a");

	fprintf(stderr, "Writing files...\n");
	fputs("lol\n", test_file2);

	fprintf(stderr, "Closing files...\n");
	fclose(test_file1);
	fclose(test_file2);

	return EXIT_SUCCESS;
GTNoInput:
    err_print( ERR_MSG_NO_INPUT, NULL );
    exit( 127 );
}				/* ----------  end of main  ---------- */
