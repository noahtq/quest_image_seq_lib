#ifndef QUEST_IMAGE_SEQ_LIB_LIBRARY_H
#define QUEST_IMAGE_SEQ_LIB_LIBRARY_H

#include <filesystem>
#include <opencv2/opencv.hpp>

namespace Quest {
    inline const std::vector<std::string> supported_image_extensions = {
        ".png", ".jpg", ".jpeg", ".jpe", ".bmp", ".dib", ".jp2",
        ".webp", ".ppm", ".pnm", ".sr", ".ras",
        ".tiff", ".tif", ".hdr", ".pic"
    };

    enum class SeqErrorCodes {Success = 0, BadPath, UnsupportedExtension};

    class SeqException: public std::exception {
        std::string message;
    public:
        explicit SeqException(const char* msg)
            : message(msg) {}

        [[nodiscard]] const char* what() const throw() {
            return message.c_str();
        }
    };

    class SeqPath {
        std::filesystem::path input_path;
        std::string pre_frame;
        int current_frame = -1;
        std::string post_frame;
        int padding = -1;

    public:
        // Constructors
        explicit SeqPath(const std::filesystem::path& new_input_path);

        // Getters and setters
        [[nodiscard]] std::filesystem::path get_input_path() const { return input_path; }

        // Methods
        [[nodiscard]] std::string outputPath() const;
        int increment() { return ++current_frame; }
        std::string outputIncrement();
    };

    class ImageSeq {
    protected:
        std::filesystem::path input_path = "";
        std::filesystem::path output_path = "";
        std::vector<cv::Mat> frames;
        int frame_count = -1;
        int width = -1;
        int height = -1;

    public:
        // Constructors
        ImageSeq() = default;
        ImageSeq(const ImageSeq& original); // Copy constructor

        // Getters and setters
        [[nodiscard]] std::filesystem::path get_input_path() const { return input_path; }
        [[nodiscard]] std::filesystem::path get_output_path() const { return output_path; }
        [[nodiscard]] int get_frame_count() const { return frame_count; }
        [[nodiscard]] cv::Mat get_frame(const int& i) const { return frames[i]; }
        void set_frame(const int& i, const cv::Mat& new_frame) { frames[i] = new_frame; }
        [[nodiscard]] int get_width() const { return width; }
        [[nodiscard]] int get_height() const { return height; }

        // Iterators
        std::vector<cv::Mat>::iterator begin() { return frames.begin(); }
        std::vector<cv::Mat>::iterator end() { return frames.end(); }
        [[nodiscard]] std::vector<cv::Mat>::const_iterator begin() const { return frames.begin(); }
        [[nodiscard]] std::vector<cv::Mat>::const_iterator end() const { return frames.end(); }

        // Operators
        cv::Mat& operator[](const int& index);
        ImageSeq& operator=(const ImageSeq& original);

        // Image IO
        Quest::SeqErrorCodes open(const std::filesystem::path& new_input_path);
        Quest::SeqErrorCodes render(const std::filesystem::path& new_output_path);

        // Friend Functions
        friend void Copy(const ImageSeq& original, ImageSeq& copy);
    };

    class Proxy : public ImageSeq {
        double scale;
    public:
        explicit Proxy(const ImageSeq& original, double resize_scale = 0.5);
    };

    // Equality Operators
    bool operator==(const ImageSeq& seq_1, const ImageSeq& seq_2);
    inline bool operator!=(const ImageSeq& seq_1, const ImageSeq& seq_2) { return !(seq_1 == seq_2); }

    // Helper functions
    void GiveMatPureWhiteAlpha(cv::Mat& image);
}

#endif //QUEST_IMAGE_SEQ_LIB_LIBRARY_H
