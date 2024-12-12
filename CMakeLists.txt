cmake_minimum_required(VERSION 3.8)

project(inf3173-243-tp3 LANGUAGES C CXX VERSION 1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Catch2 3 REQUIRED)

enable_testing()
include(CTest)

configure_file(env.sh.in env.sh)
configure_file(inf3173.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/inf3173.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)

add_custom_target(README SOURCES README.md)

add_custom_target(remise
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/remise.py"
        "${CMAKE_PROJECT_NAME}"
        --destdir="${CMAKE_CURRENT_BINARY_DIR}"
)

