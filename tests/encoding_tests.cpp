#include <fstream>
#include <string>

#include "test_tools.h"
#include "../binary_bakery_lib/encoding.h"

#include <doctest/doctest.h>

using namespace inliner;

namespace
{

}


TEST_CASE("generic binary encoding")
{
   const char* path = "generic_payload.bin";
   const std::vector<uint8_t> byte_sequence = get_byte_sequence(1.12, 4);
   write_binary_file(path, byte_sequence);

   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<generic_binary>(pl.meta));
   CHECK_EQ(pl.m_content_data.size(), 12);
}


TEST_CASE("normal image encoding")
{
   const char* path = "test_image.png";
   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<naive_image_type>(pl.meta));
   const naive_image_type image_meta = std::get<naive_image_type>(pl.meta);
   CHECK_EQ(image_meta.width, 3);
   CHECK_EQ(image_meta.height, 2);
   CHECK_EQ(image_meta.m_bpp, 3);
}


TEST_CASE("dual image encoding")
{
   const char* path = "dual_24bit.png";
   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<dual_image_type<3>>(pl.meta));
   const dual_image_type<3> image_meta = std::get<dual_image_type<3>>(pl.meta);
   CHECK_EQ(image_meta.width, 3);
   CHECK_EQ(image_meta.height, 3);
   CHECK_EQ(image_meta.color0, color(255, 255, 255));
   CHECK_EQ(image_meta.color1, color(255, 0, 0));
}


TEST_CASE("hex strings")
{
   CHECK_EQ(get_ui64_str(0), "0x0000000000000000");
   CHECK_EQ(get_ui64_str(15), "0x000000000000000f");
   CHECK_EQ(get_ui64_str(std::numeric_limits<uint64_t>::max()), "0xffffffffffffffff");
}


TEST_CASE("payload writing")
{
   const payload pl = get_payload("dual_24bit.png");
   write_payload_to_file("dual_24bit.h", "dual_24bit", 3, 120, pl);
}
