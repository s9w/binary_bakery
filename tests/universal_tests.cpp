#include <optional>

#include <binary_bakery_lib/universal.h>

#include <doctest/doctest.h>

using namespace bb;

TEST_CASE("concepts and type traits")
{
   CHECK(numerical<int>);
   CHECK(numerical<bool> == false);

   CHECK(is_variant_v<int> == false);
   CHECK(is_variant_v<std::optional<int>> == false);
   CHECK(is_variant_v<std::variant<int>>);
   CHECK(is_variant_v<std::variant<>>);

   CHECK(is_alternative_v<int, std::variant<>> == false);
   CHECK(is_alternative_v<int, std::variant<double, int, bool>>);
   CHECK(is_alternative_v<float, std::variant<double, int, bool>> == false);

#ifndef __GNUG__
   CHECK_EQ(byte_sizeof<float, uint16_t>, byte_count{6});
#endif

}
