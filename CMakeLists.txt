cmake_minimum_required(VERSION 3.0)
project(rendergraph_gui)

#==========================================================
# Config
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#==========================================================
# Dependencies
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(msgpack CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)

#==========================================================
# Sources
file(GLOB SOURCES 
    src/*.cpp
    src/ui/*.cpp 
    src/ui/QtAwesome/*.cpp
    src/client/*.cpp)
file(GLOB HEADERS 
    src/*.hpp
    src/ui/*.hpp 
    src/ui/QtAwesome/*.hpp
    src/client/*.hpp)
set(RESOURCES src/ui/QtAwesome/QtAwesome.qrc)

add_executable(rendergraph_gui ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(rendergraph_gui PRIVATE src/)
target_link_libraries(rendergraph_gui PRIVATE Qt5::Widgets libzmq msgpackc-cxx cppzmq)
