
#define BAKERY_PROVIDE_VECTOR
#define BAKERY_PROVIDE_STD_ARRAY
#include "decoder.h"


namespace user
{
   struct user_type {
      //uint8_t m_value[8];
      //uint64_t xxx;
      uint8_t r{}, g{}, b{};
   };

#include "dual_24bit.h"

   auto user_fun()
   {
      constexpr inliner::header decoded_meta = inliner::get_header(dual_24bit);
      constexpr auto decoded_array = inliner::decode_to_array<user_type, decoded_meta>(dual_24bit);

      //const auto vec = inliner::decode_to_vector<user_type>(dual_24bit);

      int end = 0;
   }
} // namespace user

#include <doctest/doctest.h>

TEST_CASE("dummy")
{
   user::user_fun();

   int end = 0;
}
