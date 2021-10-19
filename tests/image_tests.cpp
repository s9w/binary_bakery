#include <doctest/doctest.h>

#include <binary_bakery_lib/image.h>
#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_testpaths.h>

using namespace bb;

const abs_file_path test_image_file{ testRoot / "test_images/test_image_rgb.png" };


TEST_CASE("image basics")
{
   const image<3> image_3x3(test_image_file, image_vertical_direction::bottom_to_top);
   CHECK_EQ(image_3x3.m_width, 3);
   CHECK_EQ(image_3x3.m_height, 2);
   CHECK_EQ(image_3x3.get_element_count(), 6);
   CHECK_EQ(image_3x3[0], color{ 255, 0, 0 });
   CHECK_EQ(image_3x3[1], color{ 0, 255, 0 });
   CHECK_EQ(image_3x3[2], color{ 0, 0, 255 });
}


TEST_CASE("image construction")
{
   CHECK_THROWS_AS(image<1>(test_image_file, image_vertical_direction::bottom_to_top), std::exception); // wrong bpp
}


TEST_CASE("flipping")
{
   const image<3> bottom_first = image<3>(test_image_file, image_vertical_direction::bottom_to_top);
   const image<3> top_first = image<3>(test_image_file, image_vertical_direction::top_to_bottom);

   CHECK_EQ(bottom_first[0], color{255, 0, 0});
   CHECK_EQ(bottom_first[3], color{0, 0, 0});

   CHECK_EQ(top_first[0], color{ 0, 0, 0 });
   CHECK_EQ(top_first[3], color{ 255, 0, 0 });
}
