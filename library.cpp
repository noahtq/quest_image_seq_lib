#include <iostream>
#include "library.h"

bool Quest::ImageSeq::open(const std::filesystem::path& new_input_path) {
    cv::VideoCapture input_video;
    input_video.open(new_input_path, cv::CAP_IMAGES);
    if (!input_video.isOpened()) {
        return false;
    }
    input_path = new_input_path;
    frames.resize(static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_COUNT)));
    // Send all of the frames in the video writer to a vector of matrices.
    int i;
    for(i = 0; i < input_video.get(cv::CAP_PROP_FRAME_COUNT); i++) {
        input_video >> frames[i];
    }
    frame_count = i;
    return true;
}

cv::Mat& Quest::ImageSeq::operator[](const int& index) {
    if (index >= frames.size() || index < 0) {
        throw std::out_of_range("Attempting to access a frame in ImageSeq object that doesn't exist");
    }
    return frames[index];
}


