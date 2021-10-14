#include <doctest/doctest.h>

#include "test_types.h"

#include "../tests/test_images/test_payload_rgb.h"
#include "../binary_bakery_decoder.h"


using namespace bb;

namespace tests {

   TEST_CASE("default compile-time error handling")
   {
      // None of these should compile
      //constexpr auto t0 = bb::get_pixel<nc_test_rgb>(nullptr, 4); // Nullptr source
      //constexpr auto t1 = bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, -1); // Negative index
      //constexpr auto t1 = bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 6); // Index too big

      // Correct cases
      static_assert(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 0) == nc_test_rgb{ 255, 0, 0 });
      static_assert(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 1) == nc_test_rgb{ 0, 255, 0 });
      static_assert(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 2) == nc_test_rgb{ 0, 0, 255 });
   }


   TEST_CASE("default run-time error handling")
   {
      // Default is ignoring errors and returning defaulted objects
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(nullptr, 0), nc_test_rgb_null);
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, -1), nc_test_rgb_null);
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 99), nc_test_rgb_null);

      // Correct cases
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 0), nc_test_rgb{ 255, 0, 0 });
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 1), nc_test_rgb{ 0, 255, 0 });
      CHECK_EQ(bb::get_pixel<nc_test_rgb>(bb_test_image_rgb_png, 2), nc_test_rgb{ 0, 0, 255 });
   }

}