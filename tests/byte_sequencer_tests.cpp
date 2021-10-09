#include <binary_bakery_lib/byte_sequencer.h>

#include <doctest/doctest.h>

using namespace bb;


namespace
{
   constexpr auto run() -> bool
   {
      header_sequencer sequencer;
      for (int i = 0; i < 24; ++i)
         sequencer.add(uint8_t{});
      return true;
   }

} // namespace {}


TEST_CASE("byte sequencer")
{
   constexpr bool result = run();
   CHECK(result);
}
