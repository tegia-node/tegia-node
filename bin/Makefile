include ../../Makefile.variable

# =============================================================================================================
#
# MAIN
#
# =============================================================================================================	
		
platform:
	@echo " "
	@echo "------------------------------------------------------------"
	@echo "TEGIA PLATFORM: \033[32;1m COMPILING \033[0m"
	@echo "------------------------------------------------------------"
	@echo " "

#	CORE	
	g++ -c -o core.crypt.o               ../src/core/crypt.cpp            $(Flag)
	g++ -c -o core.string.o              ../src/core/string.cpp           $(Flag)
	g++ -c -o core.json.o                ../src/core/json.cpp             $(Flag)
	g++ -c -o core.http.o                ../src/core/http.cpp             $(Flag)
	g++ -c -o core.cast.o                ../src/core/cast.cpp             $(Flag)

#	CONTACTS
	g++ -c -o core.contacts.phone.o      ../src/core/contacts/phone.cpp   $(Flag)
	g++ -c -o core.contacts.email.o      ../src/core/contacts/email.cpp   $(Flag)
	g++ -c -o core.contacts.vk.o         ../src/core/contacts/vk.cpp      $(Flag)
	g++ -c -o core.contacts.person.o     ../src/core/contacts/person.cpp  $(Flag)

#	THREAD
	g++ -c -o thread_pool.o              ../src/threads/pool.cpp          $(Flag)
	g++ -c -o thread_data.o              ../src/threads/data.cpp          $(Flag)

#	MYSQL
	g++ -c -o mysql_records.o            ../src/db/mysql/records.cpp      $(Flag)
	g++ -c -o mysql_connection.o         ../src/db/mysql/connection.cpp   $(Flag)
	g++ -c -o mysql_provider.o           ../src/db/mysql/provider.cpp     $(Flag)
	g++ -c -o mysql.o                    ../src/db/mysql/mysql.cpp        $(Flag)

	g++ -c -o sphinx.o                   ../src/db/sphinx/sphinx.cpp      $(Flag)

#	CONTEXT	
	g++ -c -o context.o                  ../src/context/context.cpp       $(Flag)
	g++ -c -o user.o                     ../src/context/user.cpp          $(Flag)

#	ACTORS
	g++ -c -o actors_types.o             ../src/actors/types.cpp          $(Flag)

#	NODE
	g++ -c -o node.o                     ../src/node/node.cpp             $(Flag)
	g++ -c -o node.config.o              ../src/node/config.cpp           $(Flag)
	g++ -c -o node.logger.o              ../src/node/logger.cpp           $(Flag)
	g++ -c -o tegia.o                    ../src/tegia.cpp                 $(Flag)
	g++ -c -o main.o                     ../src/main.cpp                  $(Flag)

#	APP
	g++ -c -o app_router.o               ../src/app/router.cpp            $(Flag)

	g++ -o tegia-node main.o tegia.o \
		core.crypt.o core.string.o core.json.o core.http.o core.cast.o \
		core.contacts.phone.o 	core.contacts.email.o  core.contacts.vk.o core.contacts.person.o \
		thread_pool.o thread_data.o \
		mysql_records.o mysql_connection.o mysql_provider.o	mysql.o \
		sphinx.o	\
		context.o user.o \
		actors_types.o \
		node.o node.config.o node.logger.o \
		app_router.o \
		-pthread -ldl -lz -lcurl -lcrypto -lmysqlclient -luuid -lstdc++fs -lxml2 \
		-lnlohmann_json_schema_validator -lfmt `pkg-config --cflags --libs gumbo` \
		$(Flag)