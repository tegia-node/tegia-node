#!/bin/sh


# ===========================================================================

RED=`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`


# Получаем пароль от MySQL

string=$(sudo cat /etc/mysql/debian.cnf)
regsubstring="password"
passwd="${string#*password}"
passwd="${passwd%%socket*}"
len="$((${#passwd}-4))"
passwd="${passwd:3:$len}"

# Создаем Пользователя

MYSQL_DEBIAN_PASSWORD=$passwd
export MYSQL_PWD=$MYSQL_DEBIAN_PASSWORD;

#db_get tegia/mysql-username
MYSQL_USERNAME="tegia_test_4"

#db_get tegia/mysql-username
MYSQL_PASSWORD="gtcymKmlf"

mysql -u  debian-sys-maint -e "CREATE USER '$MYSQL_USERNAME'@'localhost' IDENTIFIED BY '$MYSQL_PASSWORD';"
mysql -u  debian-sys-maint -e "GRANT ALL PRIVILEGES ON *.* TO '$MYSQL_USERNAME'@'localhost';"
mysql -u  debian-sys-maint -e "FLUSH PRIVILEGES;"


echo "${GREEN}============================================================="
echo "Установка TEGIA Platform завершена"	
echo "=============================================================${RESET}"

exit 0