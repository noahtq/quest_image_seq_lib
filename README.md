# Quest Image Sequence Library
Static library for elegantly handling and working with image sequences in C++  
  
By far the most common way to work with videos in the commercial VFX industry is to write and read those videos as 
image sequences.  
  
This library uses the OpenCV Mat object to represent images so it is easy to work with image sequences
but you also retain all of the functionality that is offered by OpenCV.

## Current Version 0.0.1
### Download
[Apple Silicon M2 Static Library - 0.0.1](versions/libquest_image_seq_0.0.1.a)

## Tech Stack
- C++ and CMake
- [OpenCV](https://opencv.org/)
- [GTest for Unit Testing](https://github.com/google/googletest)
- [Google Benchmark for benchmarking](https://github.com/google/benchmark/tree/main)
- Developed in CLion

## Building and Using Library
Note that in order to run benchmarks you must follow the instructions [here](https://github.com/google/benchmark)
to build the Google_benchmark library for your own system.

## Performance
These performance measurements were made on an M2 Macbook Air using Google Benchmark.  
  
Each benchmark was run with 4 iterations, meaning it was concurrently run on 4 different cores and the time was averaged
between all of the iterations to get the benchmark times.

### Reading an Image Sequence
**Reading a 990 frame 1280x720 png image sequence**  
- Time (for all operations): 3.75 seconds
- CPU Time: 10.2 seconds  

**Rendering a 990 frame 1280x720 png image sequence**
- Time (for all operations): 8.65 seconds
- CPU Time: 32.2 seconds