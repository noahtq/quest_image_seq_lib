//
// Created by Noah Turnquist on 7/22/24.
//

#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include "../library.h"

class ImageSeqLibTest : public testing::Test {
protected:
    void SetUp() override {
        dog_seq.open(small_dog_seq_path);
    }

    std::filesystem::path small_dog_seq_path =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_dog_001_%04d.png";

    std::filesystem::path bad_small_dog_seq_path =
        "../../media/test_media/videos/image_sequences/small_cat_001/small_dog_001_%04d.png";

    std::filesystem::path small_dog_seq_no_framepadding =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_dog_001.png";

    std::filesystem::path small_dog_seq_name_doesnt_exist =
        "../../media/test_media/videos/image_sequences/small_dog_001/small_cat_001_%04d.png";

    Quest::ImageSeq dog_seq;
};

// --- ImageSeq Tests ---

// Test the basic getters that are returing simple data types
TEST_F(ImageSeqLibTest, TestImageSeqBasicGetters) {
    ASSERT_EQ(dog_seq.get_input_path(), small_dog_seq_path);
    ASSERT_EQ(dog_seq.get_output_path(), "");
    ASSERT_EQ(dog_seq.get_frame_count(), 187);
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
    cv::Mat new_frame = cv::imread("../../media/test_media/images/house_roof.jpg");

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
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodSuccess) {
    Quest::ImageSeq seq;
    ASSERT_TRUE(seq.open(small_dog_seq_path));
    ASSERT_EQ(seq.get_input_path(), small_dog_seq_path);
    ASSERT_EQ(seq.get_frame_count(), 187);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailDirectoryDoesntExist) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(bad_small_dog_seq_path));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailNoFramePadding) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(small_dog_seq_no_framepadding));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqOpenMethodFailFilenameDoesntExist) {
    Quest::ImageSeq seq;
    ASSERT_FALSE(seq.open(small_dog_seq_name_doesnt_exist));
    ASSERT_EQ(seq.get_input_path(), "");
    ASSERT_EQ(seq.get_frame_count(), -1);
}

TEST_F(ImageSeqLibTest, TestImageSeqSubscriptOperatorGetSuccess) {
    cv::Mat test_frame = dog_seq[20];
    GaussianBlur(test_frame, test_frame, cv::Size(5, 5), 0, 0, cv::BORDER_CONSTANT);
    ASSERT_TRUE((sum(test_frame != dog_seq.get_frame(20)) == cv::Scalar(0, 0, 0, 0)));
}

TEST_F(ImageSeqLibTest, TestImageSeqSubscriptOperatorAssignmentSuccess) {
    cv::Mat new_frame = cv::imread("../../media/test_media/images/house_roof.jpg");

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

// --- SeqPath Tests ---
TEST_F(ImageSeqLibTest, TestSeqPathoutputPath) {
    const Quest::SeqPath output_seq("small_dog_%04d.jpg");
    ASSERT_EQ(output_seq.outputPath(), "small_dog_0001.jpg");
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

TEST_F(ImageSeqLibTest, TestSeqPathoutputIncrement) {
    Quest::SeqPath output_seq("small_dog_%04d.png");
    Quest::SeqPath output_seq_short("small_dog_%00d.png");
    Quest::SeqPath output_seq_long("small_dog_%15d.png");

    for (int i = 1; i < 500; i++) {
        std::stringstream ss_norm;
        std::stringstream ss_short;
        std::stringstream ss_long;

        ss_norm << "small_dog_" << std::setfill('0') << std::setw(4) << i << ".png";
        ss_short << "small_dog_" << i << ".png";
        ss_long << "small_dog_" << std::setfill('0') << std::setw(15) << i << ".png";

        ASSERT_EQ(output_seq.outputIncrement(), ss_norm.str());
        ASSERT_EQ(output_seq_short.outputIncrement(), ss_short.str());
        ASSERT_EQ(output_seq_long.outputIncrement(), ss_long.str());
    }
}