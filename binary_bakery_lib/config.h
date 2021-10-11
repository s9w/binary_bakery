#pragma once

#include "universal.h"

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

namespace bb
{

   struct config {
      std::string output_filename = "binary_bakery_payload.h";
      int indentation_size = 3;
      int max_columns = 100;
      bool smart_mode = true;
      compression_mode compression = compression_mode::lz4;
   };


   [[nodiscard]] auto read_config_from_toml(const fs::path& path) -> config;

}
