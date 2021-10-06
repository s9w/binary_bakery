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
auto inliner::get_color_pair(const image<bpp>& image) -> std::optional<color_pair<bpp>>
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
   const image<bpp>& im,
   const color_pair<bpp>& pair
) -> std::vector<color_name>
{
   std::vector<color_name> result;
   result.reserve(im.get_pixel_count());
   for(const auto& color : im)
   {
      result.emplace_back(get_indexed_color(color, pair));
   }
   return result;;
}

template auto inliner::get_indexed_dual_image<1>(const image<1>& im, const color_pair<1>& pair) ->std::vector<color_name>;
template auto inliner::get_indexed_dual_image<2>(const image<2>& im, const color_pair<2>& pair) ->std::vector<color_name>;
template auto inliner::get_indexed_dual_image<3>(const image<3>& im, const color_pair<3>& pair) ->std::vector<color_name>;
template auto inliner::get_indexed_dual_image<4>(const image<4>& im, const color_pair<4>& pair) ->std::vector<color_name>;
