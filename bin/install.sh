#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

_OK_="${GREEN}[OK]  ${RESET}"
_ERR_="${RED}[ERR] ${RESET}"


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

tegia_folder=$(realpath ../../)


# ----------------------------------------------------------------------------------------
#
# Настраиваем подключение к MySQL
#
# ----------------------------------------------------------------------------------------

#
# HOST
#

mysql_host=$(echo $MYSQL_HOST)

#
# PORT
#

mysql_port=$(echo $MYSQL_PORT)
echo -e "${_OK_}set port = '$mysql_port'"

#
# TEGIA USER & PASSWORD
#

mysql_user='tegia_user'
mysql_password=$(echo $MYSQL_PASSWORD)
echo -e "\n${_OK_}tegia user = '$mysql_user'"
echo -e "${_OK_}mysql password is set"

#
# CONFIGURE MYSQL
#

echo -e "Configure MySQL"

if [[ "$mysql_host" == "localhost" || "$mysql_host" == "127.0.0.1" ]]; then

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

else
    echo -e "removed"
    mysql_remote_host=$(ip route get 1.1.1.1 | awk -- '{printf $7}')

    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y mysql-client

    #
    # TODO
    #

#    mysql --defaults-extra-file=../../mysql.cnf <<EOF
#    CREATE USER 'tegia_user'@'$mysql_remote_host' IDENTIFIED BY '$mysql_password';
#    GRANT ALL PRIVILEGES ON *.* TO 'tegia_user'@'$mysql_remote_host';"
#    FLUSH PRIVILEGES;
#EOF

    rm -f $tegia_folder/mysql.cnf

fi

#
# SAVE tegia.cnf FILE
#

tee ../../tegia.cnf << EOF > /dev/null
[mysql]
host=$mysql_host
port=$mysql_port
user=$mysql_user
password=$mysql_password
EOF

echo "${_OK_}file '$tegia_folder/tegia.cnf' is saved"


# ----------------------------------------------------------------------------------------
#
# Устанавливаем зависимости
#
# ----------------------------------------------------------------------------------------

sudo apt install -y git mc screen jq zip
sudo apt install -y python build-essential default-libmysqlclient-dev libtool m4 automake uuid-dev libxml2-dev
sudo apt install -y libcurl4-openssl-dev libssl-dev
sudo apt install -y cmake zlibc
sudo apt install -y libbz2-dev libzip-dev unzip
sudo apt install -y pkg-config libpugixml-dev graphviz-dev

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
mkdir -p $tegia_folder/nodes
mkdir -p $tegia_folder/debs
mkdir -p $tegia_folder/ui

# ----------------------------------------------------------------------------------------
#
# Загружаем и настраиваем используемые библиотеки
#
# ----------------------------------------------------------------------------------------

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

sudo ./bootstrap-vcpkg.sh -disableMetrics
sudo ./vcpkg install nlohmann-json json-schema-validator gumbo fmt vincentlaucsb-csv-parser xpack xlnt cpp-jwt duckx

#
# xml2json
#

cd $tegia_folder/vendors
git clone https://github.com/Cheedoong/xml2json
sudo ln -fs $tegia_folder/vendors/xml2json /usr/include/xml2json


#
# fmt
#

cd $tegia_folder/vendors
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir -p build
cd build
cmake -DBUILD_SHARED_LIBS=TRUE ..
make
sudo make install
sudo ldconfig


#
# vincentlaucsb / csv-parser
#

cd $tegia_folder/vendors
git clone https://github.com/vincentlaucsb/csv-parser.git

#
# tfussell / xlnt
#

cd $tegia_folder/vendors
git clone https://github.com/tfussell/xlnt.git
cd xlnt/third-party && rm -rf libstudxml
git clone https://git.codesynthesis.com/libstudxml/libstudxml.git
cd ../
cmake .
make -j 2
sudo make install
sudo ldconfig

#
# jwt
#

cd $tegia_folder/vendors
git clone https://github.com/arun11299/cpp-jwt.git

#
# DuckX - ms word .docx generator
#

sudo apt install -y libpugixml-dev

cd $tegia_folder/vendors
git clone https://github.com/amiremohamadi/DuckX.git
cd DuckX
mkdir -p build
cd build
cmake .. -DBUILD_SHARED_LIBS=TRUE ..
cmake --build .
sudo make install
sudo ln -fs /usr/local/lib/libduckx.so /usr/lib/libduckx.so

#
# graphviz-dev
#

sudo apt install -y graphviz-dev

#
# tegia include files
#

if [ -d /usr/include/tegia/ ]; then
	sudo rm -R /usr/include/tegia
fi

sudo ln -fs $tegia_folder/platform/include /usr/include/tegia


# ----------------------------------------------------------------------------------------
#
# Файл с флагами компиляции
#
# ----------------------------------------------------------------------------------------

tee $tegia_folder/Makefile.variable << EOF > /dev/null
ProdFlag			= -rdynamic -std=c++2a -march=native -m64 -O2
DevFlag				= -rdynamic -std=c++2a -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed 
Flag = \$(DevFlag)
EOF


exit 0
