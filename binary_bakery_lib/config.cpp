#include "config.h"

#include <iostream>
#include <format>
#include <fstream>

#include <tomlplusplus/toml.hpp>

namespace
{

   using namespace bb;

   template<typename T>
   auto set_value(
      const toml::table& tbl,
      T& target,
      const char* key
   ) -> void
   {
      const std::optional<T> read_value = tbl[key].value<T>();
      if (read_value.has_value() == false)
      {
         return;
      }
      target = read_value.value();
   }


   [[nodiscard]] auto get_compression_mode(
      const std::optional<std::string>& value
   ) -> compression_mode
   {
      if (value.has_value() == false)
         return compression_mode::none;

      if (value.value() == "none")
         return compression_mode::none;
      else if (value.value() == "zstd")
         return compression_mode::zstd;
      else if (value.value() == "lz4")
         return compression_mode::lz4;
      else
      {
         std::cout << std::format("compression_mode value \"{}\" not recognized. Using no compression.\n", value.value());
         return compression_mode::none;
      }
   }

   const std::string default_config_filename = "binary_bakery.toml";

} // namespace {}


auto bb::get_cfg_from_dir(
   const abs_directory_path& dir
) -> std::optional<config>
{
   const fs::path config_path = dir.get_path() / default_config_filename;
   if (fs::exists(config_path) == false)
   {
      return std::nullopt;
   }
   return get_cfg_from_file(abs_file_path{ dir.get_path() / default_config_filename });
}


auto bb::get_cfg_from_file(
   const abs_file_path& file
) -> std::optional<config>
{
   toml::table tbl;
   try
   {
      tbl = toml::parse_file(file.get_path().string());
   }
   catch (const toml::parse_error&)
   {
      const std::string msg = std::format("Couldn't parse file {}. Looking for other config.", file.get_path().string());
      std::cout << msg << std::endl;
      return std::nullopt;
   }

   config cfg;
   set_value(tbl, cfg.output_filename, "output_filename");
   set_value(tbl, cfg.max_columns, "max_columns");
   set_value(tbl, cfg.smart_mode, "smart_mode");
   cfg.compression = get_compression_mode(tbl["compression_mode"].value<std::string>());
   set_value(tbl, cfg.prompt_for_key, "prompt_for_key");
   return cfg;
}
