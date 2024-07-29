cmake_minimum_required(VERSION 3.28)
project(quest_image_seq_lib)

find_package(OPENCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

add_library(quest_image_seq_lib STATIC library.cpp)
target_link_libraries(quest_image_seq_lib ${OpenCV_LIBS})

add_subdirectory(Google_tests)
add_subdirectory(Google_benchmark)
