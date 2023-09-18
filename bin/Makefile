-include ../../Makefile.variable

PROJECT := tegia-node
CPPFLAGS += -I../../vendors/ftpclient/FTP

# =============================================================================================================
#
# INSTALL SOURCE
#
# =============================================================================================================


install:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m CONFIGURE SOURCE \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	bash ./sphinx-install.sh
	bash ./install.sh
	make tegia-node
	sudo ln -fs $(realpath .)/tegia-node /usr/bin/tegia-node

	@echo " "
	@echo "TEGIA PLATFORM: \033[32;1m INSTALL SUCCESS \033[0m"
	@echo " "


# =============================================================================================================
#
# MAIN
#
# =============================================================================================================	

tegia-node: platform nodes actors sockets mysql linking

platform:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -c -o ISS.o                     ../src/platform/ISS.cpp         $(Flag)
	g++ -c -o core.o                    ../src/core/core.cpp            $(Flag)
	g++ -c -o core.cast.o               ../src/core/cast.cpp            $(Flag)
	g++ -c -o core.string.o             ../src/core/string.cpp          $(Flag)
	g++ -c -o core.time.o               ../src/core/time.cpp            $(Flag)
	g++ -c -o core.crypt.o              ../src/core/crypt.cpp           $(Flag)

	g++ -c -o core.http.o               ../src/core/http.cpp            $(Flag)
	g++ -c -o core.http2.o              ../src/core/http2.cpp           $(Flag)
	g++ -c -o core.html_parser.o        ../src/core/html_parser.cpp     $(Flag)

	g++ -c -o core.person.o             ../src/core/person.cpp          $(Flag)
	g++ -c -o core.documents.o          ../src/core/documents.cpp       $(Flag)

	g++ -c -o core.html_parser2.o       ../src/core/html_parser2.cpp  -I/usr/include/libxml2/   $(Flag)

	g++ -c -o thread.pool.o             ../src/threads/thread_pool.cpp  $(Flag)

	g++ -c -o tegia.o                   ../src/tegia.cpp                $(Flag)

	g++ -c -o core.json.o               ../src/core/json.cpp            $(Flag)
	g++ -c -o context.jwt.o             ../src/context/jwt.cpp -I../../vendors/cpp-jwt/include/ $(Flag)

	g++ -c -o source.event.o            ../src/source/event/event.cpp    $(Flag)

#	Sphinx
	g++ -c -o sphinx.o                  ../src/db/sphinx/sphinx.cpp     $(Flag)

	g++ -c -o main.o           ../src/main.cpp $(Flag)


sockets: 
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING SOCKETS \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -c -o AbstractSocket.o ../src/sockets/AbstractSocket.cpp  $(Flag) $(CPPFLAGS)
	g++ -c -o ClientSocket.o   ../src/sockets/ClientSocket.cpp    $(Flag) $(CPPFLAGS)
	g++ -c -o ServerSocket.o   ../src/sockets/ServerSocket.cpp    $(Flag) $(CPPFLAGS)
	g++ -c -o SocketManager.o  ../src/sockets/SocketManager.cpp   $(Flag) $(CPPFLAGS)

mysql:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING MYSQL \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -c -o mysql.records.o           ../src/db/mysql/mysql_records.cpp     $(Flag)
	g++ -c -o mysql.connection.o        ../src/db/mysql/mysql_connection.cpp  $(Flag)
	g++ -c -o mysql.pool.o              ../src/db/mysql/mysql_pool.cpp        $(Flag)
	g++ -c -o mysql.provider.o          ../src/db/mysql/mysql_provider.cpp    $(Flag)
	g++ -c -o mysql.o                   ../src/db/mysql/mysql.cpp             $(Flag)

nodes: 
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING NODES \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -c -o node.logger.o	../src/node/logger.cpp	$(Flag) $(CPPFLAGS)
	g++ -c -o node.config.o ../src/node/config.cpp  $(Flag) $(CPPFLAGS)
	g++ -c -o node.o        ../src/node/node.cpp    $(Flag) $(CPPFLAGS)

actors:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING ACTORS \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -c -o actors.actor_list.o 	../src/actors/actor_list.cpp $(Flag) $(CPPFLAGS)
	g++ -c -o actors.add_task.o     ../src/actors/add_task.cpp   $(Flag) $(CPPFLAGS)
	g++ -c -o actors.message_t.o    ../src/actors/message_t.cpp  $(Flag) $(CPPFLAGS)
 
tegia-actors: actors linking

tegia-sockets: sockets linking

tegia-nodes: nodes linking

tegia-mysql: mysql linking

linking:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m LINKING \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

	g++ -o tegia-node *.o \
		-pthread -ldl -lz -lcurl -lcrypto -lmysqlclient -luuid -lstdc++fs \
		-lxml2 \
		-lnlohmann_json_schema_validator -lfmt `pkg-config --cflags --libs gumbo` \
		$(Flag)


