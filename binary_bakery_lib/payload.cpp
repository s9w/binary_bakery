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

   [[nodiscard]] auto is_image_path(const fs::path& path) -> bool
   {
      if (path.extension() == fs::path(".png"))
      {
         return true;
      }
      return false;
   }


   [[nodiscard]] auto get_binary_file_payload(const fs::path& path) -> payload
   {
      std::vector<uint8_t> file_content = get_binary_file(path);
      constexpr generic_binary meta{};
      const auto test = path.filename();
      return { std::move(file_content), meta, path.filename().string() };
   }


   struct stbi_loader {
      unsigned char* m_data = nullptr;
      int m_width = 0;
      int m_height = 0;
      int m_components = 0;

      explicit stbi_loader(const fs::path& path)
      {
         m_data = stbi_load(path.string().c_str(), &m_width, &m_height, &m_components, 0);
         if (m_data == nullptr) {
            const std::string msg = std::format("Couldn't open file {}", path.string());
            throw std::runtime_error(msg);
         }
      }

      ~stbi_loader()
      {
         stbi_image_free(m_data);
      }

      stbi_loader(const stbi_loader&) = delete;
      stbi_loader& operator=(const stbi_loader&) = delete;
      stbi_loader(stbi_loader&&) = delete;
      stbi_loader& operator=(stbi_loader&&) = delete;
   };


   template<int bpp>
   [[nodiscard]] auto get_image_payload_impl(
      const int width,
      const int height,
      const unsigned char* image_data_ptr,
      const fs::path& path
   ) -> payload
   {
      const image<bpp> image{ width, height, image_data_ptr };
      const naive_image_type meta{ width, height, bpp };
      std::vector<uint8_t> stream = get_image_bytestream(image);

      return { std::move(stream), meta, path };
   }


   [[nodiscard]] auto get_image_payload(const fs::path& path) -> payload
   {
      const stbi_loader loader(path);

      return [&]() {
         switch (loader.m_components) {
         case 1:
            return get_image_payload_impl<1>(loader.m_width, loader.m_height, loader.m_data, path);
         case 2:
            return get_image_payload_impl<2>(loader.m_width, loader.m_height, loader.m_data, path);
         case 3:
            return get_image_payload_impl<3>(loader.m_width, loader.m_height, loader.m_data, path);
         case 4:
            return get_image_payload_impl<4>(loader.m_width, loader.m_height, loader.m_data, path);
         default:
            std::cout << "unexpected\n";
            std::terminate();
         }
      }();
   }


   auto get_variable_name(
      const fs::path& path
   ) -> std::string
   {
      std::string result = "bb_" + path.filename().string();
      result = get_replaced_str(result, ".", "_");
      return result;
   }


   [[nodiscard]] auto get_target_path(
      const fs::path& input_path,
      const std::string& group_header_name
   ) -> fs::path
   {
      return input_path.parent_path() / group_header_name;
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
      out << R"([[nodiscard]] constexpr auto get(
   const char* name
) -> const uint64_t*
{
)";
      bool first = true;
      for (const payload& pl : payloads)
      {
         const std::string conditional_keyword = first ? "if" : "else if";
         out << std::format(
            "   {}(is_equal_c_string(name, \"{}\"))\n      return {};\n",
            conditional_keyword, pl.m_path.filename().string(), get_variable_name(pl.m_path)
         );
         first = false;
      }
      out << R"(   else
      return nullptr;
}
)";
   }

} // namespace {}


auto bb::get_payload(const fs::path& path) -> payload
{
   if (is_image_path(path))
      return get_image_payload(path);
   else
      return get_binary_file_payload(path);
}


auto bb::write_payloads_to_file(
   const config& cfg,
   std::vector<payload>&& payloads
) -> void
{
   if (payloads.empty())
      return;

   std::vector<std::string> payload_strings;
   for (payload& pl : payloads)
   {
      const auto data_source = detail::get_final_bytestream(pl, cfg);

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
      payload_str += "constexpr uint64_t ";
      payload_str += get_variable_name(pl.m_path);
      payload_str += "[]{\n";
      payload_str += indentation_str;
      payload_str += content;
      payload_str += "\n};\n";
      payload_strings.emplace_back(std::move(payload_str));
   }

   std::ofstream filestream(get_target_path(payloads[0].m_path, cfg.group_header_name), std::ios::out);
   if (!filestream.good())
   {
      std::cout << std::format("Couldn't open {} for writing\n", cfg.group_header_name);
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
   std::vector<uint8_t> result(16); // for header
   result.reserve(result.size() + pl.m_content_data.size());

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
   default:
      break;
   }

   if (compressed_stream.has_value())
   {
      compressed_size.m_value = static_cast<int>(compressed_stream.value().size());
   }
   const std::array<uint8_t, 16> header_stream = meta_and_size_to_header_stream(pl.m_meta, cfg.compression, uncompressed_size, compressed_size);
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
