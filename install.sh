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

#
# LIBS
#

sudo apt install -y default-libmysqlclient-dev
sudo apt install -y uuid-dev
sudo apt install -y libxml2-dev
sudo apt install -y libcurl4-openssl-dev libssl-dev
sudo apt install -y zlibc libbz2-dev libzip-dev unzip

#
# GNU G++
#

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-11 gcc-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11

#
# VCPKG
#

cd ${root_folder}
if ! [ -d  ${root_folder}/vcpkg/ ]
then
	git clone https://github.com/Microsoft/vcpkg.git
	${root_folder}/vcpkg/bootstrap-vcpkg.sh
fi

${root_folder}/vcpkg/vcpkg install nlohmann-json json-schema-validator fmt vincentlaucsb-csv-parser cpp-jwt

#
# xml2json
#

if ! [ -d  ${root_folder}/vendors/xml2json/ ]
then
	cd ${root_folder}/vendors
	git clone https://github.com/Cheedoong/xml2json
fi

# TODO: переделать на более правильный вариант
sudo ln -fs ${root_folder}/vendors/xml2json /usr/include/xml2json


#
# CONFIGURE
#

cd ${root_folder}/tegia-node
cmake -B build/ -S . -DCMAKE_TOOLCHAIN_FILE=${root_folder}/vcpkg/scripts/buildsystems/vcpkg.cmake

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
# TEGIA USER FOR MySQL
#

mysql_debian_password()
{
    # Получаем пароль от MySQL
    string=$(sudo cat /etc/mysql/debian.cnf)
    regsubstring="password"
    passwd="${string#*password}"
    passwd="${passwd%%socket*}"
    len="$((${#passwd}-4))"
    passwd="${passwd:3:$len}"
    echo $passwd
}

read -rp "${YELLOW}Укажите имя для пользователя, который будет использоваться для подключения к MySQL:${RESET} [tegia_user]: " mysql_user
if [[ -z "$mysql_user" ]]; then
	mysql_user="tegia_user"
fi

while [[ -z "$mysql_password" ]]; do
	read -srp "${YELLOW}Укажите пароль, который будет использоваться для подключения к MySQL:${RESET} " mysql_password
	if [[ -z "$mysql_password" ]]; then
		echo -e "\n${_ERR_}Пароль не может быть пустым"
	else
		break
	fi
done
echo " "

mysql_host='localhost'
mysql_port='3306'

#
# CREATE TEGIA USER
#

export MYSQL_PWD=$(mysql_debian_password)

iffinduser="$(mysql -u debian-sys-maint --execute="SELECT host,user FROM mysql.user WHERE host = '$mysql_host' AND user = '$mysql_user';")"
if [[ "${#iffinduser}" != 0 ]]; then

	#
	# Удалить пользователя
	#
	
	mysql -u debian-sys-maint --port=$mysql_port --execute="DROP USER '$mysql_user'@'$mysql_host';"
fi

mysql -u debian-sys-maint --port=$mysql_port << EOF
CREATE USER '$mysql_user'@'$mysql_host' IDENTIFIED BY '$mysql_password';
GRANT ALL PRIVILEGES ON *.* TO '$mysql_user'@'$mysql_host';
FLUSH PRIVILEGES;
EOF

echo -e "${_OK_}tegia user '${mysql_user}' is created on MySQL"

#
# SAVE tegia.cnf FILE
#

tee ${root_folder}/tegia.cnf << EOF > /dev/null
[mysql]
host=$mysql_host
port=$mysql_port
user=$mysql_user
password=$mysql_password
EOF

echo "${_OK_}file '${root_folder}/tegia.cnf' is saved"

#
# Makefile.variable
#

tee ${root_folder}/Makefile.variable << EOF > /dev/null
ProdFlag			= -rdynamic -I${root_folder}/tegia-node/include -std=c++2a -march=native -m64 -O2
DevFlag				= -rdynamic -I${root_folder}/tegia-node/include -std=c++2a -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
Flag = \$(DevFlag)
EOF


echo " "
echo "------------------------------------------------------------"
echo "TEGIA NODE: ${GREEN} INSTALL ${RESET}"
echo "------------------------------------------------------------"
echo " "

exit 0

# ----------------------------------------------------------------------------------------
#
# Настраиваем подключение к MySQL
#
# ----------------------------------------------------------------------------------------







# ----------------------------------------------------------------------------------------
#
# Файл с флагами компиляции
#
# ----------------------------------------------------------------------------------------



exit 0





