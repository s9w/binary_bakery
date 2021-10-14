#pragma once

#include <compare>
#include <cstdint>

namespace tests
{

   struct nc_test_rgb {
      uint8_t r{}, g{}, b{};
      nc_test_rgb() = delete;
      constexpr nc_test_rgb(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
      auto operator<=>(const nc_test_rgb&) const = default;
   };

   struct test_rgb {
      uint8_t r{}, g{}, b{};
      test_rgb() = default;
      constexpr test_rgb(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
      auto operator<=>(const test_rgb&) const = default;
   };

   static_assert(std::is_default_constructible_v<nc_test_rgb> == false);
   static_assert(std::is_default_constructible_v<test_rgb> == true);

   constexpr nc_test_rgb nc_test_rgb_null{ 0, 0, 0 };
   constexpr test_rgb test_rgb_null{ 0, 0, 0 };

} // namespace tests

