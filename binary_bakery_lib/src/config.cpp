#include <binary_bakery_lib/config.h>

#include <algorithm>
#include <fstream>

#include <binary_bakery_lib/file_tools.h>

#include <toml++/toml.h>
#include <fmt/format.h>


namespace
{

   using namespace bb;

   [[nodiscard]] auto get_lowercase_string(
      const std::string& input
   ) -> std::string
   {
      const auto get_lowercase_char = [](const char ch) {
         const auto uc = static_cast<unsigned char>(ch);
         const int lowered = std::tolower(uc);
         return static_cast<char>(lowered);
      };

      std::string result = input;
      std::transform(result.begin(), result.end(), result.begin(), get_lowercase_char);
      return result;
   }


   // Returns lowercased for std::string, identity otherwise
   template<typename read_type>
   [[nodiscard]] constexpr auto get_sanitized_input(
      const read_type& input
   ) -> read_type
   {
      if constexpr (std::same_as<read_type, std::string>)
         return get_lowercase_string(input);
      else
         return input;
   }


   template<typename read_type = std::string, typename target_type, typename fun_type>
   auto set_value(
      target_type& target,
      const toml::table& tbl,
      const char* key,
      const fun_type& interpreter
   ) -> void
   {
      const std::optional<read_type> read_value = tbl[key].value<read_type>();

      // toml file didn't contain this setting
      if (read_value.has_value() == false)
         return;

      const std::optional<target_type> result = interpreter(get_sanitized_input(read_value.value()));

      // Interpretation error
      if (result.has_value() == false)
      {
         fmt::print("The config value \"{}\" couldn't be parsed. Skipping.\n", read_value.value());
         return;
      }

      target = result.value();
   }


   // Settings that don't require a conversion function
   template<typename target_type>
   auto set_value(
      target_type& target,
      const toml::table& tbl,
      const char* key
   ) -> void
   {
      using read_type = target_type;
      const auto opt_identity = [](const read_type& in) {
         return std::optional<read_type>{in};
      };
      set_value<read_type>(target, tbl, key, opt_identity);
   }


   [[nodiscard]] constexpr auto is_equal_c_string(
      char const* first,
      char const* second
   ) -> bool
   {
      if(std::is_constant_evaluated() == false)
         return strcmp(first, second) == 0;

      return *first == *second &&
         (*first == '\0' || is_equal_c_string(&first[1], &second[1]));
   }

   [[nodiscard]] constexpr auto get_compression_mode(
      std::string_view const value
   ) -> std::optional<compression_mode>
   {
      if (is_equal_c_string(value.data(), "none"))
         return compression_mode::none;
      else if (is_equal_c_string(value.data(), "zstd"))
         return compression_mode::zstd;
      else if (is_equal_c_string(value.data(), "lz4"))
         return compression_mode::lz4;
      else
         return std::nullopt;
   }


   [[nodiscard]] constexpr auto get_image_write_direction(
      std::string_view const value
   ) -> std::optional<image_vertical_direction>
   {
      if (is_equal_c_string(value.data(), "bottom_to_top"))
         return image_vertical_direction::bottom_to_top;
      else if (is_equal_c_string(value.data(), "top_to_bottom"))
         return image_vertical_direction::top_to_bottom;
      else
         return std::nullopt;
   }

   const std::string default_config_filename = "binary_bakery.toml";

} // namespace {}


auto bb::get_cfg_from_dir(
   const abs_directory_path& dir
) -> std::optional<config>
{
   const fs::path config_path = dir.get_path() / default_config_filename;
   if (fs::exists(config_path) == false)
      return std::nullopt;
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
      fmt::print("Couldn't parse file {}. Looking for other config.\n", file.get_path().string());
      return std::nullopt;
   }

   config cfg;
   set_value(cfg.output_filename, tbl, "output_filename");
   set_value(cfg.max_columns, tbl, "max_columns");
   set_value(cfg.compression, tbl, "compression_mode", get_compression_mode);
   set_value(cfg.prompt_for_key, tbl, "prompt_for_key");
   set_value(cfg.image_loading_direction, tbl, "image_loading_direction", get_image_write_direction);
   return cfg;
}
