cmake_minimum_required(VERSION 3.28)
project(quest_image_seq_lib VERSION 0.0.1)

find_package(OPENCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
set(FinalLibraryName "quest_image_seq_${CMAKE_PROJECT_VERSION}") #Append software version number to end of library file
add_library(${FinalLibraryName} STATIC quest_seq_lib.cpp)
target_link_libraries(${FinalLibraryName} ${OpenCV_LIBS})

add_subdirectory(Google_tests)
add_subdirectory(Google_benchmark)
