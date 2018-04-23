cmake_minimum_required(VERSION 3.10)
project(league_of_rockets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -O3")

set(SOURCE_FILES league_of_rockets.cpp gui.h sockets.h league_of_rockets_lib.h)

add_executable(league_of_rockets ${SOURCE_FILES})
target_link_libraries(league_of_rockets sfml-graphics sfml-window sfml-system)
