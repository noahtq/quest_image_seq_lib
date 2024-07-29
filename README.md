# Quest Image Sequence Library
Static library for elegantly handling and working with image sequences in C++  
  
By far the most common way to work with videos in the commercial VFX industry is to write and read those videos as 
image sequences.  
  
This library uses the OpenCV Mat object to represent images so it is easy to work with image sequences
but you also retain all of the functionality that is offered by OpenCV.

## Current Version 0.0.1

## Tech Stack
- C++ and CMake
- [OpenCV](https://opencv.org/)
- [GTest for Unit Testing](https://github.com/google/googletest)
- Developed in CLion

## Building and Using Library
Note that in order to run benchmarks you must follow the instructions [here](https://github.com/google/benchmark)
to build the Google_benchmark library for your own system.