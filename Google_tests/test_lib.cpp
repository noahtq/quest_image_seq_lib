//
// Created by Noah Turnquist on 7/22/24.
//

#include <fstream>
#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include "../quest_seq_lib.h"

class ImageSeqLibTest : public testing::Test {
protected:
    void SetUp() override {
        dog_seq.open(small_dog_seq_path);
        dog_blurred.open(small_dog_blurred_path);
        output_seq = new Quest::SeqPath(small_dog_output_path);
        new_frame = cv::imread(house_picture_path);
        test_seq = new Quest::SeqPath("small_dog_%04d.png");
    }

    void TearDown() override {
        Quest::SeqPath teardown_output_seq(small_dog_output_path);
        for (int i = 1; i < 188; i++) {
            if (std::ifstream(teardown_output_seq.outputPath())) {
                std::filesystem::remove(teardown_output_seq.outputPath());
            }
            teardown_output_seq.increment();
        }
    }

    std::filesystem::path small_dog_seq_path =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_dog_001_%04d.png";

    std::filesystem::path small_dog_output_path =
    "../../media/test_media/videos/image_sequences/small_dog_001_rendered/small_dog_001_%04d.png";

    std::filesystem::path bad_small_dog_seq_path =
        "../../media/test_media/videos/image_sequences/small_cat_001/small_dog_001_%04d.png";

    std::filesystem::path small_dog_seq_no_framepadding =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_dog_001.png";

    std::filesystem::path small_dog_seq_name_doesnt_exist =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_cat_001_%04d.png";

    std::filesystem::path house_picture_path = "../../media/test_media/images/house_roof.jpg";

    std::filesystem::path small_dog_blurred_path =
        "../../media/test_media/videos/image_sequences/small_dog_001_blurred/small_dog_001_blurred_%04d.png";

    cv::Mat new_frame;

    Quest::ImageSeq dog_seq;
    Quest::ImageSeq dog_blurred;

    Quest::SeqPath *output_seq = nullptr;
    Quest::SeqPath *test_seq = nullptr;
};

// --- ImageSeq Tests ---

// Test the basic getters that are returing simple data types
TEST_F(ImageSeqLibTest, TestImageSeqBasicGetters) {
    ASSERT_EQ(dog_seq.get_input_path(), small_dog_seq_path);
    ASSERT_EQ(dog_seq.get_output_path(), "");
    ASSERT_EQ(dog_seq.get_frame_count(), 187);
    ASSERT_EQ(dog_seq.get_width(), 1080);
    ASSERT_EQ(dog_seq.get_height(), 1920);
}

// Test the frame getter. A bit more complex as it should be returning a copy
// of a CV::Mat but the internal data in that Mat does not get copied.
// So getting a frame, and then making changes to that frame, those changes
// should be propagated to the ImageSeq automatically as both are references to
// the same memory on the heap.
TEST_F(ImageSeqLibTest, TestImageSeqFrameGetter) {
    cv::Mat test_frame = dog_seq.get_frame(10);
    GaussianBlur(test_frame, test_frame, cv::Size(5, 5), 0, 0, cv::BORDER_CONSTANT);
    ASSERT_TRUE((sum(test_frame != dog_seq.get_frame(10)) == cv::Scalar(0, 0, 0, 0)));
}

// Similar to the above test, when we set a frame the internal matrix for the
// cv::Mat should all be referencing the same memory on the heap, so changes made
// to either should affect both
TEST_F(ImageSeqLibTest, TestImageSeqFrameSetter) {
    // Test that the new Mat is assigned correctly to frame 100 in the image sequence
    dog_seq.set_frame(100, new_frame);
    ASSERT_TRUE((sum(new_frame != dog_seq.get_frame(100)) == cv::Scalar(0, 0, 0, 0)));

    // Test that both Mats reference the same memory on the heap
    GaussianBlur(new_frame, new_frame, cv::Size(5, 5), 0, 0, cv::BORDER_CONSTANT);
    ASSERT_TRUE((sum(new_frame != dog_seq.get_frame(100)) == cv::Scalar(0, 0, 0, 0)));
}

