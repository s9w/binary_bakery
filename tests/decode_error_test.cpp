#include <doctest/doctest.h>

#include "test_types.h"

#include "../tests/test_images/test_payload_rgb.h"
#include <binary_bakery_decoder.h>


using namespace bb;

namespace tests {

   TEST_CASE("default compile-time error handling")
   {
      bb::error_callback = nullptr;

      // None of these should compile
      //constexpr auto t0 = bb::get_element<nc_test_rgb>(nullptr, 4); // Nullptr source
      //constexpr auto t1 = bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, -1); // Negative index
      //constexpr auto t1 = bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 6); // Index too big

      // Correct cases
      static_assert(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 0) == nc_test_rgb{ 255, 0, 0 });
      static_assert(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 1) == nc_test_rgb{ 0, 255, 0 });
      static_assert(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 2) == nc_test_rgb{ 0, 0, 255 });
   }


   TEST_CASE("default run-time error handling")
   {
      bb::error_callback = nullptr;

      // Default is ignoring errors and returning defaulted objects
      CHECK_EQ(bb::get_element<nc_test_rgb>(nullptr, 0), nc_test_rgb_null);
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, -1), nc_test_rgb_null);
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 99), nc_test_rgb_null);

      // Correct cases
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 0), nc_test_rgb{ 255, 0, 0 });
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 1), nc_test_rgb{ 0, 255, 0 });
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb_test_image_rgb_png, 2), nc_test_rgb{ 0, 0, 255 });
   }


   auto my_error_function(const char*, const std::source_location&) -> void
   {
      throw std::exception{};
   }

   TEST_CASE("user-defined compile-time error handling ")
   {
      bb::error_callback = my_error_function;

      // None of these should compile
      //constexpr auto t0 = bb::get_element<nc_test_rgb>(nullptr, 4); // Nullptr source
      //constexpr auto t1 = bb::get_element<nc_test_rgb>(bb::get_payload("red.png"), -1); // Negative index
      //constexpr auto t1 = bb::get_element<nc_test_rgb>(bb::get_payload("red.png"), 6); // Index too big

      // These should be fine
      static_assert(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 0) == nc_test_rgb{ 255, 0, 0 });
      static_assert(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 1) == nc_test_rgb{ 0, 255, 0 });
      static_assert(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 2) == nc_test_rgb{ 0, 0, 255 });
   }


   TEST_CASE("user-defined run-time error handling ")
   {
      bb::error_callback = my_error_function;
      CHECK_THROWS_AS(static_cast<void>(bb::get_element<nc_test_rgb>(nullptr, 0)), std::exception);

      // Correct cases
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 0), nc_test_rgb{ 255, 0, 0 });
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 1), nc_test_rgb{ 0, 255, 0 });
      CHECK_EQ(bb::get_element<nc_test_rgb>(bb::get_payload("test_image_rgb.png"), 2), nc_test_rgb{ 0, 0, 255 });
   }

}