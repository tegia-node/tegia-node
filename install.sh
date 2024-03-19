#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
BLUE=`tput setaf 4`
RESET=`tput sgr0`


_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"


root_folder=$(realpath ../)


echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} DEPENDENCES ${RESET}"
echo "------------------------------------------------------------"
echo " "

mkdir -p ${root_folder}/tegia-node/build
mkdir -p ${root_folder}/vendors
mkdir -p ${root_folder}/configurations
mkdir -p ${root_folder}/ui


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

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-11 gcc-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11


#
# MySQL
#

ismysql80="$(dpkg --get-selections | grep mysql-server-8.0)"
if [[ "${#ismysql80}" == 0 ]]; then

	mkdir -p $tegia_folder/vendors/mysql
	cd $tegia_folder/vendors/mysql
	wget -N https://dev.mysql.com/get/mysql-apt-config_0.8.15-1_all.deb
	sudo DEBIAN_FRONTEND=noninteractive dpkg -i mysql-apt-config_0.8.15-1_all.deb    

	sudo apt-get update -y
	sudo DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-server
	sudo DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-client

	echo -e "${_OK_}MySQL success installed"
else
	echo -e "${_OK_}MySQL is already installed"
fi

#
# VCPKG
#

# ###############
# cd ${root_folder}
# if ! [ -d  ${root_folder}/vcpkg/ ]
# then
#	git clone https://github.com/Microsoft/vcpkg.git
#	${root_folder}/vcpkg/bootstrap-vcpkg.sh
# fi

# ${root_folder}/vcpkg/vcpkg install nlohmann-json json-schema-validator fmt vincentlaucsb-csv-parser cpp-jwt
# ###############

#
# nlohmann json
#

if ! [ -d  ${root_folder}/vendors/json/ ]
then
	cd ${root_folder}/vendors
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

if ! [ -d  ${root_folder}/vendors/json-schema-validator/ ]
then
	cd ${root_folder}/vendors
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

if ! [ -d  ${root_folder}/vendors/csv-parser/ ]
then
	cd ${root_folder}/vendors
	git clone https://github.com/vincentlaucsb/csv-parser.git
fi

#
# xml2json
#

if ! [ -d  ${root_folder}/vendors/xml2json/ ]
then
	cd ${root_folder}/vendors
	git clone https://github.com/Cheedoong/xml2json
	# TODO: переделать на более правильный вариант
	sudo ln -fs ${root_folder}/vendors/xml2json /usr/include/xml2json
fi

#
# jwt
#

if ! [ -d  ${root_folder}/vendors/cpp-jwt/ ]
then
	cd ${root_folder}/vendors
	git clone https://github.com/arun11299/cpp-jwt.git
	cd cpp-jwt
	mkdir -p build
	cd build
	cmake ..
	cmake --build . -j
	sudo make install
fi

#
# CONFIGURE
#

cd ${root_folder}/tegia-node
# cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=${root_folder}/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake -B build/ -S .

echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} BUILD ${RESET}"
echo "------------------------------------------------------------"
echo " "

cd ${root_folder}/tegia-node/build
cmake --build .

echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} INSTALL ${RESET}"
echo "------------------------------------------------------------"
echo " "

#
# SAVE 'Makefile.variable' FILE
#

tee ${root_folder}/Makefile.variable << EOF > /dev/null
ProdFlag			= -rdynamic -I${root_folder}/tegia-node/include -I${root_folder}/vendors -std=c++2a -march=native -m64 -O2
DevFlag				= -rdynamic -I${root_folder}/tegia-node/include -I${root_folder}/vendors -std=c++2a -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
Flag = \$(DevFlag)
EOF


echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} INSTALLATION COMPLETED ${RESET}"
echo "------------------------------------------------------------"
echo " "

exit 0

