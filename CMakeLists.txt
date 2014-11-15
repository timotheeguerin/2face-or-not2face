cmake_minimum_required(VERSION 2.8.4)

project(2face)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SOURCE_FILES
    "include/*.h"
    "src/*.cpp"
    main.cpp
)
set(QT_DEPENDENCIES Core)
get_filename_component(EXAMPLE_BASENAME main.cpp NAME_WE)

find_package(Qt5Core)
find_package(openBR)
find_package(OpenCV)
include_directories( ${OpenCV_INCLUDE_DIRS} include )
add_executable(2face ${SOURCE_FILES})
qt5_use_modules(2face ${QT_DEPENDENCIES})
target_link_libraries(2face openbr ${OpenCV_LIBS} Qt5::Core)