TEST_F(ImageSeqLibTest, TestImageSeqDefaultConstructor) {
    const Quest::ImageSeq seq;
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_output_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
    ASSERT_EQ(seq.get_width(), -1);
    ASSERT_EQ(seq.get_height(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodSuccess) {
    Quest::ImageSeq seq;
    ASSERT_TRUE(seq.open(small_dog_seq_path));
    ASSERT_EQ(seq.get_input_path(), small_dog_seq_path);
    ASSERT_EQ(seq.get_frame_count(), 187);
    ASSERT_EQ(dog_seq.get_width(), 1080);
    ASSERT_EQ(dog_seq.get_height(), 1920);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailDirectoryDoesntExist) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(bad_small_dog_seq_path));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
    ASSERT_EQ(seq.get_width(), -1);
    ASSERT_EQ(seq.get_height(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailNoFramePadding) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(small_dog_seq_no_framepadding));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
    ASSERT_EQ(seq.get_width(), -1);
    ASSERT_EQ(seq.get_height(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailFilenameDoesntExist) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(small_dog_seq_name_doesnt_exist));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
    ASSERT_EQ(seq.get_width(), -1);
    ASSERT_EQ(seq.get_height(), -1);;
}

TEST_F(ImageSeqLibTest, TestImageSeqSubscriptOperatorGetSuccess) {
    cv::Mat test_frame = dog_seq[20];
    GaussianBlur(test_frame, test_frame, cv::Size(5, 5), 0, 0, cv::BORDER_CONSTANT);
    ASSERT_TRUE((sum(test_frame != dog_seq.get_frame(20)) == cv::Scalar(0, 0, 0, 0)));
}

TEST_F(ImageSeqLibTest, TestImageSeqSubscriptOperatorAssignmentSuccess) {


    // Test that the new Mat is assigned correctly to frame 100 in the image sequence
    dog_seq.set_frame(50, new_frame);
    ASSERT_TRUE((sum(new_frame != dog_seq.get_frame(50)) == cv::Scalar(0, 0, 0, 0)));

    // Test that both Mats reference the same memory on the heap
    GaussianBlur(new_frame, new_frame, cv::Size(5, 5), 0, 0, cv::BORDER_CONSTANT);
    ASSERT_TRUE((sum(new_frame != dog_seq.get_frame(50)) == cv::Scalar(0, 0, 0, 0)));
}

TEST_F(ImageSeqLibTest, TestImageSeqSubscriptOperatorFailOutOfRange) {
    ASSERT_THROW(dog_seq[-1], std::out_of_range);
    ASSERT_THROW(dog_seq[200], std::out_of_range);
}

TEST_F(ImageSeqLibTest, TestImageSeqRenderSuccess) {
    ASSERT_TRUE(dog_seq.render(small_dog_output_path));
    ASSERT_EQ(dog_seq.get_output_path(), small_dog_output_path);
    for(int i = 1; i < 188; i++) {
        std::ifstream ifs(output_seq->outputPath());
        ASSERT_TRUE(ifs);
        cv::Mat rendered_frame;
        rendered_frame = cv::imread(output_seq->outputPath());
        ASSERT_TRUE((sum(rendered_frame != dog_seq.get_frame(i - 1)) == cv::Scalar(0, 0, 0, 0)));
        output_seq->increment();
    }
}

TEST_F(ImageSeqLibTest, TestImageSeqRenderNoFrames) {
    Quest::ImageSeq empty_seq;
    ASSERT_THROW(empty_seq.render(small_dog_output_path), Quest::SeqException);
}

TEST_F(ImageSeqLibTest, TestImageSeqRenderNonExistentDirectory) {
    ASSERT_FALSE(dog_seq.render("../fake_dir/dog_output_%04d.png"));
}

TEST_F(ImageSeqLibTest, TestImageSeqRenderUnsupportedExtension) {
    ASSERT_FALSE(dog_seq.render("../../media/test_media/videos/image_sequences/small_dog_001/small_cat_001_%04d.obj"));
}

