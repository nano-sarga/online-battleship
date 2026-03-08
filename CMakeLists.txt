cmake_minimum_required(VERSION 4.1)
project(online_battleship)

set(CMAKE_CXX_STANDARD 17)

add_executable(online_battleship
        server.cpp
        client.cpp)
