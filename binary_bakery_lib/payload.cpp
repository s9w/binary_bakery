#include "payload.h"

#include <format>
#include <fstream>
#include <iostream>

#include "config.h"
#include "tools.h"
#include "file_tools.h"
#include "image.h"

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
      const bit_count bits = detail::get_content_bit_count(meta, file_content);
      const auto test = path.filename();
      return { std::move(file_content), meta, bits, path.filename().string() };
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
   [[nodiscard]] auto get_image_meta(
      const image<bpp>& image
   ) -> content_meta
   {
      const std::optional<color_pair<bpp>> color_pair = get_color_pair<bpp>(image);
      if (color_pair.has_value())
      {
         return dual_image_type<bpp>{
            image.m_width,
               image.m_height,
               color_pair.value().m_color0,
               color_pair.value().m_color1
         };
      }
      else
      {
         return naive_image_type{ image.m_width, image.m_height, bpp };
      }
   }


   template<int bpp>
   [[nodiscard]] auto get_image_payload_impl(
      const int width,
      const int height,
      const unsigned char* image_data_ptr,
      const fs::path& path
   ) -> payload
   {
      const image<bpp> image{ width, height, image_data_ptr };
      content_meta meta = get_image_meta(image);
      std::vector<uint8_t> stream = get_image_bytestream(image, meta);

      const bit_count bits = std::visit(
         [&](const auto& alt) {return detail::get_content_bit_count(alt, stream); },
         meta
      );

      return { std::move(stream), meta, bits, path };
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
      std::string result = "stream_" + path.filename().string();
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

} // namespace {}


auto bb::get_payload(const fs::path& path) -> payload
{
   if (is_image_path(path))
      return get_image_payload(path);
   else
      return get_binary_file_payload(path);
}


auto bb::write_payload_to_file(
   const config& cfg,
   payload&& pl
) -> void
{
   const std::string variable_name = get_variable_name(pl.m_path);
   const auto data_source = detail::get_final_bytestream(std::move(pl));

   std::ofstream filestream(get_target_path(pl.m_path, cfg.group_header_name), std::ios::out);
   if (!filestream.good())
   {
      std::cout << std::format("Couldn't open {} for writing\n", cfg.group_header_name);
      return;
   }

   std::string content;
   const int symbol_count = get_symbol_count<uint64_t>(byte_count{ data_source.size() });
   content.reserve(symbol_count*21);
   
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
      if(in_line_count == groups_per_line && is_last == false)
      {
         content += "\n";
         content += indentation_str;
         in_line_count = 0;
      }
   }

   filestream << "constexpr uint64_t ";
   filestream << variable_name;
   filestream << "[]{\n";
   filestream << indentation_str;
   filestream << content;
   filestream << "\n};\n";
}


auto detail::get_final_bytestream(payload&& pl) -> std::vector<uint8_t>
{
   std::vector<uint8_t> result(24);
   result.reserve(24 + pl.m_content_data.size());

   const std::array<uint8_t, 24> header_stream = meta_and_size_to_header_stream(pl.m_meta, pl.m_bit_count);
   std::copy(header_stream.begin(), header_stream.end(), result.data());

   append_moved(result, std::move(pl.m_content_data));

   return result;
}