TEST_F(ImageSeqLibTest, TestImageSeqIterators) {
    // Test const iterators
    for (const cv::Mat& frame : dog_seq) {
        ASSERT_EQ(frame.rows, 1920);
    }

    for (cv::Mat& frame : dog_seq) {
        GaussianBlur(frame, frame, cv::Size(25, 25), 0, 0, cv::BORDER_CONSTANT);
    }

    for (int i = 0; i < 187; i++) {
        ASSERT_TRUE((sum(dog_seq[i] != dog_blurred[i]) == cv::Scalar(0, 0, 0, 0)));
    }
}

// --- SeqPath Tests ---
TEST_F(ImageSeqLibTest, TestSeqPathoutputPath) {
    ASSERT_EQ(test_seq->outputPath(), "small_dog_0001.png");
}

TEST_F(ImageSeqLibTest, TestSeqPathConstructorSuccess) {
    std::filesystem::path short_path = "small_dog_001_%04d.png";
    std::filesystem::path full_path = "../dog/cat/small_dog_001_%04d.png";
    std::filesystem::path start_with_padding = "%04d_small_dog_001.png";
    std::filesystem::path padding_middle = "small_%04d_dog_001.png";
    std::filesystem::path short_padding = "small_dog_001_%00d.png";
    std::filesystem::path lots_of_padding = "small_dog_001_%31d.png";

    Quest::SeqPath output_seq(short_path);
    ASSERT_EQ(output_seq.get_input_path(), short_path);
    ASSERT_EQ(output_seq.outputPath(), "small_dog_001_0001.png");

    Quest::SeqPath output_seq_full(full_path);
    ASSERT_EQ(output_seq_full.get_input_path(), full_path);
    ASSERT_EQ(output_seq_full.outputPath(), "../dog/cat/small_dog_001_0001.png");

    Quest::SeqPath output_seq_start(start_with_padding);
    ASSERT_EQ(output_seq_start.get_input_path(), start_with_padding);
    ASSERT_EQ(output_seq_start.outputPath(), "0001_small_dog_001.png");

    Quest::SeqPath output_seq_middle(padding_middle);
    ASSERT_EQ(output_seq_middle.get_input_path(), padding_middle);
    ASSERT_EQ(output_seq_middle.outputPath(), "small_0001_dog_001.png");

    Quest::SeqPath output_seq_short_padding(short_padding);
    ASSERT_EQ(output_seq_short_padding.get_input_path(), short_padding);
    ASSERT_EQ(output_seq_short_padding.outputPath(), "small_dog_001_1.png");

    Quest::SeqPath output_seq_long_padding(lots_of_padding);
    ASSERT_EQ(output_seq_long_padding.get_input_path(), lots_of_padding);
    ASSERT_EQ(output_seq_long_padding.outputPath(), "small_dog_001_0000000000000000000000000000001.png");
}

TEST_F(ImageSeqLibTest, TestSeqPathConstructorNoPadding) {
    ASSERT_THROW(new Quest::SeqPath("small_dog_0001.png"), Quest::SeqException);
}

TEST_F(ImageSeqLibTest, TestSeqPathConstructorMultiplePadding) {
    ASSERT_THROW(new Quest::SeqPath("small_dog_0001_%04d_%04d.png"), Quest::SeqException);
}

TEST_F(ImageSeqLibTest, TestSeqPathIncrement) {
    ASSERT_EQ(test_seq->increment(), 2);
}

TEST_F(ImageSeqLibTest, TestSeqPathoutputIncrement) {
    Quest::SeqPath output_seq_short("small_dog_%00d.png");
    Quest::SeqPath output_seq_long("small_dog_%15d.png");

    for (int i = 1; i < 500; i++) {
        std::stringstream ss_norm;
        std::stringstream ss_short;
        std::stringstream ss_long;

        ss_norm << "small_dog_" << std::setfill('0') << std::setw(4) << i << ".png";
        ss_short << "small_dog_" << i << ".png";
        ss_long << "small_dog_" << std::setfill('0') << std::setw(15) << i << ".png";

        ASSERT_EQ(test_seq->outputIncrement(), ss_norm.str());
        ASSERT_EQ(output_seq_short.outputIncrement(), ss_short.str());
        ASSERT_EQ(output_seq_long.outputIncrement(), ss_long.str());
    }
}