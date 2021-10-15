#include <binary_bakery_lib/tools.h>

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


TEST_CASE("get_human_readable_size")
{
   CHECK_EQ(get_human_readable_size(byte_count{ 1024 * 1024 }), "1.00 MB");
   CHECK_EQ(get_human_readable_size(byte_count{ 1024 }), "1.00 KB");
   CHECK_EQ(get_human_readable_size(byte_count{ 1000 }), "1000 bytes");
}
