#include "payload.h"

#include <format>
#include <fstream>
#include <iostream>

#include "config.h"
#include "tools.h"
#include "file_tools.h"
#include "image.h"
#include "compression.h"

#include <stb/stb_image.h>


namespace {

   using namespace bb;

   [[nodiscard]] auto is_image_path(const abs_file_path& file) -> bool
   {
      if (file.get_path().extension() == fs::path(".png"))
      {
         return true;
      }
      return false;
   }


   [[nodiscard]] auto get_binary_file_payload(const abs_file_path& file) -> payload
   {
      std::vector<uint8_t> file_content = get_binary_file(file);
      constexpr generic_binary meta{};
      return { std::move(file_content), meta, file };
   }


   template<int bpp>
   [[nodiscard]] auto get_image_payload_impl(
      const abs_file_path& file,
      const image_dimensions& image_dim,
      const config& cfg
   ) -> payload
   {
      const image<bpp> image{ file, image_dim, cfg.image_loading_direction };
      const naive_image_type meta{ image.m_width, image.m_height, bpp };
      std::vector<uint8_t> stream = get_image_bytestream(image);

      return { std::move(stream), meta, file };
   }


   [[nodiscard]] auto get_image_payload(
      const abs_file_path& file,
      const config& cfg
   ) -> payload
   {
      const image_dimensions image_dim = get_image_dimensions(file);

      return [&]() {
         switch (image_dim.bpp) {
         case 1:
            return get_image_payload_impl<1>(file, image_dim, cfg);
         case 2:
            return get_image_payload_impl<2>(file, image_dim, cfg);
         case 3:
            return get_image_payload_impl<3>(file, image_dim, cfg);
         case 4:
            return get_image_payload_impl<4>(file, image_dim, cfg);
         default:
            std::cout << "unexpected\n";
            std::terminate();
         }
      }();
   }


   auto get_variable_name(
      const abs_file_path& file
   ) -> std::string
   {
      std::string result = "bb_" + file.get_path().filename().string();
      result = get_replaced_str(result, ".", "_");
      return result;
   }


   auto write_string_compare_fun(
      std::ofstream& out
   ) -> void
   {
      out << R"([[nodiscard]] constexpr auto is_equal_c_string(
   char const* first,
   char const* second
) -> bool
{
   return *first == *second &&
      (*first == '\0' || is_equal_c_string(&first[1], &second[1]));
}
)";
   }


   auto write_bb_get_fun(
      std::ofstream& out,
      const std::vector<payload>& payloads
   ) -> void
   {
      out << R"([[nodiscard]] static constexpr auto get(
   [[maybe_unused]] const char* name
) -> const uint64_t*
{
)";
      bool first = true;
      for (const payload& pl : payloads)
      {
         const std::string conditional_keyword = first ? "if" : "else if";
         out << std::format(
            "   {}(is_equal_c_string(name, \"{}\"))\n      return {};\n",
            conditional_keyword, pl.m_path.get_path().filename().string(), get_variable_name(pl.m_path)
         );
         first = false;
      }
      if (payloads.empty() == false)
      {
         out << "   else\n      return nullptr;\n";
      }
      else
      {
         out << "   return nullptr;\n";
      }
      out << "}\n";
   }


   auto report_diagnostics(
      const config& cfg,
      const payload& pl,
      const byte_count uncompressed_size,
      const std::vector<uint8_t>& compressed_bytestream
   ) -> void
   {
      const byte_count compressed_size{ compressed_bytestream.size() - 16 };
      std::cout << std::format(
         "Writing file \"{}\". Uncompressed size: {}."
         , pl.m_path.get_path().filename().string() , get_human_readable_size(uncompressed_size)
      );
      if (cfg.compression != compression_mode::none)
      {
         const double ratio = static_cast<double>(compressed_size.m_value) / static_cast<double>(uncompressed_size.m_value);
         std::cout << std::format(
            " compressed size: {} (compressed to {:.1f}%)"
            , get_human_readable_size(compressed_size), 100.0 * ratio
         );
      }
      std::cout << '\n';
   }

} // namespace {}


