#include <doctest/doctest.h>

#include "../binary_inliner_lib/image.h"
// TODO: merge these tests into image

using namespace inliner;

namespace
{
   template<int bpp>
   auto check_color_pair(
      const char* path
   ) -> void
   {
      const std::optional<color_pair<bpp>> pair = get_color_pair(get_image<bpp>(path));
      CHECK_FALSE(pair.has_value());
   }


   template<int bpp>
   auto check_color_pair(
      const char* path,
      const color<bpp>& first,
      const color<bpp>& second
   ) -> void
   {
      const std::optional<color_pair<bpp>> pair = get_color_pair(get_image<bpp>(path));
      CHECK(pair.has_value());
      CHECK_EQ(pair.value().color0, first);
      CHECK_EQ(pair.value().color1, second);
   }


   template<int bpp>
   auto check_indexing_result(
      const char* path,
      const std::vector<dual_color_name>& expected
   ) -> void
   {
      const image<bpp> image = get_image<bpp>(path);
      const std::optional<color_pair<bpp>> pair = get_color_pair(image);
      const std::vector<dual_color_name> indexed = get_indexed_dual_image<bpp>(image, pair.value());
      CHECK_EQ(indexed, expected);
   }
}


TEST_CASE("color pair detection")
{
   check_color_pair("dual_24bit.png", color(255, 255, 255), color(255, 0, 0));
   check_color_pair("dual_32bit.png", color{ 255, 255, 255, 255 }, color{ 255, 0, 0, 128 });
   check_color_pair<3>("not_dual_24bit.png");
}


TEST_CASE("color indexing test")
{
   const std::vector<dual_color_name> expectation{
      dual_color_name::first, dual_color_name::second, dual_color_name::second,
      dual_color_name::first, dual_color_name::second, dual_color_name::first,
      dual_color_name::first, dual_color_name::first, dual_color_name::first
   };
   check_indexing_result<3>("dual_24bit.png", expectation);
   check_indexing_result<4>("dual_32bit.png", expectation);
}
