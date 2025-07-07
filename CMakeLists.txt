cmake_minimum_required(VERSION 3.16)

#
# PROJECT name and version
#

project(tegia-node VERSION 2.0.0 LANGUAGES CXX)

#
# SET C++20 standard
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# PROD
# set(TEGIA_FLAGS "-rdynamic -march=native -m64 -Og")

# DEV
set(TEGIA_FLAGS "-rdynamic -march=native -m64 -Og -g -Wpedantic -Wshadow=compatible-local -Wl,--no-as-needed")

# DEBUG
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${TEGIA_FLAGS}")

# link_directories(/usr/local/lib)

#
# BUILD
#

file(GLOB_RECURSE TEGIA_SOURCES "./src/*.cpp")
add_executable(${PROJECT_NAME} ${TEGIA_SOURCES})

#
# INCLUDE
#

target_include_directories(${PROJECT_NAME} PRIVATE ./include)
target_include_directories(${PROJECT_NAME} PRIVATE ../vendors)
target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/libxml2)
target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include/quickjs)

#
# GLOBAL LIBRARY
#

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE dl)
target_link_libraries(${PROJECT_NAME} PRIVATE uuid)
target_link_libraries(${PROJECT_NAME} PRIVATE z)
target_link_libraries(${PROJECT_NAME} PRIVATE curl)
target_link_libraries(${PROJECT_NAME} PRIVATE crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE mysqlclient)
target_link_libraries(${PROJECT_NAME} PRIVATE xml2)
target_link_libraries(${PROJECT_NAME} PRIVATE quickjs)

#
# VCPKG LIBRARY
#

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

find_package(nlohmann_json_schema_validator REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json_schema_validator::validator)

find_package(cpp-jwt REQUIRED)
# add_subdirectory(../vendors/cpp-jwt)
target_link_libraries(${PROJECT_NAME} PRIVATE cpp-jwt::cpp-jwt)
