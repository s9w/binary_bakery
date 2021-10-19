#include <doctest/doctest.h>

#include "test_types.h"
#include "decoding_tools.h"
#include <binary_bakery_testpaths.h>

#include "test_images/test_payload_uncompressed.h"
#define BAKERY_PROVIDE_VECTOR
#include <binary_bakery_decoder.h>

#include <binary_bakery_lib/file_tools.h>


using namespace bb;


namespace tests {

   TEST_CASE("decode_to_vector, image")
   {
  const std::vector<uint8_t> expected =
      get_image_bytes(abs_file_path{testRoot / "test_images/red.png"});
      const std::vector<uint8_t> bytes_from_payload = get_decode_to_vector_result<test_rgb>(get_payload("red.png"), nullptr);
      CHECK_EQ(expected, bytes_from_payload);
   }


   TEST_CASE("decode_to_vector, generic binary")
   {
     const std::vector<uint8_t> expected = get_binary_file(
         abs_file_path{testRoot / "test_images/binary0.bin"});
      const std::vector<uint8_t> bytes_from_payload = decode_to_vector<uint8_t>(get_payload("binary0.bin"), nullptr);
      CHECK_EQ(expected, bytes_from_payload);
   }

   TEST_CASE("decode_into_pointer, image")
   {
     const std::vector<uint8_t> bytes_from_file =
         get_image_bytes(abs_file_path{testRoot / "test_images/red.png"});
      const std::vector<uint8_t> bytes_from_payload = get_decode_into_pointer_result(get_payload("red.png"), nullptr);
      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }


   TEST_CASE("decode_into_pointer, generic binary")
   {
     const std::vector<uint8_t> bytes_from_file = get_binary_file(
         abs_file_path{testRoot / "test_images/binary0.bin"});
      const std::vector<uint8_t> bytes_from_payload = get_decode_into_pointer_result(get_payload("binary0.bin"), nullptr);
      CHECK_EQ(bytes_from_file, bytes_from_payload);
   }

}
