#pragma once

#include <bit>     // For std::bit_cast
#include <cstdint> // For sized types
#include <string>  // For std::memcpy

#ifdef BAKERY_PROVIDE_VECTOR
#include <vector>
#endif

#ifdef BAKERY_PROVIDE_STD_ARRAY
#include <array>
#endif


namespace inliner {

   struct header {
      uint8_t  type = 0;       // 0: Generic binary
                               // 1: Image
                               // 2: Dual-image
      uint8_t  bpp = 0;        // Number of channels [1-4]
      uint8_t  padding0[2]{ 0, 0 }; // Padding, maybe embed version. TODO
      uint32_t bit_count = 0;  // Number of bits stored
      

      uint16_t width = 0;      // Width in pixels
      uint16_t height = 0;     // Height in pixels
      uint16_t padding1[2]{0, 0};

      uint32_t color0 = 0;     // Replacement colors
      uint32_t color1 = 0;     // Replacement colors
   };
   static_assert(sizeof(header) == 24);

   [[nodiscard]] constexpr auto get_header    (const uint64_t* ptr) -> header;
   [[nodiscard]] constexpr auto is_image      (const uint64_t* ptr) -> bool;
   [[nodiscard]] constexpr auto get_width     (const uint64_t* ptr) -> int;
   [[nodiscard]] constexpr auto get_height    (const uint64_t* ptr) -> int;


   [[nodiscard]] constexpr auto get_byte_count_from_bit_count(
      const int bit_count
   ) -> int
   {
      int byte_count = bit_count / 8;
      if ((bit_count % 8) > 0)
         ++byte_count;
      return byte_count;
   }

   template<typename user_type>
   constexpr auto get_ultimate_element_count(
      const header& head
   ) -> int
   {
      if (head.type == 2)
      {
         const int pixel_count = head.bit_count;
         return pixel_count;
      }
      else
      {
         // Byte count is exact for everything but Indexed images
         const int byte_count = head.bit_count / 8;
         return byte_count / sizeof(user_type);
      }
   }


#ifdef BAKERY_PROVIDE_STD_ARRAY
   template<typename user_type, header head, int array_size, int element_count = get_ultimate_element_count<user_type>(head)>
   [[nodiscard]] constexpr auto decode_to_array(
      const uint64_t(&source)[array_size]
   ) -> std::array<user_type, element_count>;
#endif

#ifdef BAKERY_PROVIDE_VECTOR
   //template<typename user_type, int source_size>
   //[[nodiscard]] auto decode_to_vector(const uint64_t(&source)[source_size]) -> std::vector<user_type>;
#endif


   //template<int source_size>
   //auto decode_into_pointer(
   //   const uint64_t(&source)[source_size],
   //   void* dst
   //) -> void;

} // namespace inliner


namespace inliner::detail {
   template<typename T, int size>
   struct better_array {
      T m_values[size];
      [[nodiscard]] constexpr auto operator[](const int index) const -> const T&;
   };

   template<typename user_type, int element_count>
   struct wrapper_type {
      uint64_t filler[3];
      alignas(user_type) std::array<user_type, element_count> m_data; // needs to be returned
      // End padding should be unnecessary because of array alignment
   };

   template<int bpp>
   struct color_type {
      uint8_t m_components[bpp];
   };
   template<int bpp>
   struct color_pair_type {
      color_type<bpp> color0;
      color_type<bpp> color1;
   };

   template<int bpp>
   [[nodiscard]] constexpr auto get_sized_color_pair(const header& head) -> color_pair_type<bpp>;
   
} // namespace inliner::detail


template <typename T, int size>
constexpr auto inliner::detail::better_array<T, size>::operator[](
   const int index
) const -> const T&
{
   return m_values[index];
}


constexpr auto inliner::get_header(
   const uint64_t* ptr
) -> header
{
   header result;

   {
      struct front_decoder {
         uint8_t  type;
         uint8_t  bpp;
         uint8_t  padding0[2];
         uint32_t bit_count;
      };

      const front_decoder temp = std::bit_cast<front_decoder>(ptr[0]);
      result.type = temp.type;
      result.bpp = temp.bpp;
      result.bit_count = temp.bit_count;
   }

   if (result.type > 0)
   {
      const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(ptr[1]);
      result.width = temp[0];
      result.height = temp[1];
   }
   if (result.type == 2)
   {
      const auto temp = std::bit_cast<detail::better_array<uint32_t, 2>>(ptr[2]);
      result.color0 = temp[0];
      result.color1 = temp[1];
   }

   return result;
}


