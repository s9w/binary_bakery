#pragma once

#include <string>

namespace bb
{

   struct config {
      bool in_single_file = true;
      std::string group_header_name = "bb_collection.h";
      int indentation_size = 3;
      int max_columns = 100;
      bool smart_mode = true;
   };


   [[nodiscard]] auto read_config_from_toml(const char* filename) -> config;

}
