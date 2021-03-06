/*
 * =====================================================================================
 *    Description:  String processing
 *
 *        Created:  03/17/2018 05:15:05 PM
 *       Compiler:  gcc
 *
 *         Author:  Sylvain S. (ResponSyS), mail@sylsau.com
 * =====================================================================================
 */

//#define _GNU_SOURCE // needed by strcasestr

#include "parse.h"
#include "img2b64.h"
#include "encode.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <errno.h>
#include <assert.h>

static unsigned g_process_buf_status = PB_NO_MATCH;
static char *g_img_off[2] = { NULL, NULL }; // "<img>" tag start offset [0], end offset [1]

// Removes newline characters inside string
// Returns number of newlines removed
static int
strip_nl( char * str )
{
    assert( str );
    int i;
    char *nl;
    for (i = 0, nl = NULL ; (nl = strchr( str, '\n' )) != NULL ; *nl=' ', i++)
        ;
    return i;
}

// Locate a substring 
// @here        pointer to fill with beginning of located substring
// @haystack    string to search
// @needle      substring to find
// Returns 1 if match, 0 if no match (set here to NULL), -1 if incomplete match
static int
strinstr( char **here, char *haystack, const char needle[] )
{
    assert( here && haystack && needle );
    *here = NULL;
    int i = 0;
    char *pos = haystack;
    
    for (; *pos != '\0' ; pos++) {
        for (; needle [i] != '\0' ; i++) {
            if (*(pos + i) == tolower( needle[i] ) || *(pos + i) == toupper( needle[i] )) {
                if (needle[i+1] == '\0') {
                    // Match
                    *here = pos;
                    return 1;
                }
            } else if (*(pos + i) == '\0') {
                // Incomplete match
                *here = pos;
                return -1;
            } else
                break;
        }
    }
    // No match
    return 0;
}

// Extract substring from string with start and end offsets
// @str     string to fill
// @off     array of start offset ([0]) and end offset ([1])
// @maxlen  max length of string to fill
// Returns 1 on success, 0 otherwise
static int
extract_substr( char str[], const char *off[2], size_t maxlen )
{
    assert( str && off );
    size_t len = off[1] - off[0] + 1; // +1 for terminating null byte
    if (len > maxlen) {
        DEBUG_WHERE();
        DEBUG_PRINTF( "substring {%10s} too big to be extracted from {%s}", off[0], str );
        return 0;
    }
    strncpy( str, off[0], len - 1 );
    str[len-1] = '\0';
    return 1;
}

// Execute regex against string
// @str         string to match against
// @patt        regex pattern
// @pm          match structure to fill
// @nmatch      number of submatches in pattern
// Returns 1 upon success, 0 otherwise
static int
match_regex( const char *str, const char *patt, regmatch_t pm[], const size_t nmatch )
{
    int n = 0; // ret value of regex functions
	regex_t preg;
	char err_buf [SIZE_ERR_BUFF];

	// Compile RE pattern
	if ((n = regcomp( &preg, patt, REG_ICASE | REG_EXTENDED ))) {
		regerror( n, &preg, err_buf, SIZE_ERR_BUFF );
		err_print( ERR_WARN "Can't compile regex pattern on {%.20s...}: %s", str, err_buf );
		return 0;
	}
    // Find RE pattern
	if ((n = regexec( &preg, str, nmatch, pm, 0x0 ))) {
		regerror( n, &preg, err_buf, SIZE_ERR_BUFF );
		err_print( ERR_WARN "Can't execute regex pattern on {%.20s...}: %s", str, err_buf );
		return 0;
	}
	// Printing matches
	if (DEBUG) {
		int i;
		for ( i=0 ; i < nmatch ; i++) {
            DEBUG_PRINTF( "    match %d [%d-%d]: {%.*s}\n", i ? 1 : 0,
                    pm[i].rm_so, pm[i].rm_eo,
                (int) pm[i].rm_eo - pm[i].rm_so, str + pm[i].rm_so );
		}
	}
    return 1;
}

// Check if src matches base64 data format string
// @src     string to check
// Returns 1 on success, 0 otherwise
static int
is_base64_src( const char *src )
{
    assert( src );
    regmatch_t pm[2];
    DEBUG_PRINTF( "Regexing base64 'src'\n" );
    return match_regex( src, RE_PATTERN_B64_SRC, pm, 1 );
}


