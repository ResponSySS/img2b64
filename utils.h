/*
 * =====================================================================================
 *    Description:  Misc functions
 *
 *        Created:  03/17/2018 05:24:27 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@systemicresponse.com
 * =====================================================================================
 */

#ifndef _UTILS_H
#define _UTILS_H

void
err_print( const char *err_str, const char *opt );

void
xmalloc_failed( size_t size );

void *
xmalloc( size_t size );

void
version_print( void );

void
usage( int status );

void
opt_print( const int argc, char *argv[] );

#endif