# =============================================================================================================
#
# CREATE TEGIA NODE DEB
#
# =============================================================================================================

tegia-node-bin-deb:
	@echo "\033[32m"
	@echo "============================================================"
	@echo "CREATE TEGIA NODE DEB"
	@echo "============================================================"
	@echo "\033[0m"

#   Собираем и копируем ноду
	make tegia-node
	cp tegia-node ../deb/tegia-node-bin/usr/sbin/tegia-node
#   Собираем и копируем конфигуратор
	@echo "\033[32mmake tegia-config\033[0m"
	make tegia-config
	cp tegia-config ../deb/tegia-node-bin/usr/sbin/tegia-config
#   Sphinx
	cp configs/sphinx.conf             ../deb/tegia-node-bin/etc/tegia/sphinx.conf
	cp ../../sphinx-3.1.1/bin/searchd  ../deb/tegia-node-bin/usr/sbin/searchd

#	Копируем конфигурационные файлы
	cp configs/config.example.json ../deb/tegia-node-bin/etc/tegia/config.json

	sudo cp /usr/local/lib/libgumbo.so.1.0.0 ../deb/tegia-node-bin/usr/lib/libgumbo.so
	rm -r -f ../deb/tegia-node-bin/usr/include/tegia/
	cp -r ../include/ ../deb/tegia-node-bin/usr/include/tegia/

#   Собираем deb-пакет
	@echo "\033[32mmake deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-node-bin
	cp ../deb/tegia-node-bin.deb ../../debs/tegia-node-bin.deb

# =============================================================================================================
#
# CREATE TEGIA DEVTOOLS DEB
#
# =============================================================================================================

tegia-devtools-deb:
	@echo "\033[32m"
	@echo "============================================================"
	@echo "CREATE TEGIA DEVTOOLS DEB"
	@echo "============================================================"
	@echo "\033[0m"

#   Собираем deb-пакет
	@echo "\033[32mmake tegia-devtools deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-devtools
	cp ../deb/tegia-devtools.deb ../../debs/tegia-devtools.deb

# =============================================================================================================
#
# CREATE TEGIA DOXYGEN DOCUMENTATIONS DEB
#
# =============================================================================================================

tegia-doxygen-deb:
	@echo "\033[32m"
	@echo "============================================================"
	@echo "CREATE TEGIA DOXYGEN DOCUMENTATIONS DEB"
	@echo "============================================================"
	@echo "\033[0m"

	cp -r ../docs/doxygen ../deb/tegia-doxygen/var/www/tegia
#   Собираем deb-пакет
	@echo "\033[32mmake tegia-doxygen deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-doxygen
	cp ../deb/tegia-doxygen.deb ../../debs/tegia-doxygen.deb

# =============================================================================================================
#
# CREATE TEGIA NODE SOURCE DEB
#
# =============================================================================================================

tegia-node-source-deb:
	@echo "\033[32m"
	@echo "============================================================"
	@echo "CREATE TEGIA NODE SOURCE DEB"
	@echo "============================================================"
	@echo "\033[0m"


#   Собираем deb-пакет
	@echo "\033[32mmake tegia-node-source deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-node-source
	cp ../deb/tegia-node-source.deb ../../debs/tegia-node-source.deb

# =============================================================================================================
#
# CREATE TEGIA TEST DEB
#
# =============================================================================================================

tegia-test-deb:
	@echo "\033[32m"
	@echo "============================================================"
	@echo "CREATE TEGIA TEST DEB"
	@echo "============================================================"
	@echo "\033[0m"

#   Собираем конфигурационные файлы
#	/bin/cat ../deb/scripts/start > ../deb/tegia-test/DEBIAN/postinst
#	/bin/cat ../deb/scripts/mysql_password >> ../deb/tegia-test/DEBIAN/postinst
#	/bin/cat ../deb/scripts/git_clone >> ../deb/tegia-test/DEBIAN/postinst
#	/bin/cat ../deb/scripts/tegia-test/postinst >> ../deb/tegia-test/DEBIAN/postinst
#	sudo chmod 0775 ../deb/tegia-test/DEBIAN/postinst

#   Собираем deb-пакет
	@echo "\033[32mmake tegia-test deb-package\033[0m"
	cd ../deb/ && fakeroot dpkg-deb --build tegia-test
	cp ../deb/tegia-test.deb ../../debs/tegia-test.deb

# =============================================================================================================
#
# MAKE TEGIA CONFIG
#
# =============================================================================================================

tegia-config:
	g++ -c -o jsoncpp.o      ../../vendors/jsoncpp/dist/jsoncpp.cpp -I../../vendors/jsoncpp/include/ $(Flag)	 -fPIC
	g++ -c -o core.json.o       ../src/core/json.cpp $(Flag) -fPIC

	g++ -c -o tegia-config.o           ../src/config/tegia-config.cpp $(Flag)
	g++ -o tegia-config tegia-config.o jsoncpp.o core.json.o \
	-lstdc++fs $(Flag)
