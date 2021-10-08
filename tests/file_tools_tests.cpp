#include <fstream>
#include <string>

#include "test_tools.h"
#include "../binary_bakery_lib/file_tools.h"

#include <doctest/doctest.h>

using namespace inliner;

namespace
{

}


TEST_CASE("to_byte_sequence()")
{
   constexpr double orginal = 3.14;
   const std::vector<uint8_t> s0 = get_byte_sequence(orginal);
   const double reconstructed = reinterpret_cast<const double&>(s0[0]);
   CHECK_EQ(orginal, reconstructed);
}


TEST_CASE("binary reading and writing")
{
   const std::vector<uint8_t> byte_sequence = get_byte_sequence(1.12, 4);

   const char* path = "binary_test.bin";
   write_binary_file(path, byte_sequence);
   const std::vector<uint8_t> result = get_binary_file(path);
   CHECK_EQ(byte_sequence, result);
}
