project(Google_tests)

find_package(OPENCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run
        test_lib.cpp
        ../quest_seq_lib.cpp
        ../quest_seq_lib.h
)

target_link_libraries(Google_Tests_run ${OpenCV_LIBS})
target_link_libraries(Google_Tests_run gtest gtest_main)