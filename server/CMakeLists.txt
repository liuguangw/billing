project(server)

set(mysqlCFlagOpt "")
set(mysqlCxxFlagOpt "")
if (BILLING_BUILD_M32)
    set(mysqlCFlagOpt "-DCMAKE_C_FLAGS=-m32")
    set(mysqlCxxFlagOpt "-DCMAKE_CXX_FLAGS=-m32")
endif ()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deps/mysql/include/mysql_version.h ${CMAKE_BINARY_DIR}/deps/mysql/libmysql/libmysqlclient.a
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake -DCMAKE_BUILD_TYPE=MinSizeRel ${mysqlCFlagOpt} ${mysqlCxxFlagOpt} -S ${PROJECT_SOURCE_DIR}/../deps/mysql-connector-c-6.1.11-src -B ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake --build ${CMAKE_BINARY_DIR}/deps/mysql --target mysqlclient)
include_directories(../deps/mysql-connector-c-6.1.11-src/include ${CMAKE_BINARY_DIR}/deps/mysql/include ../deps/yaml-cpp/include)
#
set(serverSourceList ${CMAKE_BINARY_DIR}/deps/mysql/include/mysql_version.h)
set(sourceDirList bhandler cmd common debug models services)
foreach (sourceDir IN LISTS sourceDirList)
    aux_source_directory(${sourceDir} nodeSourceList)
    list(APPEND serverSourceList ${nodeSourceList})
endforeach ()
#
#foreach(cppFilePath IN LISTS serverSourceList)
#    message(${cppFilePath})
#endforeach()
#
add_library(${PROJECT_NAME} "${serverSourceList}")
target_link_libraries(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/deps/mysql/libmysql/libmysqlclient.a dl pthread yaml-cpp)

