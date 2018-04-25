/*
 * =====================================================================================
 *    Description:  
 *
 *        Created:  12/06/2017 09:30:51 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#ifndef _IMG2B64_H
#define _IMG2B64_H

#include "utils.h"
#include "regexp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>


// DEBUG
#ifndef DEBUG
#define DEBUG 0
#endif

#define dprintf(...) \
    do { if (DEBUG) fprintf( stderr, "[DEBUG] " __VA_ARGS__ ); } while (0)

// GLOBALS
#define PROGRAM_NAME        "img2b64"
#define VERSION             "0.8"

// ERRORS
// Sometimes require another argument for clarity (ones with 'y')
#define ERR_MSG_NO_INPUT 	"no input file specified" 	
#define ERR_MSG_BAD_ARG 	"unknown argument" 		// y
#define ERR_MSG_NO_FILE 	"file does not exist" 	// y
#define ERR_MSG_BAD_FILE 	"can't open file" 		// y

#define SIZE_FREAD_BUFF 	8192

// OPTIONS
// TODO: needs a common struct for all options.
#define SHORTOPTS 		"i::nh"
extern bool quiet;
extern bool in_place;
extern char *in_place_suffix;

struct open_file_s {
	FILE *fp;
	char *path;
};

int
infile_parse( struct open_file_s infile );

void
infile_open( const char *pathname );


#endif
