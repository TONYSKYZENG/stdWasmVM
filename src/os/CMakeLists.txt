cmake_minimum_required (VERSION 2.8)
include_directories  (${PROJECT_SOURCE_DIR}/include)
set(VTIMER_AMOUNT 500)
option(VOS_LESS_VTIMER "to have less vtimers,adviced in embebed systems" OFF)
if(VOS_LESS_VTIMER)
set(VTIMER_AMOUNT 50)
endif()
message(STATUS "vos has " ${VTIMER_AMOUNT} " virtual timers")
option(VOS_POSIX_RT "build the vos with posix's real time" ON)
if(VOS_POSIX_RT)
message(STATUS "generate vos with posix real time")
add_definitions(-DVOS_POSIX_RT)
endif()
configure_file(
        "osconfig.h.in"
        "${PROJECT_SOURCE_DIR}/include/osconfig.h"
)
