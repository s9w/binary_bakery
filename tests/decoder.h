#pragma once

#include <cstdint>
#include <vector>
#include <string> // std::memcpy, should be in <vector> already
#include <bit> // for std::bit_cast


namespace inliner {

   struct meta {
      uint8_t type = -1;        // 0: Generic binary
                                // 1: Image
                                // 2: Dual-image (TODO)

      uint8_t bpp = -1;         // Number of channels [1-4]
      uint8_t padding0[6]{};    // TODO: maybe embed version

      uint16_t width = -1;      // Width in pixels
      uint16_t height = -1;     // Height in pixels
      uint16_t byte_count = -1; // Number of bytes stored
      uint16_t padding1 = -1;

      uint32_t color0 = -1;     // Replacement colors
      uint32_t color1 = -1;     // Replacement colors
   };
   

   [[nodiscard]] constexpr auto decode_meta   (const uint64_t* ptr) -> meta;
   [[nodiscard]] constexpr auto is_image      (const uint64_t* ptr) -> bool;
   [[nodiscard]] constexpr auto get_width     (const uint64_t* ptr) -> int;
   [[nodiscard]] constexpr auto get_height    (const uint64_t* ptr) -> int;
   [[nodiscard]] constexpr auto get_byte_count(const uint64_t* ptr) -> int;


   namespace detail{
      template<typename T, int size> struct std_array;
      template<typename user_type, int element_count> struct wrapper_type;
   }

   template<typename user_type, int byte_count, int source_array_size, int element_count = byte_count / sizeof(user_type)>
   [[nodiscard]] constexpr auto decode_content(
      const uint64_t(&source_array)[source_array_size]
   ) -> detail::std_array<user_type, element_count>;


   template<typename user_type, int source_array_size>
   [[nodiscard]] auto decode_content_runtime(
      const uint64_t(&source_array)[source_array_size]
   ) -> std::vector<user_type>;


   template<int source_array_size>
   [[nodiscard]] auto memcopy_into(
      const uint64_t(&source_array)[source_array_size],
      void* dst
   ) -> void;

} // namespace inliner


namespace inliner::detail {
   template<typename T, int size>
   struct std_array {
      T m_values[size];
      [[nodiscard]] constexpr auto operator[](const int index) const -> const T&;
   };

   template<typename user_type, int element_count>
   struct wrapper_type {
      uint64_t fillter[3];
      std_array<user_type, element_count> m_data; // needs to be returned
      // todo end padding
   };

   static_assert(sizeof(meta) == 24);
}


template <typename T, int size>
constexpr auto inliner::detail::std_array<T, size>::operator[](
   const int index
) const -> const T&
{
   return m_values[index];
}


constexpr auto inliner::decode_meta(
   const uint64_t* ptr
) -> meta
{
   meta result;

   const detail::std_array<uint8_t, 8> temp0 = std::bit_cast<detail::std_array<uint8_t, 8>>(ptr[0]);
   result.type = temp0[0];
   result.bpp = temp0[1];

   const detail::std_array<uint16_t, 4> temp1 = std::bit_cast<detail::std_array<uint16_t, 4>>(ptr[1]);
   result.width = temp1[0];
   result.height = temp1[1];
   result.byte_count = temp1[2];

   const detail::std_array<uint32_t, 2> temp2 = std::bit_cast<detail::std_array<uint32_t, 2>>(ptr[2]);
   result.color0 = temp2[0];
   result.color1 = temp2[1];

   return result;
}


constexpr auto inliner::is_image(
   const uint64_t* ptr
) -> bool
{
   const detail::std_array<uint8_t, 8> temp0 = std::bit_cast<detail::std_array<uint8_t, 8>>(ptr[0]);
   const uint8_t type = temp0[0];
   return type == 0 || type == 1;
}


constexpr auto inliner::get_width(
   const uint64_t* ptr
) -> int
{
   if (is_image(ptr) == false)
   {
      return -1;
   }
   const detail::std_array<uint16_t, 4> temp1 = std::bit_cast<detail::std_array<uint16_t, 4>>(ptr[1]);
   return temp1[0];
}


constexpr auto inliner::get_height(
   const uint64_t* ptr
) -> int
{
   if (is_image(ptr) == false)
   {
      return -1;
   }
   const detail::std_array<uint16_t, 4> temp1 = std::bit_cast<detail::std_array<uint16_t, 4>>(ptr[1]);
   return temp1[1];
}


constexpr auto inliner::get_byte_count(
   const uint64_t* ptr
) -> int
{
   const detail::std_array<uint16_t, 4> temp1 = std::bit_cast<detail::std_array<uint16_t, 4>>(ptr[1]);
   return temp1[2];
}


template<typename user_type, int byte_count, int source_array_size, int element_count>
constexpr auto inliner::decode_content(
   const uint64_t(&source_array)[source_array_size]
) -> detail::std_array<user_type, element_count>
{
   static_assert(element_count > 0, "Not enough bytes to even fill one of those types.");
   using target_type = detail::wrapper_type<user_type, element_count>;

   target_type result = std::bit_cast<target_type>(source_array);
   return result.m_data;
}



template<typename user_type, int source_array_size>
auto inliner::decode_content_runtime(
   const uint64_t(&source_array)[source_array_size]
) -> std::vector<user_type>
{
   const int byte_count = get_byte_count(source_array);
   const int element_count = byte_count / sizeof(user_type);

   std::vector<user_type> result;
   result.reserve(element_count);

   const user_type* ptr = reinterpret_cast<const user_type*>(&source_array[3]);
   for (int i = 0; i < element_count; ++i)
   {
      const user_type& value = reinterpret_cast<const user_type&>(ptr[i]);
      result.emplace_back(value);
   }

   return result;
}


template<int source_array_size>
auto inliner::memcopy_into(
   const uint64_t(&source_array)[source_array_size],
   void* dst
) -> void
{
   const int byte_count = get_byte_count(source_array);
   const uint64_t* start_ptr = &source_array[3];
   std::memcpy(dst, start_ptr, byte_count);
}
