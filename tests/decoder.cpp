
#define BAKERY_PROVIDE_VECTOR
#define BAKERY_PROVIDE_STD_ARRAY
#include "decoder.h"


namespace user
{
   struct user_type {
      uint8_t m_value[8];
      //uint64_t xxx;
      //uint8_t r, g, b;
   };

   auto user_fun()
   {
      #include "input1.cpp"
      constexpr inliner::header decoded_meta = inliner::get_header(input1);
      const auto content = inliner::decode_to_array<user_type, decoded_meta.byte_count>(input1);
      //constexpr int element_count = std::size(content);

      const auto x = inliner::decode_to_vector<user_type>(input1);

      //void* dst = nullptr;
      //inliner::decode_into_pointer(input0, dst);

      int end = 0;
   }
} // namespace user


#include <doctest/doctest.h>

TEST_CASE("dummy")
{
   user::user_fun();

   int end = 0;
}
