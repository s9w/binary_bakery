#include <fstream>
#include <string>

#include "test_tools.h"
#include <binary_bakery_lib/payload.h>
#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/byte_sequencer.h>

#include <doctest/doctest.h>

using namespace bb;

namespace
{

}


TEST_CASE("generic binary encoding")
{
   const fs::path path = "generic_payload.bin";
   const std::vector<uint8_t> byte_sequence = get_byte_sequence(1.12, 4);
   write_binary_file(path, byte_sequence);

   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<generic_binary>(pl.m_meta));
   CHECK_EQ(pl.m_content_data.size(), 12);
}


TEST_CASE("normal image encoding")
{
   const fs::path path = "test_image.png";
   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<naive_image_type>(pl.m_meta));
   const naive_image_type image_meta = std::get<naive_image_type>(pl.m_meta);
   CHECK_EQ(image_meta.m_width, 3);
   CHECK_EQ(image_meta.m_height, 2);
   CHECK_EQ(image_meta.m_bpp, 3);
}


TEST_CASE("dual image encoding")
{
   const fs::path path = "dual_24bit.png";
   const payload pl = get_payload(path);
   CHECK(std::holds_alternative<dual_image_type<3>>(pl.m_meta));
   const dual_image_type<3> image_meta = std::get<dual_image_type<3>>(pl.m_meta);
   CHECK_EQ(image_meta.m_width, 3);
   CHECK_EQ(image_meta.m_height, 3);
   CHECK_EQ(image_meta.m_color0, color(255, 255, 255));
   CHECK_EQ(image_meta.m_color1, color(255, 0, 0));
}


TEST_CASE("hex strings")
{
   CHECK_EQ(get_ui64_str(0), "0x0000000000000000");
   CHECK_EQ(get_ui64_str(15), "0x000000000000000f");
   CHECK_EQ(get_ui64_str(std::numeric_limits<uint64_t>::max()), "0xffffffffffffffff");
}


TEST_CASE("payload writing")
{
   payload pl = get_payload("rgb_example.png");
   const config default_cfg{};
   write_payload_to_file(default_cfg, std::move(pl));
}


TEST_CASE("get_content_bit_count()")
{
   SUBCASE("dual dual_image_type") {
      constexpr dual_image_type<2> meta{ 3, 2, color<2>::black(), color<2>::black() };
      const std::vector<uint8_t> stream(33);
      CHECK(detail::get_content_bit_count(meta, stream).m_value == 6);
   }
   SUBCASE("naive_image_type") {
      constexpr naive_image_type meta{ 5, 4, 2 };
      const std::vector<uint8_t> stream(33);
      CHECK(detail::get_content_bit_count(meta, stream).m_value == 33*8);
   }
   SUBCASE("generic_binary") {
      constexpr generic_binary meta{};
      const std::vector<uint8_t> stream(33);
      CHECK(detail::get_content_bit_count(meta, stream).m_value == 33 * 8);
   }
}
