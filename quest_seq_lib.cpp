#include <iostream>
#include <regex>
#include "quest_seq_lib.h"

bool Quest::HasFramePadding(const std::filesystem::path& file_path) {
    std::string path_string = file_path;
    const std::regex padding_pattern(R"(%\d\dd)");
    std::smatch matches, matches_2;
    if (std::regex_search(path_string, matches, padding_pattern)) {
        std::string suffix_string = matches.suffix();
        if (std::regex_search(suffix_string, matches_2, padding_pattern)) {
            return false;
        }
        return true;
    }
    return false;
}

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

Quest::ImageSeq::ImageSeq(const ImageSeq& original) {
    Copy(original, *this);
}

Quest::SeqErrorCodes Quest::ImageSeq::open(const std::filesystem::path& new_input_path) {
    enum class InputTypes { ImageNoPadding, ImagePadding, ImageSequence, Video, Unsupported };
    const std::string extension = new_input_path.extension();
    cv::VideoCapture input_video;
    InputTypes type = InputTypes::Unsupported;

    // Determine type of input - starting with images (image sequence, singular image, singular image with frame padding)
    // Also return as BadPath if the file can't be found
    for (const std::string& image_extension : Quest::supported_image_extensions) {
        if (extension == image_extension) {
            // IMAGE SEQUENCE
            if (Quest::HasFramePadding(new_input_path)) {
                input_video.open(new_input_path, cv::CAP_IMAGES);
                if (!input_video.isOpened()) {
                    return SeqErrorCodes::BadPath;
                }
                frame_count = static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_COUNT));

                // Handling edge case of image sequence with just one frame
                if (frame_count == 1) {
                    type = InputTypes::ImagePadding;
                } else {
                    type = InputTypes::ImageSequence;
                }
            } else {
                type = InputTypes::ImageNoPadding;
            }
        }
    }

    // Check if it's a video container
    for (const std::string& video_extension : Quest::supported_video_extensions) {
        if (extension == video_extension) {
            input_video.open(new_input_path);
            if (!input_video.isOpened()) {
                return SeqErrorCodes::BadPath;
            }
            frame_count = static_cast<int>(input_video.get(cv::CAP_PROP_FRAME_COUNT));
            type = InputTypes::Video;
        }
    }

    // Handle each type or return as unsupported if type can be determined
    switch (type) {
    case InputTypes::ImageNoPadding: {
        // SINGULAR IMAGE - NO FRAME PADDING
        cv::Mat img = cv::imread(new_input_path);
        if (img.empty()) {
            return SeqErrorCodes::BadPath;
        }
        frame_count = 1;
        GiveMatPureWhiteAlpha(img);
        frames.push_back(img);
    } break;
    case InputTypes::ImagePadding: {
        const SeqPath input_seq(new_input_path);
        frames.push_back(cv::imread(input_seq.outputPath()));
        GiveMatPureWhiteAlpha(frames[0]);
    } break;
    case InputTypes::ImageSequence: case InputTypes::Video: {
        frames.resize(frame_count);
        for(int i = 0; i < input_video.get(cv::CAP_PROP_FRAME_COUNT); i++) {
            input_video >> frames[i];
            if (frames[i].rows > 0 && frames[i].cols > 0) GiveMatPureWhiteAlpha(frames[i]);
        }
    } break;
    default:
        return Quest::SeqErrorCodes::UnsupportedExtension;
    }

    if (type == InputTypes::Video) {
        fps = cv::CAP_PROP_FPS;
    }

    input_path = new_input_path;
    width = frames[0].cols;
    height = frames[0].rows;

    return Quest::SeqErrorCodes::Success;
}

Quest::SeqErrorCodes Quest::ImageSeq::render(const std::filesystem::path& new_output_path) {
    if (frames.empty()) {
        throw SeqException("Attempting to render image sequence before images have been opened.");
    }

    if (!is_directory(new_output_path.parent_path())) {
        return SeqErrorCodes::BadPath;
    }

    const std::string extension = new_output_path.extension();
    for (const std::string& valid : supported_image_extensions) {
        if (extension == valid) {
            Quest::SeqPath output_seq(new_output_path);
            output_path = new_output_path;
            for (const cv::Mat& frame : frames) {
                std::filesystem::path frame_output_path = output_seq.outputIncrement();
                cv::imwrite(frame_output_path, frame);
            }
            return SeqErrorCodes::Success;
        }
    }

    for (const std::string& video_extension : supported_video_extensions) {
        if (extension == video_extension) {
            cv::Size frame_size(width, height);
            const int render_fps = fps == -1 ? default_fps : fps;
            std::string codec;
            if (extension == ".mp4" || extension == ".mov") codec = "H264";

            cv::VideoWriter output_writer(new_output_path,
                cv::VideoWriter::fourcc(codec[0], codec[1], codec[2], codec[3]),
                render_fps, frame_size);

            for (const cv::Mat& frame : frames) {
                output_writer.write(frame);
            }

            output_path = new_output_path;
            return SeqErrorCodes::Success;
        }
    }

    return SeqErrorCodes::UnsupportedExtension;
}