// Extract 'src' parameter from <img> tag in string
// @src     string to fill
// @img     <img> string to extract from
// @maxlen  max length of string to fill
// Returns 1 on success, 0 otherwise
static int
extract_src( char src[], const char *img, size_t maxlen )
{
    assert( src && img );
    DEBUG_PRINTF( "Regexing 'src'\n" );
	regmatch_t pm[2]; // [0] matchs the whole regex, [1] is first match, etc.
    if (! match_regex( img, RE_PATTERN_SRC, pm, 2 ))
        return 0;

    size_t len = pm[1].rm_eo - pm[1].rm_so + 1; // + 1 for null byte
    strncpy( src, img + pm[1].rm_so, len - 1 );
    src[len-1] = '\0';
	return 1;
}

// TODO: divide this in tinier function
// TODO:    1) what if the buffer contains the start of <img> but not its end?
//          2) what if it ends like "[...] <im"
//          solutions:
//              1) if end of <img> not found, refetch from start of <img>
//              2) always refetch next buffer 3 characters before end of previous buffer
//                 OR: check for '<' (then 'i' then 'm' etc.) in last 3 chars of buffer THEN refetch
// Search and replace <img> tag with base64
// Returns number of replacements made
// @buf         string to parse
static int
process_buf( char * buf )
{
    assert( buf );
    int reti; // Number of matches
    int n;

    // Find start offset of <img> in str
	for (reti = 0; (n = strinstr( & g_img_off[0], buf, "<img" )) != 0 ;
            buf = ++(g_img_off[0]), reti++) {
        if (n == -1) {
            // Incomplete match
            g_process_buf_status = PB_INCOMPLETE_MATCH_START;
            // re-fetch stream from offset position
            //int diff = img_off[0] - buf;
            err_print( ERR_WARN "Incomplete <img> tag" );
            return reti;
        }
        // Find end offset
	    if ( (n = strinstr( & g_img_off[1], g_img_off[0], "/>" )) != 0 ) {
            if (n == -1) {
                // Incomplete match
                g_process_buf_status = PB_INCOMPLETE_MATCH_END;
                // re-fetch stream from offset position
                //int diff = img_off[1] - buf;
                err_print( ERR_WARN "Incomplete <img> tag" );
                return reti;
            }
            DEBUG_PRINTF( "<img> #%d:\n", reti );
            char img_full [SIZE_FREAD_BUFF];
            char src [SIZE_FREAD_BUFF];
            char * b64_str;
            // Extract <img> tag from whole string
            extract_substr( img_full, (const char **)g_img_off, SIZE_FREAD_BUFF );
            // Strip newlines
            int j = strip_nl( img_full );
            DEBUG_PRINTF( "  %d newlines stripped\n", j );
            // Extract 'src' parameter and process file
	    	extract_src( src, img_full, SIZE_FREAD_BUFF );
            // Check if already a base64 'src'
            strip_nl( src );
            if (is_base64_src( src )) {
                DEBUG_PRINTF( "base64 'src' detected; skipping\n" );
                continue;
            }
            b64_str = b64_process_file( src );
            DEBUG_PRINTF( "base64 result: %.50s [...]\n", b64_str );
            //TODO: Change original <img> tag
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
	return reti;
}

// Parse a file
// Returns number of replacement made in file
// @inf     input file
// @outf    output
int
parse_file_print( const struct Open_file inf, struct Open_file outf )
{
    int reti = 0;
    size_t read_size = 0;
    char * buf = xmalloc( SIZE_FREAD_BUFF + 1);

    DEBUG_PRINTF( "Parsing '%s', printing to '%s'\n", inf.path, outf.path );
    // get next string from file
    while ( (read_size = fread( buf, 1, SIZE_FREAD_BUFF, inf.fp )) != 0 ) {
        buf [SIZE_FREAD_BUFF] = '\0';
        // parse string
        reti += process_buf( buf );
        if (g_process_buf_status == PB_INCOMPLETE_MATCH_START) {
            // <img> tag was incomplete, so refetch from start of g_img_offset[0] ("<img")
        } else if (g_process_buf_status == PB_INCOMPLETE_MATCH_END) {
            // <img> tag was incomplete, so refetch from start of g_img_offset[1] ("/>")
        }
        fseek( inf.fp, -3, SEEK_END ); // rewind last 3 chars in case  "<im" at the end // TODO: ERROR INFINITE LOOP HERE
        // TODO: REMAKE STRCASESTR? CHECK FOR '<' IN LAST 3 CHARS OF BUFFER INSTEAD OF REWINDING EACH TIME???
        fwrite( buf, 1, read_size - 3, outf.fp ); // print to outfile (minus rewinded chars)
    }
    fprintf( stderr, "\n" );
    // EOF or error?
    if (feof( inf.fp )) {
        fprintf( stderr, PRINT_PREFIX "%s: made %d replacements\n", inf.path, reti );
    } else if (ferror( inf.fp )) {
        err_print( ERR_ERR ERR_MSG_FILE_STREAM );
        exit( EBADF );
    }
    free( buf );
    return reti;
}
