#This project is configured with CMake, but don't expect CMake builds to actually work! It's only for integration with CLion, use the
#makefile in the project root to build.

cmake_minimum_required(VERSION 3.13)
project(ds_launcher)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "-DNDS -DDSI")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

# Set to your devkitPro's installation dir if environment variable does not exist.
set(DEVKITPRO $ENV{DEVKITPRO})
set(LIBNDS ${DEVKITPRO}/libnds)
set(LIBNDS_INCLUDE ${LIBNDS}/include)
set(LIBNDS_LINK ${LIBNDS}/lib)

include_directories(${LIBNDS_INCLUDE})
link_directories(${LIBNDS_LINK})

add_subdirectory(lib/porklib)

add_subdirectory(arm7)
add_subdirectory(arm9)
