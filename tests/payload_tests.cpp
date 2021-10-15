#include <fstream>
#include <string>

#include <binary_bakery_lib/payload.h>
#include <binary_bakery_lib/tools.h>
#include <binary_bakery_lib/config.h>

#include <doctest/doctest.h>

using namespace bb;

namespace
{
   auto get_ui64_str(const uint64_t value) -> std::string
   {
      std::string result;
      append_ui64_str(value, result);
      return result;
   }
}

TEST_CASE("hex strings")
{
   CHECK_EQ(get_ui64_str(0), "0x0000000000000000");
   CHECK_EQ(get_ui64_str(15), "0x000000000000000f");
   CHECK_EQ(get_ui64_str(std::numeric_limits<uint64_t>::max()), "0xffffffffffffffff");
}
