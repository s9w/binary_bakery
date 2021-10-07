#include <doctest/doctest.h>

#include "../binary_inliner_lib/payload.h"

using namespace inliner;

TEST_CASE("meta_to_binary")
{
   constexpr color color0{ 255, 0, 0 };
   constexpr color color1{ 0, 255, 0 };
   constexpr uint16_t width = 20;
   constexpr uint16_t height = 10;
   constexpr uint8_t bpp = 3;
   constexpr uint32_t byte_count = width * height * bpp;
   constexpr dual_image_type dual_meta{width, height, bpp, color0, color1};

   //const std::vector<uint8_t> expected = get_byte_sequence(
   //   2ui8,
   //   bpp,
   //   width,
   //   height,
   //   255ui8, 0ui8, 0ui8,
   //   0ui8, 255ui8, 0ui8,
   //   byte_count
   //);
   //const std::vector<uint8_t> result = meta_and_size_to_binary(payload{ {}, dual_meta }, byte_count);
   //CHECK_EQ(result, expected);
}
