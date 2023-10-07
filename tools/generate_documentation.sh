#!/bin/bash

###
# GLOBAL CONSTANTS
###

SCRIPT_NAME='generate_documentation.sh'
DOXYGEN='doxygen'

###
# DEFAULT OPTIONS VALUES
###

###
# FUNCTIONS
###

print_help(){
	echo "USAGE:"
	echo "    ${SCRIPT_NAME} [-e <env_file>] <Doxyfile>"
	echo "SYNOPSIS:"
	echo "    This script is a thin wrapper around ${DOXYGEN}."
	echo "OPTIONS:"
	echo "    <Doxyfile>   ${DOXYGEN} configuration file."
	echo "    -e|--env     File Containing Environment to be passed to Doxygen."
#	echo "    -c|--config  File Containing additional Doxygen Configuration."
	echo "    -h|--help    Print this help message."
	echo "RESULT:"
	echo "     0    OK"
	echo "    !0    Doccumentation not properly generated."
}

###
# ARGUMENT HANDLING
###

while [ $# -gt 0 ];
do
	case $1 in
		-e|--env)
			ENV_FILE=$2
			shift 2
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
			DOXYFILE="$1"
			shift 1
			;;

	esac
done

# No positional arguments
#set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters

# Verify arguments.

if [ ! -f "${DOXYFILE}" ]
then
	echo "The provided Doxyfile '${DOXYFILE}' must be and existing file containing valid doxygen configuration."
fi
c
# Add Variables from ENV_FILE to environment
if [ -f "${ENV_FILE}" ]
then
	set -a
	source "${ENV_FILE}"
	set +a
fi


###
# MAIN
###

"${DOXYGEN}" "${DOXYFILE}"
exit $?