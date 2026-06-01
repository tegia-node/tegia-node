#!/bin/bash
set -euo pipefail

RED=`tput setaf 1 2>/dev/null || true`
GREEN=`tput setaf 2 2>/dev/null || true`
YELLOW=`tput setaf 3 2>/dev/null || true`
BLUE=`tput setaf 4 2>/dev/null || true`
RESET=`tput sgr0 2>/dev/null || true`


_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"

ROOT=$(realpath ../)

usage()
{
	cat <<USAGE
Usage:
  bash install.sh [--build-only|--skip-system-deps]

Host-level dependencies are installed by .deploy/init-host.sh.
This script builds tegia-node and regenerates Makefile.variable only.
USAGE
}

for arg in "$@"; do
	case "${arg}" in
		--build-only|--skip-system-deps)
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "${_ERR_}unknown argument: ${arg}" >&2
			usage >&2
			exit 2
			;;
	esac
done

check_cmd()
{
	local cmd=$1
	command -v "${cmd}" >/dev/null 2>&1 || {
		echo "${_ERR_}required command not found: ${cmd}. Run: bash .deploy/init-host.sh --install" >&2
		exit 5
	}
}

check_header()
{
	local path=$1
	if ! [ -e "${path}" ]; then
		echo "${_ERR_}required header/path not found: ${path}. Run: bash .deploy/init-host.sh --install" >&2
		exit 5
	fi
}

echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} PREFLIGHT ${RESET}"
echo "------------------------------------------------------------"
echo " "

mkdir -p ${ROOT}/tegia-node/build
mkdir -p ${ROOT}/configurations
mkdir -p ${ROOT}/ui

check_cmd cmake
check_cmd make
check_cmd mysql
check_cmd searchd
check_cmd flyway
check_header /usr/include/mysql/mysql.h
check_header /usr/include/libxml2
check_header /usr/include/uuid/uuid.h
check_header /usr/include/xml2json
check_header /usr/local/include/csv.hpp
check_header /usr/local/src/tegia-vendors/date/include/date/date.h

#
# CONFIGURE
#

cd ${ROOT}/tegia-node
# cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=${root_folder}/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake -B build/ -S .

echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} BUILD ${RESET}"
echo "------------------------------------------------------------"
echo " "

#
# SAVE 'Makefile.variable' FILE
#

tee ${ROOT}/Makefile.variable << EOF > /dev/null
iNODE				= ${ROOT}/tegia-node/include
iVENDORS			= /usr/local/src/tegia-vendors
iVENDORSINCLUDE     = /usr/local/include /usr/include /usr/include/libxml2 /usr/local/src/tegia-vendors /usr/local/src/tegia-vendors/*/include /usr/local/src/tegia-vendors/*/single_include
C++VER				= -std=c++2a

ProdFlag			= -rdynamic -I\$(iNODE) -I\$(iVENDORS) \$(addprefix -I,\$(iVENDORSINCLUDE)) \$(C++VER) -march=native -m64 -O2
DevFlag				= -rdynamic -I\$(iNODE) -I\$(iVENDORS) \$(addprefix -I,\$(iVENDORSINCLUDE)) \$(C++VER) -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
Flag = \$(DevFlag)
EOF


cd ${ROOT}/tegia-node/build
cmake --build .


echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} INSTALLATION COMPLETED ${RESET}"
echo "------------------------------------------------------------"
echo " "

exit 0
