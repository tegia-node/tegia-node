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
	g++ -c -o core_crypt.o               ../src/core/crypt.cpp            $(Flag)
	g++ -c -o core.json.o                ../src/core/json.cpp             $(Flag)

#	THREAD
	g++ -c -o thread_pool.o              ../src/threads/pool.cpp          $(Flag)
	g++ -c -o thread_data.o              ../src/threads/data.cpp          $(Flag)

#	MYSQL
	g++ -c -o mysql_records.o            ../src/db/mysql/records.cpp      $(Flag)
	g++ -c -o mysql_connection.o         ../src/db/mysql/connection.cpp   $(Flag)
	g++ -c -o mysql_provider.o           ../src/db/mysql/provider.cpp     $(Flag)
	g++ -c -o mysql.o                    ../src/db/mysql/mysql.cpp        $(Flag)

#	CONTEXT	
	g++ -c -o context.o                  ../src/context/context.cpp       $(Flag)
	g++ -c -o user.o                     ../src/context/user.cpp          $(Flag)

#	NODE
	g++ -c -o node.o                     ../src/node/node.cpp             $(Flag)
	g++ -c -o node.config.o              ../src/node/config.cpp           $(Flag)
	g++ -c -o node.logger.o              ../src/node/logger.cpp           $(Flag)
	g++ -c -o main.o                     ../src/main.cpp                  $(Flag)
 
	g++ -o tegia-node main.o \
		core_crypt.o core.json.o \
		thread_pool.o thread_data.o \
		mysql_records.o mysql_connection.o mysql_provider.o	mysql.o \
		context.o user.o \
		node.o node.config.o node.logger.o \
		-pthread -ldl -lz -lcurl -lcrypto -lmysqlclient -luuid -lstdc++fs -lxml2 \
		-lnlohmann_json_schema_validator -lfmt `pkg-config --cflags --libs gumbo` \
		$(Flag)

