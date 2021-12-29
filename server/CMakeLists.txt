project(server)

# mysql库的编译器选项
set(mysqlCFlagOpt "")
set(mysqlCxxFlagOpt "")
# 输出路径
set(mysqlBinaryDir ${PROJECT_BINARY_DIR}/../deps/mysql)
# 产生的重要文件
set(mysqlVersionFilePath ${mysqlBinaryDir}/include/mysql_version.h)
set(mysqlLibOutPath ${mysqlBinaryDir}/libmysql/libmysqlclient.a)
# target名称
set(mysqlLibTarget mysqlclient)
if (BILLING_BUILD_M32)
    set(mysqlCFlagOpt "-DCMAKE_C_FLAGS=-m32")
    set(mysqlCxxFlagOpt "-DCMAKE_CXX_FLAGS-m32")
endif ()
# 如果进行动态链接
if (${BUILD_SHARED_LIBS})
    set(mysqlLibOutPath ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmysqlclient.so)
    set(mysqlLibTarget libmysql)
endif ()
add_custom_command(OUTPUT ${mysqlVersionFilePath} ${mysqlLibOutPath}
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/deps/mysql
        COMMAND cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${mysqlCFlagOpt} ${mysqlCxxFlagOpt} -S ${PROJECT_SOURCE_DIR}/../deps/mysql-connector-c-6.1.11-src -B ${mysqlBinaryDir}
        COMMAND cmake --build ${mysqlBinaryDir} --target ${mysqlLibTarget})
include_directories(../deps/mysql-connector-c-6.1.11-src/include ${mysqlBinaryDir}/include ../deps/yaml-cpp/include)
# generate build info
include(cmake/build_info.cmake)
include(cmake/cmd_text.cmake)
#
set(serverSourceList ${mysqlVersionFilePath})
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

