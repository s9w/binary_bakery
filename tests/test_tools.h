#pragma once

#include "../binary_bakery_lib/file_tools.h"

namespace bb
{

   template<typename ... Ts>
   auto write_binary_sequence(
      const char* path,
      const Ts... value
   ) -> void;

}

template<typename ... Ts>
auto bb::write_binary_sequence(
   const char* path,
   const Ts... value
) -> void
{
   const std::vector<uint8_t> byte_sequence = get_byte_sequence(value...);
   write_binary_file(path, byte_sequence);
}