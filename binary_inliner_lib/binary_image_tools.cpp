#include "binary_image_tools.h"

#include <optional>
#include <stdexcept>

namespace {

   using namespace inliner;

   template<int bpp>
   [[nodiscard]] auto get_actual_color_pair(
      const image<bpp>& im
   ) -> std::optional<color_pair<bpp>>
   {
      using color_type = image<bpp>::color_type;

      color_type color_A = im[0];
      std::optional<color_type> color_B;
      for (const color_type& color : im)
      {
         if (color == color_A)
         {
            continue;
         }
         if (color_B.has_value() && color == color_B.value())
         {
            continue;
         }
         if (color_B.has_value() == false)
         {
            color_B.emplace(color);
            continue;
         }
         return std::nullopt;
      }

      // Only contains one color
      if (color_B.has_value() == false)
      {
         color_B.emplace(color_A);
      }
      return color_pair<bpp>{color_A, color_B.value()};
   }

   //template auto get_actual_color_pair<1>(unsigned char* data, const int width, const int height)->std::optional<color_pair<1>>;
   //template auto get_actual_color_pair<2>(unsigned char* data, const int width, const int height)->std::optional<color_pair<2>>;
   //template auto get_actual_color_pair<3>(unsigned char* data, const int width, const int height)->std::optional<color_pair<3>>;
   //template auto get_actual_color_pair<4>(unsigned char* data, const int width, const int height)->std::optional<color_pair<4>>;


   template<int bpp>
   [[nodiscard]] constexpr auto get_indexed_color(
      const color<bpp>& color,
      const color_pair<bpp>& pair
   ) -> color_name
   {
      if (color == pair.color0)
      {
         return color_name::first;
      }
      else if (color == pair.color1)
      {
         return color_name::second;
      }
      else {
         throw std::runtime_error("Color doesn't belong to pair. This shouldn't happen.");
      }
   }

} // namespace {}


template<int bpp>
auto inliner::get_color_pair(
   const image<bpp>& image
) -> std::optional<color_pair<bpp>>
{
   if (image.get_pixel_count() == 0)
   {
      return std::nullopt;
   }
   else if (image.get_pixel_count() == 1)
   {
      return color_pair<bpp>{ image[0], image[0] };
   }
   else if (image.get_pixel_count() == 2)
   {
      return color_pair<bpp>{ image[0], image[1] };
   }
   else
   {
      return get_actual_color_pair(image);
   }
}

template auto inliner::get_color_pair(const image<1>& image)->std::optional<color_pair<1>>;
template auto inliner::get_color_pair(const image<2>& image)->std::optional<color_pair<2>>;
template auto inliner::get_color_pair(const image<3>& image)->std::optional<color_pair<3>>;
template auto inliner::get_color_pair(const image<4>& image)->std::optional<color_pair<4>>;



template<int bpp>
auto inliner::get_indexed_dual_image(
   unsigned char* data,
   const int width,
   const int height,
   const color_pair<bpp>& pair
) -> std::vector<color_name>
{
   const int pixel_count = width * height;
   using color_type = color<bpp>;
   const color_type* colors = std::bit_cast<const color_type*>(data);

   std::vector<color_name> result;
   result.reserve(pixel_count);

   for (int i = 0; i < pixel_count; ++i)
   {
      result.emplace_back(get_indexed_color(colors[i]));
   }

   return result;
}
