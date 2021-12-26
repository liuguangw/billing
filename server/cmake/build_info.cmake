# git commit hash
find_package(Git REQUIRED)
execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        OUTPUT_VARIABLE BILLING_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# build time
set(ENV{TZ} Asia/Shanghai)
execute_process(
        COMMAND date "+%F %T GMT%:z"
        OUTPUT_VARIABLE BILLING_BUILD_TIME
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# build machine
execute_process(
        COMMAND sh ${PROJECT_SOURCE_DIR}/cmake/machine.sh
        OUTPUT_VARIABLE BILLING_BUILD_MACHINE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# configure
configure_file(${PROJECT_SOURCE_DIR}/common/build_info.h.in ${PROJECT_SOURCE_DIR}/common/build_info.h)