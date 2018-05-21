/*
 * =====================================================================================
 *        Created:  04/27/2018 09:52:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

#include "encode.h"
#include "img2b64.h"
#include "utils.h"

#include "inc/cencode.h"
#include "inc/cdecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


// destination buffer large enough to hold the encoded data
static char b64_string_encoded [SIZE_FREAD_BUFF * 2];


// Process file from path and encode to b64 string
char *
b64_process_file( const char *path )
{
    assert( path );

    FILE *fp;
    // Open resource to encode
	if ((fp = fopen( path, "r" )) == NULL) {
		//err_print( ERR_ERR "%s: %s", ERR_MSG_BAD_FILE, path );
		err_print( ERR_ERR "Can't process file: %s: '%s'", strerror(errno), path );
        exit( errno );
    }
    // Encode file
    if (! b64_encode( b64_string_encoded, fp )) {
		err_print( ERR_ERR "Can't encode file to base64 string: '%s'", path );
        exit( errno );
    }
    // Close file
	if (fclose( fp ) != 0) { // this implicitly flushes the buffer too
		err_print( ERR_ERR "Can't close processed file: %s: '%s'", strerror(errno), path );
        exit( errno );
    }
    return b64_string_encoded;
}

// Encode file to b64 string
int
b64_encode( char *dest, FILE *fp )
{
    assert( dest );

	char input [SIZE_FREAD_BUFF];
	base64_encodestate es;
	int cnt = 0; // number of bytes encoded by a single call

	base64_init_encodestate(&es); // initialise the encoder state
	// gather data from the input and send it to the output
	while (1)
	{
		cnt = fread(input, 1, SIZE_FREAD_BUFF, fp);
		if (cnt == 0) break;
		cnt = base64_encode_block(input, cnt, dest, &es);
		/* output the encoded bytes to the output file */
        //memcpy( dest, input, cnt );
	}
	/* since we have reached the end of the input file, we know that 
	   there is no more input data; finalise the encoding */
	cnt = base64_encode_blockend(dest, &es);
	/* write the last bytes to the output file */
    //memcpy( dest, input, cnt );
	/*---------- STOP ENCODING  ----------*/
    return 1;
}
