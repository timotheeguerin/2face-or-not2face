cmake_minimum_required(VERSION 2.8.4)

project(facerecognition)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
file(GLOB SOURCE_FILES
    "include/*.h"
    "src/*.cpp"
    "vendor/**/*.h"
    "vendor/**/*.cpp"
)
find_package(OpenCV)
include_directories( ${OpenCV_INCLUDE_DIRS} include)
add_executable(facerecognition ${SOURCE_FILES})
target_link_libraries(facerecognition ${OpenCV_LIBS})