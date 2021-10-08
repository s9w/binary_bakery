#include "payload.h"

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
   auto get_bpp(
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


   template<image_type_c meta_type>
   auto write_dimensions(
      header_sequencer& sequencer,
      const meta_type& meta
   ) -> void
   {
      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.width));
      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.height));
   }


   auto write_dimensions(
      header_sequencer& sequencer,
      const generic_binary&
   ) -> void
   {
      sequencer.add<uint16_t>(static_cast<uint16_t>(0ui16)); // Width
      sequencer.add<uint16_t>(static_cast<uint16_t>(0ui16)); // Height
   }


   template<typename meta_type>
   auto write_dual_colors(
      header_sequencer& sequencer,
      const meta_type&
   ) -> void
   {
      sequencer.add<uint64_t>(0ui64);
   }


   template<dual_image_type_c meta_type>
   auto write_dual_colors(
      header_sequencer& sequencer,
      const meta_type& meta
   ) -> void
   {
      const int dummy_components = 4 - meta.m_bpp;
      for (int i = 0; i < meta.m_bpp; ++i)
         sequencer.add<uint8_t>(meta.color0[i]);
      for (int i = 0; i < dummy_components; ++i)
         sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8));

      for (int i = 0; i < meta.m_bpp; ++i)
         sequencer.add<uint8_t>(meta.color1[i]);
      for (int i = 0; i < dummy_components; ++i)
         sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8));
   }

} // namespace {}


auto bb::meta_and_size_to_binary(
   const payload& pl,
   const uint32_t data_bit_count
) -> std::array<uint8_t, 24>
{
   header_sequencer sequencer;

   // Write type
   sequencer.add<uint8_t>(
      std::visit(
         [](const auto& alternative) {
            using type = std::decay_t<decltype(alternative)>;
            return meta_type_int_v<type>;
         }
         , pl.meta
      )
   );

   // Write bpp
   sequencer.add<uint8_t>(
      std::visit(
         [](const auto x) {return get_bpp<uint8_t>(x); }
         , pl.meta
      )
   );

   // 2 byte padding
   sequencer.add<uint16_t>(0ui16);

   // binary bit count
   sequencer.add<uint32_t>(static_cast<uint32_t>(data_bit_count));

   

   // Dimensions
   std::visit(
      [&](const auto& alternative) {write_dimensions(sequencer, alternative); }
      , pl.meta
   );

   // 4 byte padding
   sequencer.add<uint32_t>(0ui32);
   

   // Write dual colors
   std::visit(
      [&](const auto& alternative) {write_dual_colors(sequencer, alternative); }
      , pl.meta
   );

   return sequencer.m_sequence;
}