auto bb::get_payload(
   const abs_file_path& file,
   const config& cfg
) -> payload
{
   if (is_image_path(file))
      return get_image_payload(file, cfg);
   else
      return get_binary_file_payload(file);
}


auto bb::write_payloads_to_file(
   const config& cfg,
   std::vector<payload>&& payloads,
   const abs_directory_path& working_dir
) -> void
{
   std::vector<std::string> payload_strings;
   for (payload& pl : payloads)
   {
      const byte_count uncompressed_size{ pl.m_content_data.size() }; // needs to be read here because it's moved in next line
      const std::vector<uint8_t> data_source = detail::get_final_bytestream(pl, cfg);

      report_diagnostics(cfg, pl, uncompressed_size, data_source);

      std::string content;
      const int symbol_count = get_symbol_count<uint64_t>(byte_count{ data_source.size() });
      content.reserve(symbol_count * 21);

      const uint64_t* ui64_ptr = reinterpret_cast<const uint64_t*>(data_source.data());

      constexpr auto chunk_len = std::char_traits<char>::length("0x3f3ffc3f3ffc3f3f, ");
      const int groups_per_line = (cfg.max_columns - cfg.indentation_size) / chunk_len;
      int in_line_count = 0;
      const std::string indentation_str(cfg.indentation_size, ' ');
      for (int i = 0; i < symbol_count; ++i)
      {
         const bool is_last = i == (symbol_count - 1);
         append_ui64_str(ui64_ptr[i], content);
         if (is_last == false)
         {
            content += ", ";
         }
         ++in_line_count;
         if (in_line_count == groups_per_line && is_last == false)
         {
            content += "\n";
            content += indentation_str;
            in_line_count = 0;
         }
      }

      std::string payload_str;
      payload_str += "static constexpr uint64_t ";
      payload_str += get_variable_name(pl.m_path);
      payload_str += "[]{\n";
      payload_str += indentation_str;
      payload_str += content;
      payload_str += "\n};\n";
      payload_strings.emplace_back(std::move(payload_str));
   }

   const fs::path output_path = working_dir.get_path() / cfg.output_filename;
   std::ofstream filestream(output_path, std::ios::out);
   if (!filestream.good())
   {
      std::cout << std::format("Couldn't open {} for writing\n", cfg.output_filename);
      return;
   }

   filestream << "namespace bb{\n";
   for(std::string& payload_string : payload_strings)
   {
      filestream << payload_string;
   }

   filestream << '\n';
   write_string_compare_fun(filestream);
   filestream << '\n';
   write_bb_get_fun(filestream, payloads);
   filestream << "\n} // namespace bb\n";
}


auto detail::get_final_bytestream(
   payload& pl,
   const config& cfg
) -> std::vector<uint8_t>
{
   const byte_count uncompressed_size{ pl.m_content_data.size() };
   byte_count compressed_size = uncompressed_size;

   std::optional<std::vector<uint8_t>> compressed_stream;

   switch (cfg.compression) {
   case compression_mode::none:
      // nothing
      break;
   case compression_mode::zstd:
      compressed_stream.emplace(get_zstd_compressed(pl.m_content_data));
      break;
   case compression_mode::lz4:
      compressed_stream.emplace(get_lz4_compressed(pl.m_content_data));
      break;
   default:
      break;
   }

   if (compressed_stream.has_value())
   {
      compressed_size.m_value = static_cast<int>(compressed_stream.value().size());
   }
   const std::array<uint8_t, 16> header_stream = meta_and_size_to_header_stream(pl.m_meta, cfg.compression, uncompressed_size, compressed_size);

   std::vector<uint8_t> result(16); // for header
   result.reserve(result.size() + pl.m_content_data.size());
   std::copy(header_stream.begin(), header_stream.end(), result.data());
   if (compressed_stream.has_value())
   {
      append_moved(result, std::move(*compressed_stream));
   }
   else
   {
      append_moved(result, std::move(pl.m_content_data));
   }
   

   return result;
}
