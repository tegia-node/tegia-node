#!/bin/bash

#set -e

. /usr/share/debconf/confmodule


db_get tegia-test/mysql-username
MYSQL_USERNAME="$RET"


echo "[$MYSQL_USERNAME]"

echo "1.0.4 v"


