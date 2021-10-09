#include "payload.h"

#include <format>
#include <fstream>
#include <iostream>

#include "config.h"
#include "tools.h"
#include "file_tools.h"
#include "image.h"

#include "stb_image.h"


namespace {

   using namespace bb;

   [[nodiscard]] auto is_image_path(const std::string& filename) -> bool
   {
      if (filename.ends_with("png"))
      {
         return true;
      }
      return false;
   }


   [[nodiscard]] auto get_binary_file_payload(const char* path) -> payload
   {
      std::vector<uint8_t> file_content = get_binary_file(path);
      constexpr generic_binary meta{};
      const int bit_count = detail::get_content_bit_count(meta, file_content);
      return { std::move(file_content), meta, bit_count };
   }


   struct stbi_loader {
      unsigned char* m_data = nullptr;
      int m_width = 0;
      int m_height = 0;
      int m_components = 0;

      explicit stbi_loader(const std::string& filename)
      {
         m_data = stbi_load(filename.c_str(), &m_width, &m_height, &m_components, 0);
         if (m_data == nullptr) {
            const std::string msg = std::format("Couldn't open file {}", filename);
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
               color_pair.value().color0,
               color_pair.value().color1
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
      const unsigned char* image_data_ptr
   ) -> payload
   {
      const image<bpp> image{ width, height, image_data_ptr };
      content_meta meta = get_image_meta(image);
      std::vector<uint8_t> stream = get_image_bytestream(image, meta);

      const int bit_count = std::visit(
         [&](const auto& alt) {return detail::get_content_bit_count(alt, stream); },
         meta
      );

      return { std::move(stream), meta, bit_count };
   }


   [[nodiscard]] auto get_image_payload(const std::string& filename) -> payload
   {
      const stbi_loader loader(filename);

      return [&]() {
         switch (loader.m_components) {
         case 1:
            return get_image_payload_impl<1>(loader.m_width, loader.m_height, loader.m_data);
         case 2:
            return get_image_payload_impl<2>(loader.m_width, loader.m_height, loader.m_data);
         case 3:
            return get_image_payload_impl<3>(loader.m_width, loader.m_height, loader.m_data);
         case 4:
            return get_image_payload_impl<4>(loader.m_width, loader.m_height, loader.m_data);
         default:
            std::cout << "unexpected\n";
            std::terminate();
         };
      }();
   }

} // namespace {}


auto bb::get_payload(const std::string& filename) -> payload
{
   if (is_image_path(filename))
      return get_image_payload(filename);
   else
      return get_binary_file_payload(filename.c_str());
}


auto bb::write_payload_to_file(
   const std::string& filename,
   const std::string& variable_name,
   const config& cfg,
   const payload& pl
) -> void
{
   std::vector<uint8_t> target_bytes; // TODO reserve
   for (const uint8_t byte : meta_and_size_to_binary(pl.m_meta, pl.m_bit_count))
      target_bytes.emplace_back(byte);
   append_copy(target_bytes, pl.m_content_data);

   std::ofstream filestream(filename, std::ios::out);
   if (!filestream.good())
   {
      std::cout << std::format("Couldn't open {} for writing\n", filename);
      return;
   }
   std::string content;
   // TODO think about good reserve estimate
   const int symbol_count = get_symbol_count<uint64_t>(static_cast<int>(target_bytes.size()));
   const uint64_t* ui64_ptr = reinterpret_cast<const uint64_t*>(target_bytes.data());

   const int groups_per_line = (cfg.max_columns - cfg.indentation_size) / (2 + 16 + 2);
   int in_line_count = 0;
   const std::string indentation_str(cfg.indentation_size, ' ');
   for (int i = 0; i < symbol_count; ++i)
   {
      const bool is_last = i == (symbol_count - 1);
      content += get_ui64_str(ui64_ptr[i]);
      if (is_last == false)
      {
         content += ", ";
      }
      ++in_line_count;
      if(in_line_count == groups_per_line && is_last == false)
      {
         content += std::format("\n{}", indentation_str);
         in_line_count = 0;
      }
   }
   
   filestream << std::format("constexpr uint64_t {0}[]{{\n{1}{2}\n}};\n", variable_name, indentation_str, content);
}

