#ifndef QUEST_IMAGE_SEQ_LIB_LIBRARY_H
#define QUEST_IMAGE_SEQ_LIB_LIBRARY_H

#include <filesystem>
#include <opencv2/opencv.hpp>

namespace Quest {
    class ImageSeq {
        std::filesystem::path input_path = "";
        std::filesystem::path output_path = "";
        std::vector<cv::Mat> frames;
        int frame_count = -1;

    public:
        // Constructors
        ImageSeq() = default;

        // Getters and setters
        [[nodiscard]] std::filesystem::path get_input_path() const { return input_path; }
        [[nodiscard]] std::filesystem::path get_output_path() const { return output_path; }
        [[nodiscard]] int get_frame_count() const { return frame_count; }
        [[nodiscard]] cv::Mat get_frame(const int& i) const { return frames[i]; }
        void set_frame(const int& i, const cv::Mat& new_frame) { frames[i] = new_frame; }

        // Image IO
        bool open(const std::filesystem::path& new_input_path);
    };
}

#endif //QUEST_IMAGE_SEQ_LIB_LIBRARY_H
