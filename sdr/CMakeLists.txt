cmake_minimum_required(VERSION 3.10.2)
project(sdr)

include_directories(include)
file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_executable(main ${SOURCES} src/in/file_source.cpp include/in/file_source.h)
target_link_libraries(main "rtlsdr" "pthread")
