#pragma once

#include <vector>

#include "tools.h"


namespace inliner
{

   struct binary_file_content {
      std::vector<uint64_t> data;
      int byte_count;
   };

   [[nodiscard]] auto get_binary_file(const char* path) -> binary_file_content;
   auto write_binary_file(const char* path, const std::vector<uint8_t>& byte_sequence) -> void;

}
