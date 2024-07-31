project(Google_benchmark)

find_package(OPENCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(benchmark)

add_executable(Google_benchmark_run
        benchmark_lib.cpp
        ../library.cpp
        ../library.h
)
target_link_libraries(Google_benchmark_run ${OpenCV_LIBS})
target_link_libraries(Google_benchmark_run benchmark::benchmark)