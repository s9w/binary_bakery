#include <doctest/doctest.h>

#include <binary_bakery_lib/color.h>

using namespace bb;

TEST_CASE("color constructor types")
{
   static_assert(std::same_as<decltype(color(0)), color<1>>);
   static_assert(std::same_as<decltype(color(0, 1)), color<2>>);
   static_assert(std::same_as<decltype(color(0, 1, 2)), color<3>>);
   static_assert(std::same_as<decltype(color(0, 1, 2, 3)), color<4>>);
}


TEST_CASE("color comparisons")
{
   constexpr color red{255, 0, 0};
   constexpr color another_red{255, 0, 0};
   constexpr color green{0, 255, 0};
   CHECK_EQ(red, another_red);
   CHECK_NE(red, green);
}
