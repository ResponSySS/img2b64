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


struct opt_s opt        = { 00, NULL };

// TODO: What does it do????
int
infile_parse( struct open_file_s infile )
{
	DEBUG_PRINTF( "READING '%s'\n", infile.path );
	char buf[SIZE_FREAD_BUFF];
	char buf_cp[SIZE_FREAD_BUFF];
	char *ptr_img_start = NULL;
	char *ptr_img_end = NULL;
	char *img_tag_full = NULL;
	char *src = NULL;
//	while (fread( buf, sizeof(char), SIZE_FREAD_BUFF, infile.fp )) {
// 		printf( "-%s", buf);
// 		// looking for '<'
// 		char *ptr_img_start = strchr( buf, '<' );
// 		printf( "this must be '<': ...%c%c%c...", *(ptr_img_start-1), *(ptr_img_start), *(ptr_img_start+1) );
// 		// retrieves next 3 chars
// 		// ensure no EOF, nor end of buf
// 		// if == "img", then go to next step
// 	}

	while ((fread( buf, sizeof(char), SIZE_FREAD_BUFF, infile.fp )) != (size_t)0) {
	 	if ((ptr_img_start = strcasestr( buf, "<img" )) != NULL) {
			DEBUG_PRINTF( "IMG TAG STARTS AT: %.10s\n", ptr_img_start );
			if ((ptr_img_end = strcasestr( ptr_img_start, "/>" )) != NULL) {
				DEBUG_PRINTF( " IMG ENDS AT: %.10s", ptr_img_end );

				ssize_t char_count = ptr_img_end - ptr_img_start;
				// + 1 for null byte at the end
				img_tag_full = xmalloc( sizeof(char) * (char_count + 1));
				strncpy( img_tag_full, ptr_img_start, char_count );
				img_tag_full[char_count] = '\0';

				// Removes newline characters inside <img> tag
				char *nl = NULL;
				while ((nl = strchr( img_tag_full, '\n' )) != NULL) {
					*nl = ' ';
				}
				DEBUG_PRINTF( "  FULL is %ld long\n --- \n%s\n ---",
					char_count, img_tag_full );
				src = regex_extract_src( img_tag_full );
				free( img_tag_full );
			}
		}
	// 	if (feof( infile.fp ) != 0) {
	//		// EOF reached
	//		puts( "EOF reached: bye!" );
	//		return EXIT_SUCCESS;
	// 	}
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

// TODO: What does it do????
void
infile_open( const char *pathname )
{
	if (access( pathname, F_OK ) == -1) {
		err_print ( strerror(errno), pathname ); exit( errno );
	}

	struct open_file_s infile;
	infile.path = xmalloc( sizeof pathname );
	strcpy( infile.path, pathname );

 	infile.fp = fopen( infile.path, "r+" );
	if (! infile.fp) {
		err_print( ERR_MSG_BAD_FILE, infile.path ); exit( errno );
	}
	infile_parse( infile );
	fclose( infile.fp ); // this implicitly flushes the buffer too
}

int
main( int argc, char *argv[] )
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

	int count_infiles = 0;

	if (argc < 2) 	{ err_print( ERR_MSG_NO_INPUT, NULL ); usage(127); }
//	err_print( ERR_MSG_NO_INPUT, "" );
//	err_print( ERR_MSG_BAD_ARG, "--fuck-you" );
//	err_print( ERR_MSG_BAD_FILE, "./myfile.tits" );
	
	// TODO: crash unexpectedly when invalid option string (like "--hekeko")
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
				// TODO: not working?
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
		infile_open( argv[optind] );
		optind++; count_infiles--;
	}
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
}				/* ----------  end of main  ---------- */
