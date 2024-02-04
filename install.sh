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

tegia_folder=$(realpath ../)

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

tee ../tegia.cnf << EOF > /dev/null
[mysql]
host=$mysql_host
port=$mysql_port
user=$mysql_user
password=$mysql_password
EOF

echo "${_OK_}file '$tegia_folder/tegia.cnf' is saved"


