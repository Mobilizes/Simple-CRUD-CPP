cmake_minimum_required(VERSION 3.10)
project(rpl_tugas1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MYSQL_CONNECTOR_CPP_INCLUDE_DIR /usr/include/mysql-cppconn)
set(MYSQL_CONNECTOR_CPP_LIB /usr/lib/libmysqlcppconn.so)

add_library(${PROJECT_NAME} SHARED "controller.cpp" "repository.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${MYSQL_CONNECTOR_CPP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${MYSQL_CONNECTOR_CPP_LIB})
target_link_libraries(${PROJECT_NAME} PUBLIC mysqlcppconn)

add_executable(simple_cli_cpp "main.cpp")
target_include_directories(simple_cli_cpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" ${MYSQL_CONNECTOR_CPP_INCLUDE_DIR})
target_link_libraries(simple_cli_cpp ${PROJECT_NAME} mysqlcppconn)