constexpr auto inliner::is_image(
   const uint64_t* ptr
) -> bool
{
   const auto temp0 = std::bit_cast<detail::better_array<uint8_t, 8>>(ptr[0]);
   const uint8_t type = temp0[0];
   return type == 1 || type == 2;
}


constexpr auto inliner::get_width(
   const uint64_t* ptr
) -> int
{
   if (is_image(ptr) == false)
   {
      return -1;
   }
   const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(ptr[1]);
   return temp[0];
}


constexpr auto inliner::get_height(
   const uint64_t* ptr
) -> int
{
   if (is_image(ptr) == false)
   {
      return -1;
   }
   const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(ptr[1]);
   return temp[1];
}


namespace inliner::detail {
   
}


#ifdef BAKERY_PROVIDE_STD_ARRAY
template<typename user_type, inliner::header head, int array_size, int element_count>
constexpr auto inliner::decode_to_array(
   const uint64_t(&source)[array_size]
) -> std::array<user_type, element_count>
{
   static_assert(element_count > 0, "Not enough bytes to even fill one of those types.");

   if constexpr (head.type == 2)
   {
      static_assert(sizeof(user_type) == head.bpp, "User type has different size than bpp");
      //}
      const auto [color0, color1] = detail::get_sized_color_pair<head.bpp>(head);

      // First cast into its encoded representation
      const int byte_count = get_byte_count_from_bit_count(head.bit_count);
      using interm_type = detail::wrapper_type<uint8_t, byte_count>;
      const std::array<uint8_t, byte_count> interm = std::bit_cast<interm_type>(source).m_data;

      // Then reconstruct the original colors
      using target_type = std::array<user_type, element_count>;
      target_type result;
      for (int i = 0; i < element_count; ++i)
      {
         const int byte_index = i / 8;
         const int bit_index = i % 8;
         const int left_shift_amount = 7 - bit_index; // 7 is leftest bit
         const uint8_t mask = 1ui8 << left_shift_amount;
         const bool binary_value = static_cast<bool>((interm[byte_index] & mask) >> left_shift_amount);
         const user_type replacement_color = std::bit_cast<user_type>(binary_value ? color1 : color0);
         result[i] = replacement_color;
      }
      return result;
   }
   else
   {
      using target_type = detail::wrapper_type<user_type, element_count>;
      return std::bit_cast<target_type>(source).m_data;
   }
}
#endif


//#ifdef BAKERY_PROVIDE_VECTOR
//template<typename user_type, int source_size>
//auto inliner::decode_to_vector(
//   const uint64_t(&source)[source_size]
//) -> std::vector<user_type>
//{
//   const int byte_count = get_byte_count(&source[0]);
//   const int element_count = byte_count / sizeof(user_type);
//
//   std::vector<user_type> result(element_count);
//   std::memcpy(result.data(), &source[3], byte_count);
//   return result;
//}
//#endif
//
//
//template<int source_size>
//auto inliner::decode_into_pointer(
//   const uint64_t(&source)[source_size],
//   void* dst
//) -> void
//{
//   const int byte_count = get_byte_count(source);
//   const uint64_t* start_ptr = &source[3];
//   std::memcpy(dst, start_ptr, byte_count);
//}


template<int bpp>
constexpr auto inliner::detail::get_sized_color_pair(
   const header& head
) -> color_pair_type<bpp>
{
   const auto head_color0 = std::bit_cast<color_type<4>>(head.color0);
   const auto head_color1 = std::bit_cast<color_type<4>>(head.color1);

   color_type<bpp> color0;
   color_type<bpp> color1;
   for (int i = 0; i < bpp; ++i)
   {
      color0.m_components[i] = head_color0.m_components[i];
      color1.m_components[i] = head_color1.m_components[i];
   }
   return { color0, color1 };
}
