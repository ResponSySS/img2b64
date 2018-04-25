#!/bin/bash - 
#===============================================================================
#
#         USAGE: ./img2b64.sh --help
# 
#   DESCRIPTION: base64, sed, sort, uniq
#  REQUIREMENTS: ---
#        AUTHOR: Sylvain S. (ResponSyS), mail@sylsau.com
#  ORGANIZATION: 
#       CREATED: 11/28/2017 16:42
#===============================================================================
# Enables strict mode
[[ "$DEBUG" ]] && set -o nounset -o errexit -o pipefail

TEMP_B64_FILE="img2b64-$$.b64"
trap 'rm -f --verbose "$TEMP_B64_FILE"' EXIT
# TODO: to make it whitespace-safe use:
#IFS=$'\n'

F_IN="$1"
pushd "$(dirname "$F_IN")" # || err
F_IN="$(basename "$F_IN")" # || err
echo "IN = $F_IN"

# get list of all img src values
SRC_LIST="$(sed -n "s/^.*<img\s*src=['\"]\([^\"']\+\)['\"].*$/\1/p" $F_IN | sort | uniq)"

echo "Source file(s): '$SRC_LIST'"
for IMG_PATH in $SRC_LIST ; do
	local PLACEHOLDER=">>>${IMG_PATH}<<<"
	base64 -w0 < "$IMG_PATH" > "$TEMP_B64_FILE"
	head -c20 "$TEMP_B64_FILE"
	echo
	# can't use '/' as delimiter (troubles with /file/path) so using ',' instead
	# SED: ARGUMENTS TOO LONG
	# TODO: ONLY ONE WAY:
	# 	MAKE A NEWLINE WITH A PLACEHOLDER
	# 	REPLACE THE PLACEHOLDER WITH ED(IT)

	sed -n "s,\(<img\s*src=['\"]\)${IMG_PATH}\(['\"]\),\1\n${PLACEHOLDER}\n\2,p" $F_IN
	#printf '%s\n' "s,\(<img\s*src=['\"]\)${IMG_PATH}\(['\"]\),\1$(base64 -w0 "$IMG_PATH")\2,p"  $F_IN
	edit $F_IN <<EOF
/${PLACEHOLDER}/r $TEMP_B64_FILE
1
/${PLACEHOLDER}/d
w
EOF
done
