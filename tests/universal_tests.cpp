#include <binary_bakery_lib/universal.h>

#include <doctest/doctest.h>

using namespace bb;


TEST_CASE("bit and byte types")
{
   CHECK_EQ(bit_count{8}.get_byte_count(), byte_count{ 1 });
   CHECK_EQ(bit_count{9}.get_byte_count(), byte_count{ 2 });
}
