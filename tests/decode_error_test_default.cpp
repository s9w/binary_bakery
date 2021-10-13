#include <doctest/doctest.h>

#include "../binary_bakery_decoder.h"
#include "test_images/test_image_rgb_payload.h"

using namespace bb;

struct user_type {
   uint8_t r{}, g{}, b{};
   user_type() = delete;
   constexpr user_type(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
   auto operator<=>(const user_type&) const = default;
};
constexpr user_type null_obj{ 0, 0, 0 };

TEST_CASE("default compile-time error handling")
{
   // None of these should compile
   //constexpr auto t0 = bb::get_pixel<user_type>(nullptr, 4); // Nullptr source
   //constexpr auto t1 = bb::get_pixel<user_type>(bb_test_image_rgb_png, -1); // Negative index
   //constexpr auto t1 = bb::get_pixel<user_type>(bb_test_image_rgb_png, 6); // Index too big

   // Correct cases
   static_assert(bb::get_pixel<user_type>(bb_test_image_rgb_png, 0) == user_type{255, 0, 0});
   static_assert(bb::get_pixel<user_type>(bb_test_image_rgb_png, 1) == user_type{0, 255, 0});
   static_assert(bb::get_pixel<user_type>(bb_test_image_rgb_png, 2) == user_type{0, 0, 255});
}


TEST_CASE("default run-time error handling")
{
   // Default is ignoring errors and returning defaulted objects
   CHECK_EQ(bb::get_pixel<user_type>(nullptr, 0), null_obj);
   CHECK_EQ(bb::get_pixel<user_type>(bb_test_image_rgb_png, -1), null_obj);
   CHECK_EQ(bb::get_pixel<user_type>(bb_test_image_rgb_png, 99), null_obj);

   // Correct cases
   CHECK_EQ(bb::get_pixel<user_type>(bb_test_image_rgb_png, 0), user_type{ 255, 0, 0 });
   CHECK_EQ(bb::get_pixel<user_type>(bb_test_image_rgb_png, 1), user_type{ 0, 255, 0 });
   CHECK_EQ(bb::get_pixel<user_type>(bb_test_image_rgb_png, 2), user_type{ 0, 0, 255 });
}