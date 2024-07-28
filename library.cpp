#include <iostream>
#include <regex>
#include "library.h"

Quest::SeqPath::SeqPath(const std::filesystem::path& new_input_path) {
    std::string path_string = new_input_path;
    const std::regex padding_pattern(R"(%\d\dd)");
    std::smatch matches, matches_2;
    if (std::regex_search(path_string, matches, padding_pattern)) {
        std::string suffix_string = matches.suffix();
        if (std::regex_search(suffix_string, matches_2, padding_pattern)) {
            throw SeqException("More than one frame padding pattern specified");
        }
        input_path = new_input_path;
        pre_frame = matches.prefix();
        current_frame = 1;
        post_frame = matches.suffix();

        std::string padding_str = matches[0];
        padding = std::stoi(matches[0].str().substr(1, 2));
    } else {
        throw SeqException("No frame padding set in input path");
    }
}

std::string Quest::SeqPath::outputPath() const {
    std::stringstream ss;
    ss << pre_frame << std::setfill('0') << std::setw(padding) << current_frame << post_frame;
    return ss.str();
}

std::string Quest::SeqPath::outputIncrement() {
    std::string output = outputPath();
    ++current_frame;
    return output;
}

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

bool Quest::ImageSeq::render(const std::filesystem::path& new_output_path) {
    return false;
}

cv::Mat& Quest::ImageSeq::operator[](const int& index) {
    if (index >= frames.size() || index < 0) {
        throw std::out_of_range("Attempting to access a frame in ImageSeq object that doesn't exist");
    }
    return frames[index];
}
