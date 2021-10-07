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
      #include "user_in_test.cpp"
      constexpr inliner::meta decoded_meta = inliner::decode_meta(input0);
      static_assert(decoded_meta.byte_count == 8);
      const auto content = inliner::decode_content<user_type, decoded_meta.byte_count>(input0);

      const auto x = inliner::decode_content_runtime<user_type>(input0);

      int end = 0;
   }
}


#include <doctest/doctest.h>

TEST_CASE("dummy")
{
   user::user_fun();

   int end = 0;
}