cv::Mat& Quest::ImageSeq::operator[](const int& index) {
    if (index >= frames.size() || index < 0) {
        throw std::out_of_range("Attempting to access a frame in ImageSeq object that doesn't exist");
    }
    return frames[index];
}

Quest::ImageSeq& Quest::ImageSeq::operator=(const ImageSeq& original) {
    Copy(original, *this);
    return *this;
}

void Quest::Copy(const ImageSeq& original, ImageSeq& copy) {
    copy.input_path = original.input_path;
    copy.output_path = original.output_path;
    copy.frame_count = original.frame_count;
    copy.width = original.width;
    copy.height = original.height;

    copy.frames.resize(original.frame_count);
    for (int i = 0; i < original.frame_count; i++) {
        original.frames[i].copyTo(copy.frames[i]);
    }
}

Quest::Proxy::Proxy(const ImageSeq& original, const double resize_scale) {
    if (resize_scale <= 0 || resize_scale > 1) {
        throw SeqException("Proxy Sequences must have a resize scale of between 0 and 1");
    }

    input_path = original.get_input_path();
    output_path = "";
    scale = resize_scale;
    frame_count = original.get_frame_count();
    for (const auto& frame : original) {
        cv::Mat resized_frame;
        cv::resize(frame, resized_frame, cv::Size(), resize_scale, resize_scale, cv::INTER_AREA);
        frames.push_back(resized_frame);
    }
    width = frames[0].cols;
    height = frames[0].rows;
}

bool Quest::MatEquals(const cv::Mat& mat_1, const cv::Mat& mat_2) {
    if ((mat_1.type() != 16 && mat_1.type() != 24) || (mat_2.type() != 16 && mat_2.type() != 24)) {
        throw SeqException("This function only supports CV Mat types of CV_8UC3 (default Mat type) or CV_8UC4");
    }

    // Return false if one mat has an alpha and the other does not
    if (mat_1.type() != mat_2.type()) return false;

    if (mat_1.rows != mat_2.rows || mat_1.cols != mat_2.cols) return false;

    switch (mat_1.type()) {
    case 16: {
        cv::MatConstIterator_<cv::Vec3b> mat_1_it, mat_2_it, mat_1_end;
        for (mat_1_it = mat_1.begin<cv::Vec3b>(), mat_2_it = mat_2.begin<cv::Vec3b>(),
            mat_1_end = mat_1.end<cv::Vec3b>(); mat_1_it < mat_1_end;
            ++mat_1_it, ++mat_2_it) {
            for (int ch = 0; ch < 3; ch++) {
                if ((*mat_1_it)[ch] != (*mat_2_it)[ch]) return false;
            }
            }
        break;
    }
    case 24: {
        cv::MatConstIterator_<cv::Vec4b> mat_1_it, mat_2_it, mat_1_end;
        for (mat_1_it = mat_1.begin<cv::Vec4b>(), mat_2_it = mat_2.begin<cv::Vec4b>(),
            mat_1_end = mat_1.end<cv::Vec4b>(); mat_1_it < mat_1_end;
            ++mat_1_it, ++mat_2_it) {
            for (int ch = 0; ch < 4; ch++) {
                if ((*mat_1_it)[ch] != (*mat_2_it)[ch]) return false;
            }
            }
        break;
    }
    default:
        throw SeqException("Something went wrong");
    }
    return true;
}

// Image Seq Equality Operators Compares the Frames Only
bool Quest::operator==(const ImageSeq& seq_1, const ImageSeq& seq_2) {
    if (seq_1.get_frame_count() != seq_2.get_frame_count()) return false;
    if (seq_1.get_height() != seq_2.get_height() || seq_1.get_width() != seq_2.get_width()) return false;
    for (int i = 0; i < seq_1.get_frame_count(); i++) {
        if (Quest::MatNotEquals(seq_1.get_frame(i), seq_2.get_frame(i))) return false;
    }
    return true;
}

void Quest::GiveMatAlpha(cv::Mat& image, const int& alpha_val) {
    if (image.type() != 16 && image.type() != 24) {
        throw SeqException("This function only supports CV Mat types of CV_8UC3 (default Mat type) or CV_8UC4");
    }
    if (image.rows <= 0 || image.cols <= 0) {
        throw SeqException("The Mat must have dimensions greater than 0 x 0");
    }

    if (alpha_val < 0 || alpha_val > 255) {
        throw SeqException("Alpha value must be between 0 and 255");
    }

    const cv::Size frame_size = cv::Size(image.cols, image.rows);
    const cv::Mat pure_white(frame_size, CV_8UC1, cv::Scalar(alpha_val));
    image.convertTo(image, CV_8UC4);
    cv::Mat channels[4];
    cv::split(image, channels);
    pure_white.copyTo(channels[3]);
    cv::merge(channels, 4, image);
}

void Quest::GiveMatPureWhiteAlpha(cv::Mat& image) {
    GiveMatAlpha(image, 255);
}

void Quest::GiveMatPureBlackAlpha(cv::Mat& image) {
    GiveMatAlpha(image, 0);
}

