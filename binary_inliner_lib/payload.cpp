#include "payload.h"

#include "tools.h"

namespace
{
   using namespace inliner;

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





   template<image_type_c meta_type>
   auto write_details(
      header_sequencer& sequencer,
      const meta_type& meta
   ) -> void
   {
      sequencer.add<uint8_t>(static_cast<uint8_t>(meta.m_bpp));

      // padding
      for(int i=0; i<6; ++i)
         sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8));

      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.width));
      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.height));

      //if constexpr (dual_image_type_c<meta_type>) {
      //   for (int i = 0; i < meta.m_bpp; ++i)
      //      sequencer.add<uint8_t>(meta.color0[i]);
      //   for (int i = 0; i < meta.m_bpp; ++i)
      //      sequencer.add<uint8_t>(meta.color1[i]);
      //}
   }


   auto write_details(
      header_sequencer& sequencer,
      const generic_binary&
   ) -> void
   {
      sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8)); // bpp

      // padding
      for (int i = 0; i < 6; ++i)
         sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8));

      sequencer.add<uint16_t>(static_cast<uint16_t>(0ui16)); // Width
      sequencer.add<uint16_t>(static_cast<uint16_t>(0ui16)); // Height
   }


   template<typename meta_type>
   auto write_dual_colors(
      header_sequencer& sequencer,
      const meta_type&
   ) -> void
   {
      // Dummy
      for(int i=0; i<8; ++i)
         sequencer.add<uint8_t>(static_cast<uint8_t>(0ui8));
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


auto inliner::meta_and_size_to_binary(
   const payload& pl,
   const uint32_t data_size
) -> std::array<uint8_t, 24>
{
   header_sequencer sequencer;

   sequencer.add<uint8_t>(
      std::visit(
         [](const auto& alternative) {
            using type = std::decay_t<decltype(alternative)>;
            return meta_type_int_v<type>;
         }
         , pl.meta
      )
   );

   std::visit(
      [&](const auto& alternative) {write_details(sequencer, alternative); }
      , pl.meta
   );

   std::visit(
      [&](const auto& alternative) {write_dual_colors(sequencer, alternative); }
      , pl.meta
   );

   sequencer.add<uint32_t>(data_size);

   return sequencer.m_sequence;
}

