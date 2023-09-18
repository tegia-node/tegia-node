#!/bin/bash
work_dir='sphinxsearch-v3-install'

# sphinx_dir_name='sphinx-3.3.1'
sphinx_dir_name='sphinx-3.4.1'

#sphinx_file_name='sphinx-3.3.1-b72d67b-linux-amd64.tar.gz'
sphinx_file_name='sphinx-3.4.1-efbcc65-linux-amd64.tar.gz'
sphinx_file_server="http://sphinxsearch.com/files/$sphinx_file_name"

sphinx_work_dir_list='/etc/sphinx /var/run/sphinx /var/log/sphinx /var/lib/sphinx /var/lib/sphinx/data'
sphinx_work_file_list='indexer indextool searchd wordbreaker'

sphinx_tmpfiles='/usr/lib/tmpfiles.d/sphinx.conf'
sphinx_conf='/etc/sphinx/sphinx.conf'
sphinx_service='/lib/systemd/system/sphinx.service'

command='install'
wait_time_question=10

while [ -n "$1" ]
do
    case "$1" in
        -delete) command='delete' ;;
        -clear) command='clear' ;;
        -update) command='update' ;;
    esac

    shift
done

if [ $command = 'install' ]
then
    if ! dpkg-query -l | grep mysql > /dev/null 2>&1
    then
        read -e -t $wait_time_question -p "MySQL is not installed! Install (yes/no)? " -i "yes" mysql_question

        if [ -z $mysql_question ]
        then
            echo ''
            mysql_question='yes'
        fi

        if [ $mysql_question = 'yes' ]
        then
            echo "MySQL is being installed"
            apt -y install mysql-server > /dev/null 2>&1
        else
            echo "Install MySQL and repeat one more time ;)"
            exit 0
        fi
    fi

    if ! dpkg-query -l | grep libmysqlclient > /dev/null 2>&1
    then
        read -e -t $wait_time_question -p "MySQL-dev (libmysqlclient-dev) is not installed! Install (yes/no)? " -i "yes" mysql_question

        if [ -z $mysql_question ]
        then
            echo ''
            mysql_question='yes'
        fi

        if [ $mysql_question = 'yes' ]
        then
            echo "MySQL-dev is being installed"
            apt -y install libmysqlclient-dev > /dev/null 2>&1
        else
            echo "Install MySQL-dev (libmysqlclient-dev) and repeat one more time ;)"
            exit 0
        fi
    fi

    if [ ! -d $work_dir ]
    then
        mkdir $work_dir
    fi

    cd $work_dir

    if [ ! -d $sphinx_dir_name ]
    then
        echo "Download $sphinx_dir_name"
        wget $sphinx_file_server > /dev/null 2>&1
        echo "Unpacking archive"
        tar -xzf $sphinx_file_name
    fi

    if ! id sphinx > /dev/null 2>&1
    then
        echo "Create user and group \"sphinx\""
        useradd -r -U -c 'Sphinxsearch system user' sphinx
    fi

    for dir in $sphinx_work_dir_list
    do
        if [ ! -d $dir ]
        then
            echo " * Create work dir: $dir"
            mkdir -p $dir
            chown -R sphinx:sphinx $dir
        fi
    done

    for file in $sphinx_work_file_list
    do
        if [ ! -f "/etc/sphinx/$file" ] && [ -f "$sphinx_dir_name/bin/$file" ]
        then
            echo " * Copy work file in etc: $file"
            cp "$sphinx_dir_name/bin/$file" "/etc/sphinx/"

            chown sphinx:sphinx "/etc/sphinx/$file"
        fi
    done

    if [ ! -f $sphinx_tmpfiles ]
    then
        echo "Create tmpfiles.d"
        printf "#Type Path        Mode UID      GID      Age Argument\nd /var/run/sphinx    0755 sphinx sphinx -   -" > $sphinx_tmpfiles
    fi

    if [ ! -f $sphinx_conf ]
    then
        if [ -f "../sphinx.conf" ]
        then
            echo "Copy sphinx.conf"
            cp ../sphinx.conf $sphinx_conf
        else
            echo "Create sphinx.conf"
            printf "index default\n{\n	type = rt\n	path = /var/lib/sphinx/data/default\n\n	rt_mem_limit = 256M\n	index_exact_words = 1\n	morphology = stem_enru\n\n	rt_field = el_name\n	rt_field = el_price\n	rt_field = el_catalog\n\n	stored_fields = el_name, el_price, el_catalog\n\n	rt_attr_uint = el_id\n}\n\nindexer\n{\n	mem_limit = 512M\n}\n\nsearchd\n{\n	listen = 127.0.0.1:9312\n	listen = 127.0.0.1:9306:mysql41\n	listen = /var/run/sphinx/searchd.sock:sphinx\n	log	= /var/log/sphinx/sphinx.log\n	query_log = /var/log/sphinx/query.log\n	read_timeout = 5\n	client_timeout = 300\n	max_children = 30\n	persistent_connections_limit = 30\n	pid_file = /var/run/sphinx/sphinx.pid\n	binlog_path = /var/lib/sphinx/data\n	seamless_rotate = 1\n	preopen_indexes = 1\n	unlink_old = 1\n	max_packet_size = 8M\n	max_filters	= 256\n	max_filter_values = 4096\n	max_batch_queries = 32\n	workers	= threads\n}" > $sphinx_conf
        fi

        chown sphinx:sphinx $sphinx_conf
    fi

    if [ ! -f $sphinx_service ]
    then
        echo "Create sphinx.service"
        printf "[Unit]\nDescription=Sphinx Search - Fast standalone full-text SQL search engine\nAfter=network.target\n\n[Service]\nType=simple\nWorkingDirectory=/etc/sphinx/\nExecStart=/etc/sphinx/searchd\nExecStop=/etc/sphinx/searchd --stop\nPIDFile=/var/run/sphinx/sphinx.pid\n\n[Install]\nWantedBy=multi-user.target" > $sphinx_service
    fi

    read -e -t $wait_time_question -p "Enable and start sphinx? (yes/no) " -i "yes" enable_sphinx

    if [ -z $enable_sphinx ]
    then
        echo ''
        enable_sphinx='yes'
    fi

    if [ $enable_sphinx = 'yes' ]
    then
        systemctl enable sphinx > /dev/null 2>&1
        systemctl start sphinx > /dev/null 2>&1
    fi

    if [ -d "/var/www/html" ]
    then
        read -e -t $wait_time_question -p "Copy sphinx document file (sphinx3.html) in your html folder? (yes/no) " -i "yes" sphinx_doc_file

        if [ -z $sphinx_doc_file ]
        then
            echo ''
            sphinx_doc_file='yes'
        fi

        if [ $sphinx_doc_file = 'yes' ]
        then
            cp "$sphinx_dir_name/doc/sphinx3.html" /var/www/html/sphinx3.html
        fi
    fi

    cd ..
    rm -r $work_dir
