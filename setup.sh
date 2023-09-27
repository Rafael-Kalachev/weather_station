#!/bin/sh


###
# GLOBAL VARIABLES
###

# COMMON VARIABLES


# GIT RELATED VARIABLES

GIT_HOOKS_DIR=".gitconfig/hooks/"
TOOLS_DIR="tools/"

###
# FUNCTIONS
###

# COMMON FUNCTIONS

# GIT RELATED FUNCTIONS

setup_git_config(){
	git config --local include.path ../.gitconfig/git_configuration || return 1
}

prepare_git_hooks(){
	# Make all hooks executable 
	# @note if hooks files are not executable git will ignore them
	${SUDO} chmod +x ${GIT_HOOKS_DIR} -R || return 1
}

verify_dependencies_available(){
	# Verify dependencies
	# @todo Change hardcoded dependency names with dynamic options
	python3 --version > /dev/null || ( echo "ERROR: python3 not available."; return 1 )
	clang-format-15 --version > /dev/null || ( echo "ERROR: clang-format not available."; return 1 )
	arm-none-eabi-gcc --version > /dev/null || ( echo "ERROR: arm-none-eabi-gcc not available."; return 1 )
	make --version > /dev/null || ( echo "ERROR: make not available."; return 1 )
}

prepare_tools(){
	# Make all shell tools executable.
	find "${TOOLS_DIR}" -name '*.sh' | xargs ${SUDO} chmod +x  || return 1 
}

###
# MAIN
###

setup_git_config ||  ( echo "ERROR: Unable to setup git config." ; exit 1 )

prepare_git_hooks || ( echo "ERROR: Unable to prepare git hooks."; exit 1 )

verify_dependencies_available || ( echo "ERROR: Some dependencies are missing/not installed."; exit 1 )

prepare_tools || ( echo "ERROR: Unable to prepare tools."; exit 1)
