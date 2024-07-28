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
