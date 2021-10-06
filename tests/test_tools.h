#pragma once

#include "../binary_inliner_lib/file_tools.h"

namespace inliner
{

   template<typename ... Ts>
   auto write_binary_sequence(
      const char* path,
      const Ts... value
   ) -> void;

}

template<typename ... Ts>
auto inliner::write_binary_sequence(
   const char* path,
   const Ts... value
) -> void
{
   const std::vector<uint8_t> byte_sequence = get_byte_sequence(value...);
   write_binary_file(path, byte_sequence);
}