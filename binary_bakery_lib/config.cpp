#include "config.h"

#include <iostream>
#include <format>
#include <fstream>

#include <tomlplusplus/toml.hpp>

namespace
{

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
         std::cout << std::format("Setting {} can't be read from config. Keeping default value ({}).\n", key, target);
         return;
      }
      target = read_value.value();
   }

} // namespace {}


auto bb::read_config_from_toml(
   const fs::path& path
) -> config
{
   config cfg;

   toml::table tbl;
   try
   {
      tbl = toml::parse_file(path.string());
   }
   catch (const toml::parse_error&)
   {
      const std::string msg = std::format("Couldn't parse file {}. Using default config.", path.string());
      std::cout << msg << std::endl;
      return cfg;
   }
   
   set_value(tbl, cfg.in_single_file, "in_single_file");
   set_value(tbl, cfg.group_header_name, "group_header_name");
   set_value(tbl, cfg.indentation_size, "indentation_size");
   set_value(tbl, cfg.max_columns, "max_columns");
   set_value(tbl, cfg.smart_mode, "smart_mode");

   return cfg;
}
