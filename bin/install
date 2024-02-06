#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
BLUE=`tput setaf 4`
RESET=`tput sgr0`


_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"


tegia_folder=$(realpath ../../)


echo "";
echo "${GREEN}Установка tegia-node${RESET}"
echo "";


# ----------------------------------------------------------------------------------------
#
# Настраиваем подключение к MySQL
#
# ----------------------------------------------------------------------------------------



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


mysql_install()
{
	echo "MYSQL INSTALL"

	#
	# MYSQL CONNECTION
	#

	#echo "Укажите mysql host"
	read -rp "${YELLOW}Укажите mysql host:${RESET} [localhost]: " mysql_host
	if [[ -z "$mysql_host" ]]; then
		mysql_host="localhost"
	fi

	#echo "Укажите mysql port"
	read -rp "${YELLOW}Укажите mysql port:${RESET} [3306]: " mysql_port
	if [[ -z "$mysql_port" ]]; then
		mysql_port='3306'
	fi

	#echo "Укажите mysql user"
	read -rp "${YELLOW}Укажите mysql user:${RESET} [tegia_user]: " mysql_user
	if [[ -z "$mysql_user" ]]; then
		mysql_user="tegia_user"
	fi

	while [[ -z "$mysql_password" ]]; do
		read -srp "${YELLOW}Укажите пароль для подключения к MySQL:${RESET} " mysql_password
		if [[ -z "$mysql_password" ]]; then
			echo -e "\n${_ERR_}Пароль не может быть пустым"
		else
			break
		fi
	done
	echo " "

    #
    # install mysql local
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
    # CREATE TEGIA USER
    #

    export MYSQL_PWD=$(mysql_debian_password)

    iffinduser="$(mysql -u debian-sys-maint --execute="SELECT host,user FROM mysql.user WHERE host = '$mysql_host' AND user = 'tegia_user';")"
    if [[ "${#iffinduser}" != 0 ]]; then

        #
        # Удалить пользователя
        #
        
        mysql -u debian-sys-maint --port=$mysql_port --execute="DROP USER 'tegia_user'@'$mysql_host';"
    fi

    mysql -u debian-sys-maint --port=$mysql_port << EOF
    CREATE USER 'tegia_user'@'$mysql_host' IDENTIFIED BY '$mysql_password';
    GRANT ALL PRIVILEGES ON *.* TO 'tegia_user'@'$mysql_host';
    FLUSH PRIVILEGES;
EOF

    echo -e "${_OK_}tegia_user is created on MySQL"

	#
	# SAVE tegia.cnf FILE
	#

	tee ../tegia.cnf << EOF > /dev/null
[mysql]
host=$mysql_host
port=$mysql_port
user=$mysql_user
password=$mysql_password
EOF

	echo "${_OK_}file '$tegia_folder/tegia.cnf' is saved"

}


# ----------------------------------------------------------------------------------------
#
# ----------------------------------------------------------------------------------------

title="Установка MySQL"
prompt="${YELLOW}Выберите вариант установки:${RESET}"
options=("Локальная становка MySQL" "Не устанавливать MySQL")

echo "$title"
PS3="$prompt "
select opt in "${options[@]}"; do 
    case "$REPLY" in
    1) mysql_install; break;;
    2) echo "${_OK_}MySQL не будет устанавливаться"; break;;
    *) echo "Invalid option. Try another one.";continue;;
    esac
done

# ----------------------------------------------------------------------------------------
#
# Устанавливаем зависимости
#
# ----------------------------------------------------------------------------------------

sudo apt install -y mc screen jq zip
sudo apt install -y python build-essential default-libmysqlclient-dev libtool m4 automake uuid-dev libxml2-dev
sudo apt install -y libcurl4-openssl-dev libssl-dev
sudo apt install -y cmake zlibc
sudo apt install -y libbz2-dev libzip-dev unzip
sudo apt install -y pkg-config

sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-11 gcc-11
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11

# ----------------------------------------------------------------------------------------
#
# Настраиваем директории
#
# ----------------------------------------------------------------------------------------

mkdir -p $tegia_folder/vendors
mkdir -p $tegia_folder/configurations
mkdir -p $tegia_folder/applications
mkdir -p $tegia_folder/ui

cd $tegia_folder
ln -s $tegia_folder/tegia-node/include $tegia_folder/tegia-node/tegia

# ----------------------------------------------------------------------------------------
#
# Загружаем и настраиваем используемые библиотеки
#
# ----------------------------------------------------------------------------------------

cd $tegia_folder/vendors
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

sudo ./bootstrap-vcpkg.sh -disableMetrics
sudo ./vcpkg install nlohmann-json json-schema-validator fmt vincentlaucsb-csv-parser xpack cpp-jwt

#
# xml2json
#

cd $tegia_folder/vendors
git clone https://github.com/Cheedoong/xml2json
sudo ln -fs $tegia_folder/vendors/xml2json /usr/include/xml2json

#
# tegia include files
#

if [ -d /usr/include/tegia2/ ]; then
	sudo rm -R /usr/include/tegia2
fi

sudo ln -fs $tegia_folder/tegia-node/include /usr/include/tegia2

# ----------------------------------------------------------------------------------------
#
# Файл с флагами компиляции
#
# ----------------------------------------------------------------------------------------

tee $tegia_folder/Makefile.variable << EOF > /dev/null
ProdFlag			= -rdynamic -I${tegia_folder}/tegia-node -std=c++2a -march=native -m64 -O2
DevFlag				= -rdynamic -I${tegia_folder}/tegia-node -std=c++2a -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
Flag = \$(DevFlag)
EOF


exit 0





