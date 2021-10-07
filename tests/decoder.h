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
      uint8_t type = 0;        // 0: Generic binary
                               // 1: Image
                               // 2: Dual-image (TODO)
      uint8_t bpp = 0;         // Number of channels [1-4]
      uint16_t byte_count = 0; // Number of bytes stored
      uint8_t padding0[4]{0, 0, 0, 0}; // TODO: maybe embed version

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
   [[nodiscard]] constexpr auto get_byte_count(const uint64_t* ptr) -> int;

   template<typename user_type, int source_size>
   [[nodiscard]] constexpr auto get_element_count(const uint64_t(&source)[source_size]) -> int;


#ifdef BAKERY_PROVIDE_STD_ARRAY
   template<typename user_type, int byte_count, int source_size, int element_count = byte_count / sizeof(user_type)>
   [[nodiscard]] constexpr auto decode_to_array(
      const uint64_t(&source)[source_size]
   ) -> std::array<user_type, element_count>;
#endif

#ifdef BAKERY_PROVIDE_VECTOR
   template<typename user_type, int source_size>
   [[nodiscard]] auto decode_to_vector(const uint64_t(&source)[source_size]) -> std::vector<user_type>;
#endif


   template<int source_size>
   auto decode_into_pointer(
      const uint64_t(&source)[source_size],
      void* dst
   ) -> void;

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
         uint8_t type = 0;
         uint8_t bpp = 0;
         uint16_t byte_count = 0;
         uint8_t padding0[4];
      };

      const front_decoder temp = std::bit_cast<front_decoder>(ptr[0]);
      result.type = temp.type;
      result.bpp = temp.bpp;
      result.byte_count = temp.byte_count;
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


constexpr auto inliner::get_byte_count(
   const uint64_t* ptr
) -> int
{
   const auto temp1 = std::bit_cast<detail::better_array<uint16_t, 4>>(ptr[0]);
   return temp1[1];
}


#ifdef BAKERY_PROVIDE_STD_ARRAY
template<typename user_type, int byte_count, int source_size, int element_count>
constexpr auto inliner::decode_to_array(
   const uint64_t(&source)[source_size]
) -> std::array<user_type, element_count>
{
   static_assert(element_count > 0, "Not enough bytes to even fill one of those types.");
   using target_type = detail::wrapper_type<user_type, element_count>;

   return std::bit_cast<target_type>(source).m_data;
}
#endif


#ifdef BAKERY_PROVIDE_VECTOR
template<typename user_type, int source_size>
auto inliner::decode_to_vector(
   const uint64_t(&source)[source_size]
) -> std::vector<user_type>
{
   const int byte_count = get_byte_count(&source[0]);
   const int element_count = byte_count / sizeof(user_type);

   std::vector<user_type> result(element_count);
   std::memcpy(result.data(), &source[3], byte_count);
   return result;
}
#endif


template<int source_size>
auto inliner::decode_into_pointer(
   const uint64_t(&source)[source_size],
   void* dst
) -> void
{
   const int byte_count = get_byte_count(source);
   const uint64_t* start_ptr = &source[3];
   std::memcpy(dst, start_ptr, byte_count);
}


template<typename user_type, int source_size>
constexpr auto inliner::get_element_count(
   const uint64_t(&source)[source_size]
) -> int
{
   const int byte_count = get_byte_count(source);
   const int element_count = byte_count / sizeof(user_type);
   return element_count;
}
