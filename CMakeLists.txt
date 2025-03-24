cmake_minimum_required(VERSION 3.10)
project(rpl_tugas1)

set(CMAKE_CXX_STANDARD 17)

set(MYSQL_CONNECTOR_CPP_INCLUDE_DIR /usr/include/mysql-cppconn)
set(MYSQL_CONNECTOR_CPP_LIB /usr/lib/libmysqlcppconn.so)

include_directories(${MYSQL_CONNECTOR_CPP_INCLUDE_DIR})
link_directories(${MYSQL_CONNECTOR_CPP_LIB})

add_executable(cli_cpp main.cpp)
target_link_libraries(cli_cpp PRIVATE mysqlcppconn)
