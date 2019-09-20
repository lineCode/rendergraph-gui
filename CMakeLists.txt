cmake_minimum_required(VERSION 3.10)
project(rendergraph_gui)

#==========================================================
# Config
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#==========================================================
# Dependencies
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(ZeroMQ CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenImageIO REQUIRED)		# Does not natively support cmake targets

#==========================================================
# Vendored dependencies
add_subdirectory(ext/fmt-5.3.0)
add_subdirectory(ext/filesystem-master)

#==========================================================
# Sources
file(GLOB SOURCES 
    src/*.cpp
    src/ui/*.cpp 
    src/ui/nodes/*.cpp 
    src/ui/QtAwesome/*.cpp
    src/util/*.cpp
    src/node/*.cpp
    src/img/*.cpp
    src/gfx/*.cpp
    src/gfxopengl/*.cpp
    src/client/*.cpp)

file(GLOB HEADERS 
    src/*.h
    src/ui/*.h 
    src/ui/nodes/*.h 
    src/util/*.h
    src/node/*.h
    src/img/*.h
    src/gfx/*.h
    src/gfxopengl/*.h
    src/ui/QtAwesome/*.h
    src/client/*.h)
set(RESOURCES src/ui/QtAwesome/QtAwesome.qrc)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ FILES ${SOURCES} ${HEADERS} ${RESOURCES})

add_executable(rendergraph_gui ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(rendergraph_gui PRIVATE src/)
target_include_directories(rendergraph_gui PRIVATE ${RAPIDJSON_INCLUDE_DIRS} ${OPENIMAGEIO_INCLUDE_DIR} ext/string-view-lite)
target_link_libraries(rendergraph_gui PRIVATE OpenGL::GL Qt5::Widgets libzmq cppzmq fmt-header-only ghc_filesystem ${OPENIMAGEIO_LIBRARIES})

