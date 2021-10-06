#include <optional>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "../binary_inliner_lib/encoding.h"
#include "../binary_inliner_lib/binary_image_tools.h"

using namespace inliner;

TEST_CASE("plotting things()")
{
   const std::string path = "C:\\Dropbox\\code\\binary_inliner\\tests\\dual_24bit.png";
   const image<3> im = get_image<3>(path);
   const std::optional<color_pair<3>> pair = get_color_pair(im);
   CHECK(pair.has_value());

}

auto run_doctest() -> std::optional<int> {
   doctest::Context context;
   const int res = context.run();
   if (context.shouldExit())
      return res;
   return std::nullopt;
}


auto main() -> int
{
   const std::optional<int> doctest_result = run_doctest();
   if (doctest_result.has_value())
      return doctest_result.value();
   return 0;
}

