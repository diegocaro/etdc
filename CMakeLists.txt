cmake_minimum_required(VERSION 2.8.7)

project(etdc C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O3")

include_directories(${etdc_SOURCE_DIR})

add_library(etdc etdc.c)

add_executable(etdc_example etdc_example.c )
target_link_libraries(etdc_example etdc)

add_executable(etdc_uint_example etdc_uint_example.c )
target_link_libraries(etdc_uint_example etdc)

add_executable(etdc_test etdc_test.c )
target_link_libraries(etdc_test etdc)


target_include_directories(etdc PUBLIC .)