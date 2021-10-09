#pragma once

#include <vector>

#include "tools.h"


namespace bb
{

   [[nodiscard]] auto get_binary_file(const char* path) -> std::vector<uint8_t>;
   auto write_binary_file(const char* path, const std::vector<uint8_t>& byte_sequence) -> void;

}
