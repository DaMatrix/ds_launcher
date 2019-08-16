cmake_minimum_required(VERSION 3.13)

project(template-arm7)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DARM7")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

add_executable(template-arm7 "")

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/source/*.h")
target_sources(template-arm7 PRIVATE ${sources} PUBLIC ${headers})

target_link_libraries(template-arm7 porklib_cpp)
