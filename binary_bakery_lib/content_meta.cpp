#include "content_meta.h"

#include "byte_sequencer.h"
#include "tools.h"

namespace
{
   using namespace bb;

   template<typename T>
   struct meta_type_int  {};

   template<>
   struct meta_type_int<generic_binary> : std::integral_constant<uint8_t, 0> {};

   template<>
   struct meta_type_int<naive_image_type> : std::integral_constant<uint8_t, 1> {};

   template<int i>
   struct meta_type_int<dual_image_type<i>> : std::integral_constant<uint8_t, 2> {};

   template<typename T>
   constexpr uint8_t meta_type_int_v = meta_type_int<T>::value;


   template<typename target_type, typename meta_type>
   [[nodiscard]] constexpr auto get_bpp(
      const meta_type& meta
   ) -> target_type
   {
      if constexpr (image_type_c<meta_type>)
      {
         return static_cast<target_type>(meta.m_bpp);
      }
      else
      {
         return target_type{};
      }
   }


   template<typename meta_type>
   [[nodiscard]] constexpr auto get_dimensions(
      const meta_type& meta
   ) -> std::array<uint16_t, 2>
   {
      if constexpr (image_type_c<meta_type>)
      {
         return {
            static_cast<uint16_t>(meta.m_width),
            static_cast<uint16_t>(meta.m_height)
         };
      }
      else
      {
         return { 0ui16, 0ui16 };
      }
   }


   template<typename meta_type>
   [[nodiscard]] constexpr auto get_4byte_color_pair(
      const meta_type& meta
   ) -> color_pair<4>
   {
      if constexpr (dual_image_type_c<meta_type>)
      {
         return {
            meta.m_color0.get_4byte_padded(),
            meta.m_color1.get_4byte_padded()
         };
      }
      else
      {
         return { color<4>::black(), color<4>::black() };
      }
   }

} // namespace {}


auto bb::meta_and_size_to_header_stream(
   const content_meta& meta,
   const bit_count data_bit_count
) -> std::array<uint8_t, 24>
{
   header_sequencer sequencer;

   // Write type
   std::visit(
      [&]<typename T>(const T&) {sequencer.add(meta_type_int_v<T>); }
      , meta
   );

   // Write bpp
   std::visit(
      [&](const auto alternative) {sequencer.add(get_bpp<uint8_t>(alternative)); }
      , meta
   );

   // 2 byte padding
   sequencer.add<uint16_t>(0ui16);

   // binary bit count
   sequencer.add<uint32_t>(data_bit_count.m_value); // TODO cast

   // Dimensions
   std::visit(
      [&](const auto& alternative) {sequencer.add(get_dimensions(alternative)); }
      , meta
   );

   // 4 byte padding
   sequencer.add<uint32_t>(0ui32);
   

   // Write dual colors
   std::visit(
      [&](const auto& alternative) {sequencer.add(get_4byte_color_pair(alternative)); }
      , meta
   );

   return sequencer.get();
}