elif [ $command = 'delete' ]
then
    if systemctl --type=service | grep sphinx > /dev/null 2>&1
    then
        echo "Disable and stop sphinx"
        systemctl stop sphinx > /dev/null 2>&1
        systemctl disable sphinx > /dev/null 2>&1
        systemctl daemon-reload > /dev/null 2>&1
        systemctl reset-failed > /dev/null 2>&1
    fi

    if pidof searchd > /dev/null 2>&1
    then
        echo "Kill process sphinx"
        killall searchd > /dev/null 2>&1
    fi

    if dpkg-query -l | grep libmysqlclient > /dev/null 2>&1
    then
        read -e -t $wait_time_question -p "Uninstal MySQL-dev (libmysqlclient-dev) (yes/no)? " -i "no" mysql_question

        if [ -z $mysql_question ]
        then
            echo ''
            mysql_question='no'
        fi

        if [ $mysql_question = "yes" ]
        then
            echo "MySQL-dev (libmysqlclient-dev) is removed"
            apt -y remove --purge libmysqlclient* > /dev/null 2>&1
        fi
    fi

    if dpkg-query -l | grep mysql > /dev/null 2>&1
    then
        read -e -t $wait_time_question -p "Uninstal MySQL (yes/no)? " -i "no" mysql_question

        if [ -z $mysql_question ]
        then
            echo ''
            mysql_question='no'
        fi

        if [ $mysql_question = "yes" ]
        then
            echo "MySQL is removed"
            apt -y remove --purge mysql* > /dev/null 2>&1
        fi
    fi

    for dir in $sphinx_work_dir_list
    do
        if [ -d $dir ]
        then
            rm -r $dir
        fi
    done

    for file in $sphinx_work_file_list
    do
        if [ -f "/usr/bin/$file" ]
        then
            rm "/usr/bin/$file"
        fi
    done

    if id sphinx > /dev/null 2>&1
    then
        userdel sphinx
    fi

    if getent group sphinx > /dev/null 2>&1
    then
        groupdel sphinx
    fi

    if [ -d $work_dir ]
    then
        rm -r $work_dir
    fi

    if [ -f $sphinx_tmpfiles ]
    then
        rm $sphinx_tmpfiles
    fi

    if [ -f $sphinx_service ]
    then
        rm $sphinx_service
    fi
elif [ $command = 'clear' ]
then
    echo 'command == clear'
elif [ $command = 'update' ]
then
    echo 'command == update'
fi