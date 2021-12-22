project(server)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deps/mysql/include/mysql_version.h ${CMAKE_BINARY_DIR}/deps/mysql/libmysql/libmysqlclient.a
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-Wno-error -DCMAKE_CXX_FLAGS=-Wno-error -S ${PROJECT_SOURCE_DIR}/../deps/mysql-connector-c-6.1.11-src -B ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake --build ${CMAKE_BINARY_DIR}/deps/mysql --target mysqlclient -v)
include_directories(../deps/mysql-connector-c-6.1.11-src/include ${CMAKE_BINARY_DIR}/deps/mysql/include ../deps/yaml-cpp/include)
# scan
aux_source_directory(cmd cmdSourceList)
aux_source_directory(common commonSourceList)
aux_source_directory(services servicesSourceList)
set(serverSourceList "${cmdSourceList};${servicesSourceList};${commonSourceList}")
#
add_library(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/deps/mysql/include/mysql_version.h ${serverSourceList})
target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/deps/mysql/libmysql/libmysqlclient.a dl pthread yaml-cpp)

