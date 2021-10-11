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
         std::cout << std::format("Setting {} can't be read from config. Keeping default value ({}).\n", key, target);
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
      else
      {
         std::cout << std::format("compression_mode value \"{}\" not recognized. Using no compression.\n", value.value());
         return compression_mode::none;
      }
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
   
   set_value(tbl, cfg.output_filename, "output_filename");
   set_value(tbl, cfg.max_columns, "max_columns");
   set_value(tbl, cfg.smart_mode, "smart_mode");
   cfg.compression = get_compression_mode(tbl["compression_mode"].value<std::string>());

   return cfg;
}
