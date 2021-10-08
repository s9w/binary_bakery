
#define BAKERY_PROVIDE_VECTOR
#define BAKERY_PROVIDE_STD_ARRAY
#include "../decoder.h"

#include <compare>

namespace user
{
   struct user_type {
      //uint8_t m_value[8];
      //uint64_t xxx;
      uint8_t r{}, g{}, b{};
      auto operator<=>(const user_type&) const = default;
   };

#include "dual_24bit.h"

   auto user_fun()
   {
      constexpr inliner::header decoded_header = inliner::get_header(dual_24bit);
      constexpr auto decoded_array = inliner::decode_to_array<user_type, decoded_header>(dual_24bit);

      constexpr auto easy_count = inliner::get_element_count<user_type>(decoded_header);

      const std::vector<user_type> vec = inliner::decode_to_vector<user_type>(dual_24bit);
      std::vector<user_type> vec_copy(vec.size());
      inliner::decode_into_pointer(dual_24bit, vec_copy.data());
      const bool eq = vec == vec_copy;
      if (eq == false) {
         int not_equal = 1;
      }

      int end = 0;
   }
} // namespace user

#include <doctest/doctest.h>

TEST_CASE("dummy")
{
   user::user_fun();

   int end = 0;
}
