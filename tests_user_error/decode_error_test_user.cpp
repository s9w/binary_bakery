#include <doctest/doctest.h>

#include "../binary_bakery_decoder.h"

#include "../tests/test_images/test_payload_rgb.h"

using namespace bb;


auto my_error_function(const char*, const std::source_location&) -> void
{
   throw std::exception{};
}

struct user_type {
   uint8_t r{}, g{}, b{};
   user_type() = delete;
   constexpr user_type(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
   auto operator<=>(const user_type&) const = default;
};
constexpr user_type null_obj{ 0, 0, 0 };

TEST_CASE("user-defined compile-time error handling ")
{
   bb::error_callback = my_error_function;

   // None of these should compile
   //constexpr auto t0 = bb::get_pixel<user_type>(nullptr, 4); // Nullptr source
   //constexpr auto t1 = bb::get_pixel<user_type>(bb::get("red.png"), -1); // Negative index
   //constexpr auto t1 = bb::get_pixel<user_type>(bb::get("red.png"), 6); // Index too big

   // These should be fine
   static_assert(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 0) == user_type{ 255, 0, 0 });
   static_assert(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 1) == user_type{ 0, 255, 0 });
   static_assert(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 2) == user_type{ 0, 0, 255 });
}


TEST_CASE("user-defined run-time error handling ")
{
   bb::error_callback = my_error_function;
   CHECK_THROWS_AS(bb::get_pixel<user_type>(nullptr, 0), std::exception);

   // Correct cases
   CHECK_EQ(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 0), user_type{ 255, 0, 0 });
   CHECK_EQ(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 1), user_type{ 0, 255, 0 });
   CHECK_EQ(bb::get_pixel<user_type>(bb::get("test_image_rgb.png"), 2), user_type{ 0, 0, 255 });
}

