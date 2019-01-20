cmake_minimum_required(VERSION 3.9)

project(template-arm9)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS -DARM9)
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

# Set to your devkitPro's installation dir if environment variable does not exist.
set(DEVKITPRO $ENV{DEVKITPRO})

include_directories(${DEVKITPRO}/libnds/lib)
include_directories(${DEVKITPRO}/libnds/include)
include_directories(${DEVKITPRO}/libnds/include/nds)
include_directories(${DEVKITPRO}/libnds/include/nds/arm9)
include_directories(${DEVKITPRO}/devkitARM)

include_directories(include)

link_directories(${DEVKITPRO}/libnds/lib)
link_directories(${DEVKITPRO}/libnds/include)
link_directories(${DEVKITPRO}/libnds/include/nds)
link_directories(${DEVKITPRO}/libnds/include/nds/arm9)

add_executable(template-arm9
        source/main.cpp include/main.h
        )
