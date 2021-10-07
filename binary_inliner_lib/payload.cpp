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
      binary_sequencer& sequencer,
      const meta_type& meta
   ) -> void
   {
      sequencer.add<uint8_t>(static_cast<uint8_t>(meta.m_bpp));
      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.width));
      sequencer.add<uint16_t>(static_cast<uint16_t>(meta.height));

      if constexpr (dual_image_type_c<meta_type>) {
         for (int i = 0; i < meta.m_bpp; ++i)
            sequencer.add<uint8_t>(meta.color0[i]);
         for (int i = 0; i < meta.m_bpp; ++i)
            sequencer.add<uint8_t>(meta.color1[i]);
      }
   }


   auto write_details(
      binary_sequencer&,
      const generic_binary&
   ) -> void
   {
      // Do nothing
   }

} // namespace {}


auto inliner::meta_to_binary(
   const payload& pl
) -> std::vector<uint8_t>
{
   constexpr size_t maximum_binary_size = 14;
   binary_sequencer sequencer(maximum_binary_size);

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

   return sequencer.m_sequence;
}

