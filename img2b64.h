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
#ifdef NDEBUG

#define DEBUG 0
#define DEBUG_PRINTF(...)   ((void)0)
#define DEBUG_WHERE()       ((void)0)
#define DEBUG_EXIT()        ((void)0)

#else // ! NDEBUG

#define DEBUG_PRINTF(...) \
    do {    fprintf( stderr, "[DEBUG] " __VA_ARGS__ );                          } while (0)
#define DEBUG_WHERE() \
    do {    fprintf( stderr, "[DEBUG] %s:%d:%s", __FILE__, __LINE__, __func__ );} while (0)
#define DEBUG_EXIT()   \
    do {    fprintf( stderr, "[DEBUG] exiting now\n" ); exit(1);                } while (0)

#endif

// GLOBALS
#define PROGRAM_NAME        "img2b64"
#define VERSION             "0.05.15" // change it to 2018.05.XX when ready
#define PRINT_PREFIX        PROGRAM_NAME ": "

// ERRORS
// Sometimes require another argument for clarity (ones with 'y')
#define ERR_MSG_NO_INPUT 	"no input file specified" 	
#define ERR_MSG_BAD_ARG 	"unknown argument" 		// y
#define ERR_MSG_NO_FILE 	"file does not exist" 	// y
#define ERR_MSG_BAD_FILE 	"can't open file" 		// y
#define ERR_MSG_FILE_STREAM "error processing file stream" // y

#define SIZE_FREAD_BUFF 	8192 // exactly 8 KiB (taken from sed 'execute.c')

// OPTIONS
#define SHORTOPTS 		"i::nh"

enum {
    OPT_QUIET           = 0x1,
    OPT_INPLACE         = 0x2
};
struct Opt {
    unsigned char opt;
    char *in_place_suffix;
};
extern struct Opt opt;

// FILES
struct Open_file {
	FILE *fp;
	char *path;
};

#define OUTFILE_PATH        "/tmp/img2b64.out"
#define OUTFILE_STDOUT      "/dev/stdout"


#endif
