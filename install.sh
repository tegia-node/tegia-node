#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
BLUE=`tput setaf 4`
RESET=`tput sgr0`


_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"

ROOT=$(realpath ../)

echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} DEPENDENCES ${RESET}"
echo "------------------------------------------------------------"
echo " "

mkdir -p ${ROOT}/tegia-node/build
mkdir -p ${ROOT}/vendors
mkdir -p ${ROOT}/configurations
mkdir -p ${ROOT}/ui

#
# ENV
#

sudo apt install -y mc
sudo apt install -y screen
sudo apt install -y jq
sudo apt install -y zip
sudo apt install -y python
sudo apt install -y build-essential
sudo apt install -y libtool
sudo apt install -y m4
sudo apt install -y automake
sudo apt install -y pkg-config
sudo apt install -y cmake
sudo apt install -y libgtest-dev

#
# LIBS
#
sudo apt install -y default-libmysqlclient-dev
sudo apt install -y uuid-dev
sudo apt install -y libxml2-dev
sudo apt install -y libcurl4-openssl-dev libssl-dev
sudo apt install -y zlibc libbz2-dev libzip-dev unzip
sudo apt install -y libfmt-dev

#
# GNU G++
#

# sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
# sudo apt install -y g++-11 gcc-11
# sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11

#
# MySQL
#

version=$(lsb_release -r | awk '{print $2}')

# Проверяем версию ОС и выполняем соответствующие действия
if [[ "$version" == "20.04" ]]
then
    echo "Версия ОС: Ubuntu 20.04"
	ismysql80="$(dpkg --get-selections | grep mysql-server-8.0)"
	if [[ "${#ismysql80}" == 0 ]]; then

		mkdir -p $ROOT/vendors/mysql
		cd $ROOT/vendors/mysql
		wget -N https://dev.mysql.com/get/mysql-apt-config_0.8.15-1_all.deb
		sudo DEBIAN_FRONTEND=noninteractive dpkg -i mysql-apt-config_0.8.15-1_all.deb    

		sudo apt-get update -y
		sudo DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-server
		sudo DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-client

		echo -e "${_OK_}MySQL success installed"
	else
		echo -e "${_OK_}MySQL is already installed"
	fi
elif [[ "$version" == "24.04" ]]
then
    echo "Версия ОС: Ubuntu 24.04"
    sudo apt install -y mysql-server
else
    echo "Версия ОС: другая версия (или неизвестная)"
    # Ваши действия для других версий
fi

#
# Manticore
#

wget https://repo.manticoresearch.com/manticore-repo.noarch.deb
sudo dpkg -i manticore-repo.noarch.deb
sudo apt update
sudo apt install manticore manticore-extra -y

#
# nlohmann json
#

if ! [ -d  ${ROOT}/vendors/json/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/nlohmann/json.git
	cd json
	mkdir -p build
	cd build
	cmake ..
	make
	sudo make install
	sudo ldconfig
fi

#
# json-schema-validator
#

if ! [ -d  ${ROOT}/vendors/json-schema-validator/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/pboettch/json-schema-validator.git
	cd json-schema-validator
	mkdir -p build
	cd build
	cmake .. -DBUILD_SHARED_LIBS=ON ..
	make
	sudo make install
	sudo ldconfig
fi

#
# vincentlaucsb / csv-parser
#

if ! [ -d  ${ROOT}/vendors/csv-parser/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/vincentlaucsb/csv-parser.git
fi

#
# xml2json
#

if ! [ -d  ${ROOT}/vendors/xml2json/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/Cheedoong/xml2json
	# TODO: переделать на более правильный вариант
	sudo ln -fs ${ROOT}/vendors/xml2json /usr/include/xml2json
fi

#
# jwt
#

if ! [ -d  ${ROOT}/vendors/cpp-jwt/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/arun11299/cpp-jwt.git
	cd cpp-jwt
	mkdir -p build
	cd build
	cmake ..
	cmake --build . -j
	sudo make install
fi

#
# inja
#

if ! [ -d  ${ROOT}/vendors/inja/ ]
then
	cd ${ROOT}/vendors
	git clone https://github.com/pantor/inja.git --depth=1 --branch=v3.4.0
fi

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
iVENDORS			= ${ROOT}/vendors
C++VER				= -std=c++2a

ProdFlag			= -rdynamic -I\$(iNODE) -I\$(iVENDORS) \$(C++VER) -march=native -m64 -O2
DevFlag				= -rdynamic -I\$(iNODE) -I\$(iVENDORS) \$(C++VER) -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
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

