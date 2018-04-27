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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>


// DEBUG
#ifndef DEBUG
#define DEBUG 0
#endif

// TODO: invalid memory read caused by this (probably bad handling of VA_ARGS
#define DEBUG_PRINTF(...) \
    do { if (DEBUG)     fprintf( stderr, "[DEBUG] " __VA_ARGS__ ); } while (0)
#define DEBUG_WHERE() \
    do { if (DEBUG)     fprintf( stderr, "[DEBUG] %s:%s:%s", __file__, __line__, __func__ ); } while (0)

// GLOBALS
#define PROGRAM_NAME        "img2b64"
#define VERSION             "0.8"

// ERRORS
// Sometimes require another argument for clarity (ones with 'y')
#define ERR_MSG_NO_INPUT 	"no input file specified" 	
#define ERR_MSG_BAD_ARG 	"unknown argument" 		// y
#define ERR_MSG_NO_FILE 	"file does not exist" 	// y
#define ERR_MSG_BAD_FILE 	"can't open file" 		// y
#define ERR_MSG_FILE_STREAM "error processing file stream" // y

#define SIZE_FREAD_BUFF 	8192 // arbitrary (?) value taken from sed 'execute.c'

// OPTIONS
// TODO: needs a common struct for all options.
#define SHORTOPTS 		"i::nh"

#define OPT_QUIET       1
#define OPT_INPLACE     2
struct opt_s {
    unsigned char opt;
    char *in_place_suffix;
};
extern struct opt_s opt;
#define OPT_S_DEF           { 00, NULL }

// FILES
struct open_file_s {
	FILE *fp;
	char *path;
};
#define OPEN_FILE_S_DEF     { NULL, NULL }

#define OUTFILE_PATH        "/tmp/img2b64.out"


#endif
