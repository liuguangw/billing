cmake_minimum_required(VERSION 3.10)

message("build type ${CMAKE_BUILD_TYPE}")
project(billing VERSION 1.0.0 LANGUAGES CXX)
# 是否编译32位
option(BILLING_BUILD_M32 "Build 32bit billing" OFF)
if (BILLING_BUILD_M32)
    message("build 32bit")
    set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
endif ()
# 是否进行动态链接
if (${BUILD_SHARED_LIBS})
    message("build shared")
else ()
    message("build static")
    set(CMAKE_CXX_FLAGS "-static ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-static ${CMAKE_C_FLAGS}")
endif ()
# 动态链接库的输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
add_subdirectory(deps/yaml-cpp)
add_subdirectory(server)
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./lib")
target_link_libraries(${PROJECT_NAME} server)
file(COPY config.yaml DESTINATION ${PROJECT_BINARY_DIR})
# compress
if (NOT (${BUILD_SHARED_LIBS}))
    find_program(UPX_BIN upx)
    if (NOT ("${UPX_BIN}" STREQUAL "UPX_BIN-NOTFOUND"))
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND echo "using upx ..."
                COMMAND ${UPX_BIN} --best ${PROJECT_BINARY_DIR}/${PROJECT_NAME})
    endif ()
endif ()
