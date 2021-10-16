#include "payload.h"

#include <fstream>

#include <binary_bakery_lib/config.h>
#include <binary_bakery_lib/tools.h>
#include <binary_bakery_lib/file_tools.h>
#include <binary_bakery_lib/image.h>
#include <binary_bakery_lib/compression.h>

#include <fmt/format.h>


namespace {

   using namespace bb;

   [[nodiscard]] auto is_image_path(const abs_file_path& file) -> bool
   {
      const std::string ext_string = file.get_path().extension().string();
      return ext_string == std::string{ ".png" }
          || ext_string == std::string{ ".tga" }
          || ext_string == std::string{ ".bmp" };
   }


   [[nodiscard]] auto get_binary_file_payload(const abs_file_path& file) -> payload
   {
      return payload{ get_binary_file(file), generic_binary{}, file };
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

      return { get_image_bytestream(image), meta, file };
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
            fmt::print("unexpected\n");
            std::terminate();
         }
      }();
   }


   [[nodiscard]] auto get_variable_name(
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
   if(std::is_constant_evaluated() == false)
      return strcmp(first, second) == 0;

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
      out << R"([[nodiscard]] static constexpr auto get_payload(
   [[maybe_unused]] const char* name
) -> const uint64_t*
{
)";
      bool first = true;
      for (const payload& pl : payloads)
      {
         const std::string conditional_keyword = first ? "if" : "else if";
         out << fmt::format(
            "   {}(is_equal_c_string(name, \"{}\"))\n      return &{}[0];\n",
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
      fmt::print(
         "Writing file \"{}\". Uncompressed size: {}."
         , pl.m_path.get_path().filename().string() , get_human_readable_size(uncompressed_size)
      );
      if (cfg.compression != compression_mode::none)
      {
         const double ratio = static_cast<double>(compressed_size.m_value) / static_cast<double>(uncompressed_size.m_value);
         fmt::print(
            " compressed size: {} (compressed to {:.1f}%)"
            , get_human_readable_size(compressed_size), 100.0 * ratio
         );
      }
      printf("\n");
   }


   auto add_word_separator(
      std::string& target,
      const bool is_last_word_total,
      const bool is_last_word_in_line
   ) -> void
   {
      if (is_last_word_total)
         return;

      if (is_last_word_in_line)
         target += ",";
      else
         target += ", ";
   }


   // Stringified content of the data array, ie "0xffffffffffffffff, 0xffffffffffffffff, ..."
   [[nodiscard]] auto get_content(
      const std::vector<uint8_t> data_source,
      const std::string& indentation_str,
      const int words_per_line
   ) -> std::string
   {
      const uint64_t* ui64_ptr = reinterpret_cast<const uint64_t*>(data_source.data());
      
      const int word_count = get_symbol_count<uint64_t>(byte_count{ data_source.size() });
      std::string content;
      content.reserve(word_count * 21);
      int words_in_line = 0;
      for (int i = 0; i < word_count; ++i)
      {
         const bool is_last_word_total = i == (word_count - 1);
         const bool is_last_word_in_line = words_in_line == (words_per_line - 1);

         append_ui64_str(ui64_ptr[i], content);
         add_word_separator(content, is_last_word_total, is_last_word_in_line);

         ++words_in_line;

         if (is_last_word_in_line && is_last_word_total == false)
         {
            content += '\n';
            content += indentation_str;
            words_in_line = 0;
         }
      }
      return content;
   }


   [[nodiscard]] auto get_payload_string(
      const config& cfg,
      const abs_file_path& payload_path,
      const std::string& content_str
   ) -> std::string
   {
      const std::string indentation_str(cfg.indentation_size, ' ');

      std::string payload_str;
      payload_str += fmt::format("static constexpr uint64_t {}[]{{\n", get_variable_name(payload_path));
      payload_str += indentation_str;
      payload_str += content_str;
      payload_str += "\n};\n";
      return payload_str;
   }


   [[nodiscard]] auto get_payload_strings(
      std::vector<payload>&& payloads,
      const config& cfg
   ) -> std::vector<std::string>
   {
      const std::string indentation_str(cfg.indentation_size, ' ');
      constexpr auto chunk_string_len = std::char_traits<char>::length("0x3f3ffc3f3ffc3f3f, ");
      const int words_per_line = (cfg.max_columns - cfg.indentation_size) / chunk_string_len;

      std::vector<std::string> payload_strings;
      for (payload& pl : payloads)
      {
         const byte_count uncompressed_size{ pl.m_content_data.size() }; // needs to be read here because it's moved in next line
         const std::vector<uint8_t> data_source = detail::get_final_bytestream(pl, cfg);

         report_diagnostics(cfg, pl, uncompressed_size, data_source);
         std::string payload_str = get_payload_string(
            cfg,
            pl.m_path,
            get_content(data_source, indentation_str, words_per_line)
         );
         payload_strings.emplace_back(std::move(payload_str));
      }
      return payload_strings;
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
   const std::vector<std::string> payload_strings = get_payload_strings(std::move(payloads), cfg);
   const fs::path output_path = working_dir.get_path() / cfg.output_filename;
   std::ofstream filestream(output_path, std::ios::out);
   if (!filestream.good())
   {
      fmt::print("Couldn't open {} for writing\n", cfg.output_filename);
      return;
   }

   filestream << "#include <cstdint>\n";
   filestream << "#include <cstring>\n";
   filestream << "#include <type_traits> // std::is_constant_evaluated\n\n";
   filestream << "namespace bb{\n";
   for(const std::string& payload_string : payload_strings)
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
   const std::array<uint8_t, 16> header_stream = get_header_bytes(pl.m_meta, cfg.compression, uncompressed_size, compressed_size);

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
