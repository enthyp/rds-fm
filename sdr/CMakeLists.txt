cmake_minimum_required(VERSION 3.14.2)
project(sdr)

include_directories(include)
file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

add_executable(main ${SOURCES})
target_link_libraries(main "rtlsdr" "pthread")