#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

#include "tools.h"


namespace bb
{

   [[nodiscard]] auto get_binary_file(const fs::path& path) -> std::vector<uint8_t>;
   auto write_binary_file(const fs::path& path, const std::vector<uint8_t>& byte_sequence) -> void;

}
