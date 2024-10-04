# Quest Image Sequence Library
Static library for elegantly handling and working with image sequences in C++  
  
By far the most common way to work with videos in the commercial VFX industry is to write and read those videos as 
image sequences.  
  
This library uses the OpenCV Mat object to represent images so it is easy to work with image sequences
but you also retain all of the functionality that is offered by OpenCV.

## Current Version 0.0.3
### Download
[Apple Silicon M2 Static Library - 0.0.3](versions/lib_quest_image_sequence_0_0_3.tar)

## Tech Stack
- C++ and CMake
- [OpenCV](https://opencv.org/)
- [GTest for Unit Testing](https://github.com/google/googletest)
- [Google Benchmark for benchmarking](https://github.com/google/benchmark/tree/main)
- Developed in CLion

## Building and Using Library
Note that in order to run benchmarks you must follow the instructions [here](https://github.com/google/benchmark)
to build the Google_benchmark library for your own system.

## Performance - Last updated for version 0.0.1
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

## Supported Image/Video File Types
### Image
- .png
- .jpg
- .jpeg
- .jpe
- .bmp
- .dib
- .jp2
- .webp
- .sr
- .ras
- .tiff
- .tif
    
*Support for .exr and .dpx images is planned and will be coming in a future update*

### Video
**Note: These are the currently supported video file formats and codecs but please note that if your system does not have the appropriate codec installed or does not support a specific file type, then that option will not work for you.**
- MP4 using H264 Codec
- MOV using H264 Codec
