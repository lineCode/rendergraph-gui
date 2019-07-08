cmake_minimum_required(VERSION 3.0)
project(rendergraph_gui)

#==========================================================
# Config
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

set(CMAKE_AUTOMOC ON)

#==========================================================
# Dependencies
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(msgpack CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)

#==========================================================
# Sources
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

add_executable(rendergraph_gui ${SOURCES} ${HEADERS})
target_link_libraries(rendergraph_gui PRIVATE Qt5::Widgets libzmq msgpackc-cxx cppzmq)
