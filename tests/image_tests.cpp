#include <doctest/doctest.h>

#include <binary_bakery_lib/image.h>

using namespace bb;

TEST_CASE("image basics")
{
   image<3> image_3x3 = get_image<3>("test_images/test_image_rgb.png");
   CHECK_EQ(image_3x3.m_width, 3);
   CHECK_EQ(image_3x3.m_height, 2);
   CHECK_EQ(image_3x3.get_pixel_count(), 6);
   CHECK_EQ(image_3x3[0], color{ 255, 0, 0 });
   CHECK_EQ(image_3x3[1], color{ 0, 255, 0 });
   CHECK_EQ(image_3x3[2], color{ 0, 0, 255 });
}


TEST_CASE("image construction")
{
   CHECK_THROWS_AS(get_image<1>("test_images/test_image_rgb.png"), std::exception); // wrong bpp
}
