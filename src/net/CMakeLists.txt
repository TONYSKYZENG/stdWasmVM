
message(STATUS "select pack " ${RUN_PLATFORM} " for lower net support")
message(STATUS "compile " ${LOW_DRV} " for lower net support")

#set(CMAKE_C_COMPILER "gcc")
#set(CMAKE_CXX_COMPILER "g++")
include_directories  (${PROJECT_SOURCE_DIR}/include)

configure_file(
        "netconfig.h.in"
        "${PROJECT_SOURCE_DIR}/include/netconfig.h"
)
#detect driver type, or you can assign one
