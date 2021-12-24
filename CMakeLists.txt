cmake_minimum_required(VERSION 3.10)

message("build type ${CMAKE_BUILD_TYPE}")
project(billing VERSION 1.0.0 LANGUAGES CXX)
# 静态链接
set(CMAKE_CXX_FLAGS "-static -g0 -Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS "-static ${CMAKE_C_FLAGS}")
# 是否编译32位
option(BILLING_BUILD_M32 "Build 32bit billing" OFF)
if (BILLING_BUILD_M32)
  message("build 32bit")
  set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
endif()
add_subdirectory(deps/yaml-cpp)
add_subdirectory(server)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} server)
file(COPY config.yaml DESTINATION ${CMAKE_BINARY_DIR})
