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

#
# BUILD
#

file(GLOB_RECURSE TEGIA_SOURCES "./src/*.cpp")
add_executable(${PROJECT_NAME} ${TEGIA_SOURCES})

#
# INCLUDE
#

target_include_directories(${PROJECT_NAME} PRIVATE ./include)

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

#
# VCPKG LIBRARY
#

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

find_package(nlohmann_json_schema_validator REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json_schema_validator::validator)

