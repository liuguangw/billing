project(server)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/mysql/include/mysql_version.h ${CMAKE_BINARY_DIR}/mysql/libmysql/libmysqlclient.a
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/mysql
        COMMAND cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-Wno-error -DCMAKE_CXX_FLAGS=-Wno-error -S ${PROJECT_SOURCE_DIR}/../deps/mysql-connector-c-6.1.11-src -B ${CMAKE_BINARY_DIR}/mysql
        COMMAND cmake --build ${CMAKE_BINARY_DIR}/mysql --target mysqlclient -v)
include_directories(../deps/mysql-connector-c-6.1.11-src/include ${CMAKE_BINARY_DIR}/mysql/include)
add_library(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/mysql/include/mysql_version.h cmd/command.cpp services/database_connection.cpp)
target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/mysql/libmysql/libmysqlclient.a dl pthread)

