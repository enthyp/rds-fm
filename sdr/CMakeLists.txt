cmake_minimum_required(VERSION 3.10.2)
project(sdr)

include_directories(include)
file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_executable(main ${SOURCES} include/ring_buffer.h src/ring_buffer.cpp)
target_link_libraries(main "rtlsdr" "pthread")

