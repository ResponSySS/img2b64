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

#ifndef _IMG2B64_H
#define _IMG2B64_H


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

#include "utils.h"
#include "regexp.h"


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
#define SIZE_FREAD_BUFF 	8192

// OPTIONS
//TODO: needs a common struct for all options.
//TODO: don't define here!!! FAG!
#define SHORTOPTS 		"i::nh"
bool quiet 		= false;
bool in_place 		= false;
char *in_place_suffix 	= NULL;

struct open_file_s {
	FILE *fp;
	char *path;
};

int
infile_parse( struct open_file_s infile );

void
infile_open( const char *pathname );


#endif
