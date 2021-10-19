#include <doctest/doctest.h>

#include "test_types.h"

#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/image.h>
#include <binary_bakery_lib/payload.h>
#include <binary_bakery_lib/config.h>
#include <binary_bakery_testpaths.h>

#define BAKERY_PROVIDE_VECTOR
#include <binary_bakery_decoder.h>


using namespace bb;

namespace {

   auto get_file_roundtrip_bytes(const abs_file_path& source_file)
   {
      const config default_cfg{};
      payload payload = get_payload(source_file, default_cfg);
      const std::vector<uint8_t> bytestream = detail::get_final_bytestream(payload, default_cfg);
      const uint64_t* ptr = reinterpret_cast<const uint64_t*>(bytestream.data());
      return decode_to_vector<uint8_t>(ptr);
   }

} // namespace {}


namespace tests {

   TEST_CASE("png image roundtrip")
   {
      const abs_file_path source_file{testRoot / "test_images/test_image_rgb.png"};
      const image<3> image(source_file, image_vertical_direction::bottom_to_top);
      const std::vector<uint8_t> expected = get_image_bytestream(image);
      const std::vector<uint8_t> result = get_file_roundtrip_bytes(source_file);
      CHECK_EQ(expected, result);
   }

   TEST_CASE("tga image roundtrip")
   {
      const abs_file_path source_file{testRoot / "test_images/tga_image.tga"};
      const image<3> image(source_file, image_vertical_direction::bottom_to_top);
      const std::vector<uint8_t> expected = get_image_bytestream(image);
      const std::vector<uint8_t> result = get_file_roundtrip_bytes(source_file);
      CHECK_EQ(expected, result);
   }

   TEST_CASE("bmp image roundtrip")
   {
      const abs_file_path source_file{testRoot / "test_images/bmp_image.bmp"};
      const image<3> image(source_file, image_vertical_direction::bottom_to_top);
      const std::vector<uint8_t> expected = get_image_bytestream(image);
      const std::vector<uint8_t> result = get_file_roundtrip_bytes(source_file);
      CHECK_EQ(expected, result);
   }

   TEST_CASE("binary roundtrip")
   {
      const abs_file_path source_file{testRoot / "test_images/binary0.bin"};
      const auto expected = get_binary_file(source_file);
      const auto result = get_file_roundtrip_bytes(source_file);
      CHECK_EQ(result, expected);
   }

}
