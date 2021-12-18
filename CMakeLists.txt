cmake_minimum_required(VERSION 3.10)

project(billing)
include_directories(deps/mysql-connector-c-6.1.11-src/include build/mysql/include)
link_directories(build/mysql/libmysql)
# 静态链接
set(CMAKE_CXX_FLAGS "-static -g0 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-static ${CMAKE_C_FLAGS}")

add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} mysqlclient pthread dl)

