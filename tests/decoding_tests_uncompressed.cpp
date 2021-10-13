#include <doctest/doctest.h>

#include "test_types.h"

#define BAKERY_PROVIDE_VECTOR
#include "../binary_bakery_decoder.h"
#include "test_images/test_payload_uncompressed.h"

#include <binary_bakery_lib/image.h>
#include <binary_bakery_lib/file_tools.h>

using namespace bb;


namespace
{

   auto get_image_bytes(const abs_file_path& file) -> std::vector<uint8_t>{
      image<3> im{ file, image_vertical_direction::bottom_to_top };
      return get_image_bytestream(im);
   }

} // namespace {}


namespace tests {

   TEST_CASE("decode_to_vector, image")
   {
      const abs_file_path test_image_path{ "test_images/test_image_rgb.png" };
      const std::vector<uint8_t> bytes_from_file = get_image_bytes(test_image_path);

      const std::vector<test_rgb> decoded = decode_to_vector<test_rgb>("test_image_rgb.png");
      const size_t byte_count = decoded.size() * sizeof(test_rgb);
      std::vector<uint8_t> bytes_from_payload(byte_count);
      std::memcpy(bytes_from_payload.data(), decoded.data(), byte_count);

      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }


   TEST_CASE("decode_to_vector, generic binary")
   {
      const abs_file_path test_image_path{ "test_images/binary0.bin" };
      const std::vector<uint8_t> bytes_from_file = get_binary_file(test_image_path);

      const std::vector<uint8_t> bytes_from_payload = decode_to_vector<uint8_t>("binary0.bin");

      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }


   TEST_CASE("decode_into_pointer, image")
   {
      const abs_file_path test_file_path{ "test_images/test_image_rgb.png" };
      const std::vector<uint8_t> bytes_from_file = get_image_bytes(test_file_path);

      const auto header = get_header("test_image_rgb.png");
      const std::vector<uint8_t> bytes_from_payload(header.decompressed_size);
      decode_into_pointer("test_image_rgb.png", (void*)(bytes_from_payload.data()));

      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }


   TEST_CASE("decode_into_pointer, generic binary")
   {
      const abs_file_path test_file_path{ "test_images/binary0.bin" };
      const std::vector<uint8_t> bytes_from_file = get_binary_file(test_file_path);

      const auto header = get_header("binary0.bin");
      const std::vector<uint8_t> bytes_from_payload(header.decompressed_size);
      decode_into_pointer("binary0.bin", (void*)(bytes_from_payload.data()));

      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }

}
