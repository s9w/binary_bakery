#pragma once

#include "universal.h"
#include "file_tools.h"

#include <string>

namespace bb
{

   struct config {
      std::string output_filename = "binary_bakery_payload.h";
      int indentation_size = 3;
      int max_columns = 100;
      bool smart_mode = true;
      compression_mode compression = compression_mode::lz4;
   };

   [[nodiscard]] auto get_cfg_from_dir(const abs_directory_path& dir) -> std::optional<config>;

   [[nodiscard]] auto read_config_from_toml(const abs_file_path& file) -> config;

}
