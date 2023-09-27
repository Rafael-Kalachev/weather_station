#!/bin/bash

###
# GLOBAL CONSTANTS
###

SCRIPT_NAME='format_code.sh'
CLANG_FORMAT='clang-format-15'

###
# DEFAULT OPTIONS VALUES
###

POSITIONAL_ARGS=()

###
# FUNCTIONS
###

print_help(){
	echo "USAGE:"
	echo "    ${SCRIPT_NAME} --check <files>"
	echo "    ${SCRIPT_NAME} --fix <dirs|files>"
	echo "SYNOPSIS:"
	echo "    This script is a thin wrapper around ${CLANG_FORMAT}."
	echo "OPTIONS:"
	echo "    <files>      One or more files that will be processed by ${CLANG_FORMAT}."
	echo "    --check      Check the provided files."
	echo "    --fix        Fix the provided wiles."
	echo "    -h|--help    Print this help message."
	echo "RESULT:"
	echo "     0    OK"
	echo "    !0    Error or files are not formatted properly."
}

###
# ARGUMENT HANDLING
###

while [ $# -gt 0 ];
do
	case $1 in
		--check)
			OPTIONS='--dry-run -Werror'
			shift 1
			;;
		--fix)
			OPTIONS='-i'
			shift 1
			;;
		-h|--help)
			print_help
			exit 0
			;;
		-*)
			echo "Unknown option $1"
			echo "See help:"
			print_help
			exit 1
			;;
		*)
			if [ -f "${1}" ]
			then
				POSITIONAL_ARGS+=("$1") # save positional arg
			else
				echo "The provided '${1}' is not an existing file."
				exit 1
			fi
			shift 1
			;;

	esac
done

# No positional arguments
set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

# Verify that OPTIONS is set.

if [ -z "${OPTIONS}" ]
then
	echo "It is required to provide either --check or --fix."
	echo "See help."
	print_help
	exit 1 
fi


###
# MAIN
###

${CLANG_FORMAT} ${OPTIONS} "$@"
exit $?