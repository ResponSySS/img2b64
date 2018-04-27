/*
 * =====================================================================================
 *        Created:  04/27/2018 09:52:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#ifndef _ENCODE_H
#define _ENCODE_H


#include <stdio.h>
#include <stdlib.h>

char *
b64_process_resource( const char *path );

int
b64_encode( char *dest, FILE *fp );


#endif
