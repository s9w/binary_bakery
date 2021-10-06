#include <fstream>
#include <string>

#include "test_tools.h"
#include "../binary_inliner_lib/file_tools.h"

#include <doctest/doctest.h>

using namespace inliner;

namespace
{
   auto are_sequences_equal(
      const std::vector<uint8_t>& orig_sequence,
      const std::vector<uint64_t>& read_data
   ) -> bool
   {
      const uint8_t* read_start_ptr = reinterpret_cast<const uint8_t*>(read_data.data());
      return std::equal(
         std::begin(orig_sequence),
         std::end(orig_sequence),
         read_start_ptr
      );
   }
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
   const std::vector<uint64_t> result = get_binary_file(path).data;
   CHECK(are_sequences_equal(byte_sequence, result));
}
