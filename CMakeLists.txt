cmake_minimum_required(VERSION 4.2)
project(lab3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(lab3
        main.cpp
        server.cpp
        client.cpp
)

target_include_directories(lab3 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(lab3 PRIVATE Boost::system)

if (WIN32)
    target_link_libraries(lab3 PRIVATE ws2_32 mswsock)
endif()
