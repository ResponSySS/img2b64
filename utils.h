/*
 * =====================================================================================
 *    Description:  Misc functions
 *
 *        Created:  03/17/2018 05:24:27 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>

#define XMALLOC_ERR_STR 	"out of memory allocating %lu bytes\n"

void
err_print( const char *fmt, ... );

void *
xmalloc( size_t size );

void
print_version( void );

void
usage( const int status );

void
opt_print( const int argc, char *argv[] );

#endif
