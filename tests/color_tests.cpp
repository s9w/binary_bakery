#include <doctest/doctest.h>

#include "../binary_inliner_lib/image.h"

using namespace inliner;

TEST_CASE("color type")
{
   color<3> red{255, 0, 0};
   color<3> another_red{255, 0, 0};
   color<3> green{0, 255, 0};
   CHECK_EQ(red, another_red);
   CHECK_NE(red, green);
}


TEST_CASE("image_type")
{
   image<3> image_3x3 = get_image<3>("C:\\Dropbox\\code\\binary_inliner\\tests\\test_image.png");
   CHECK_EQ(image_3x3.m_width, 3);
   CHECK_EQ(image_3x3.m_height, 2);
   CHECK_EQ(image_3x3.get_pixel_count(), 6);
   CHECK_EQ(image_3x3[0], color<3>{0, 0, 0});
   CHECK_EQ(image_3x3[1], color<3>{255, 0, 0});
   CHECK_EQ(image_3x3[5], color<3>{255, 255, 255});
}
