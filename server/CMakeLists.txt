project(server)

set(mysqlCFlagOpt "")
set(mysqlCxxFlagOpt "")
set(mysqlLibOutPath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmysqlclient.a)
if (BILLING_BUILD_M32)
    set(mysqlCFlagOpt "-DCMAKE_C_FLAGS=-m32")
    set(mysqlCxxFlagOpt "-DCMAKE_CXX_FLAGS-m32")
endif ()
if (BILLING_BUILD_SHARED)
  set(mysqlLibOutPath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmysqlclient.so)
endif()
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/deps/mysql/include/mysql_version.h ${mysqlLibOutPath}
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${mysqlCFlagOpt} ${mysqlCxxFlagOpt} -S ${PROJECT_SOURCE_DIR}/../deps/mysql-connector-c-6.1.11-src -B ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake --build ${CMAKE_BINARY_DIR}/deps/mysql)
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
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./lib")
target_link_libraries(${PROJECT_NAME} ${mysqlLibOutPath} dl pthread yaml-cpp)

