#include <doctest/doctest.h>

#include "test_types.h"

#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/image.h>
#include <binary_bakery_lib/payload.h>
#include <binary_bakery_lib/config.h>

#define BAKERY_PROVIDE_VECTOR
#include "../binary_bakery_decoder.h"


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

   TEST_CASE("image roundtrip")
   {
      const abs_file_path source_file{ "test_images/test_image_rgb.png" };
      const image<3> image(source_file, image_vertical_direction::bottom_to_top);
      CHECK_EQ(get_image_bytestream(image), get_file_roundtrip_bytes(source_file));
   }

   TEST_CASE("image roundtrip")
   {
      const abs_file_path source_file{ "test_images/binary0.bin" };
      CHECK_EQ(get_binary_file(source_file), get_file_roundtrip_bytes(source_file));
   }

}
