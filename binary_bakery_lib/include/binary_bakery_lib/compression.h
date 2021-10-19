#pragma once

#include <vector>

namespace bb
{
   auto get_zstd_compressed(const std::vector<uint8_t>& input) -> std::vector<uint8_t>;
   auto get_lz4_compressed(const std::vector<uint8_t>& input) -> std::vector<uint8_t>;
}
