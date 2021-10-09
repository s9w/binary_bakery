#include "../binary_bakery_lib/tools.h"

#include <doctest/doctest.h>

using namespace bb;

namespace
{
   //template<typename T, int array_size>
   //auto are_equivalent(
   //   const std::vector<T>& vec,
   //   const std::array<T, array_size>& array
   //) -> bool
   //{
   //   if (vec.size() != array_size)
   //   {
   //      return false;
   //   }
   //   for (int i = 0; i < array_size; ++i)
   //   {
   //      if (vec[i] != array[i])
   //      {
   //         return false;
   //      }
   //   }
   //   return true;
   //}
}


TEST_CASE("get_bit_encoded()")
{
   enum class some_enum{first, second};

   {
      const std::vector<some_enum> source{ some_enum::first, some_enum::second, some_enum::second };
      const std::vector<uint8_t> encoded = get_bit_encoded(source, some_enum::second);
      const std::vector<uint8_t> expectation{ 64ui8 + 32ui8 };
      CHECK_EQ(encoded, expectation);
   }
   {
      const std::vector<some_enum> source{
         some_enum::second, some_enum::first, some_enum::first, some_enum::first, 
         some_enum::first, some_enum::first, some_enum::first, some_enum::second,
         some_enum::second
      };
      const std::vector<uint8_t> encoded = get_bit_encoded(source, some_enum::second);
      const std::vector<uint8_t> expectation{ 128ui8 + 1ui8 , 128ui8 };
      CHECK_EQ(encoded, expectation);
   }
}
