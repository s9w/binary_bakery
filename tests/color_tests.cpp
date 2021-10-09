#include <doctest/doctest.h>

#include <binary_bakery_lib/image.h>

using namespace bb;

TEST_CASE("color type")
{
   color red{255, 0, 0};
   color another_red{255, 0, 0};
   color green{0, 255, 0};
   CHECK_EQ(red, another_red);
   CHECK_NE(red, green);
}


// TODO move this
TEST_CASE("image_type")
{
   image<3> image_3x3 = get_image<3>("test_image.png");
   CHECK_EQ(image_3x3.m_width, 3);
   CHECK_EQ(image_3x3.m_height, 2);
   CHECK_EQ(image_3x3.get_pixel_count(), 6);
   CHECK_EQ(image_3x3[0], color{0, 0, 0});
   CHECK_EQ(image_3x3[1], color{255, 0, 0});
   CHECK_EQ(image_3x3[5], color{255, 255, 255});
}


TEST_CASE("get_4byte_padded()")
{
   CHECK_EQ(color{ 1ui8, 2ui8, 3ui8, 4ui8 }.get_4byte_padded(), color{ 1ui8, 2ui8, 3ui8, 4ui8 });
   CHECK_EQ(color{ 1ui8, 2ui8, 3ui8       }.get_4byte_padded(), color{ 1ui8, 2ui8, 3ui8, 0ui8 });
   CHECK_EQ(color{ 1ui8, 2ui8             }.get_4byte_padded(), color{ 1ui8, 2ui8, 0ui8, 0ui8 });
}
