#pragma once

#include <bit>     // For std::bit_cast
#include <cstdint> // For sized types
#include <string>  // For std::memcpy

#ifdef    BAKERY_PROVIDE_VECTOR
#include <vector>
#endif // BAKERY_PROVIDE_VECTOR

#ifdef    BAKERY_PROVIDE_STD_ARRAY
#include <array>
#endif // BAKERY_PROVIDE_STD_ARRAY


namespace bb {

   struct header {
      uint8_t  type = 0;       // 0: Generic binary
                               // 1: Image
                               // 2: Dual-image
      uint8_t  bpp = 0;        // Number of channels [1-4]
      uint8_t  padding0[2]{ 0, 0 }; // Padding, maybe embed version, compression? TODO
      uint32_t bit_count = 0;  // Number of bits stored
      

      uint16_t width = 0;      // Width in pixels
      uint16_t height = 0;     // Height in pixels
      uint16_t padding1[2]{0, 0};

      uint32_t color0 = 0;     // Replacement colors for indexed images
      uint32_t color1 = 0;     // Replacement colors for indexed images
   };
   static_assert(sizeof(header) == 24);

   [[nodiscard]] constexpr auto get_header(const char* name) -> header;
   [[nodiscard]] constexpr auto get_header(const uint64_t* source) -> header;
   [[nodiscard]] constexpr auto is_image  (const char* name) -> bool;
   [[nodiscard]] constexpr auto is_image  (const uint64_t* source) -> bool;
   [[nodiscard]] constexpr auto get_width (const char* name) -> int;
   [[nodiscard]] constexpr auto get_width (const uint64_t* source) -> int;
   [[nodiscard]] constexpr auto get_height(const char* name) -> int;
   [[nodiscard]] constexpr auto get_height(const uint64_t* source) -> int;

   // These methods provide easy access to the number of elements in the dataset. In images, that equates to the number
   // of pixels. In generic binaries, that's the number of elements of the target type.
   // Can be called with a decoded header (faster), or the dataset itself. Can be called without a target type if the
   // dataset is an image, otherwise the target type has to be provided as template parameter. Throws if that
   // assumption is violated.
   // Every function is constexpr.
   

   template<typename user_type>
   [[nodiscard]] constexpr auto get_element_count(const char* name) -> int;
   [[nodiscard]] constexpr auto get_element_count(const char* name) -> int;
   
   template<typename user_type>
   [[nodiscard]] constexpr auto get_element_count(const uint64_t* source) -> int;
   [[nodiscard]] constexpr auto get_element_count(const uint64_t* source) -> int;




#ifdef    BAKERY_PROVIDE_STD_ARRAY
   // Returns an std::array of provided type. This is constexpr. Warning: Arrays allocate the
   // memory on the stack. This will quickly be exhausted and is often limited to as little as 1MB.
   // Don't use this interface unless you know what you're doing.
   template<typename user_type, header head, int array_size, int element_count = get_element_count<user_type>(head)>
   [[nodiscard]] constexpr auto decode_to_array(
      const uint64_t(&source)[array_size]
   ) -> std::array<user_type, element_count>;
#endif // BAKERY_PROVIDE_STD_ARRAY

#ifdef    BAKERY_PROVIDE_VECTOR
   // Returns an std::vector of provided type.
   template<typename user_type>
   [[nodiscard]] auto decode_to_vector(const uint64_t* source) -> std::vector<user_type>;
   template<typename user_type>
   [[nodiscard]] auto decode_to_vector(const char* name) -> std::vector<user_type>;
#endif // BAKERY_PROVIDE_VECTOR

   // This writes into an arbitrary container-pointer. No memory management - needs to be allocated!
   template<typename user_type>
   auto decode_into_pointer(const uint64_t* source, user_type* dst) -> void;
   template<typename user_type>
   auto decode_into_pointer(const char* name, user_type* dst) -> void;

   // This is defined in the payload header. Returns nullptr if the name isn't in the dataset.
   [[nodiscard]] constexpr auto get(const char* name) -> const uint64_t*;

} // namespace bb


namespace bb::detail {
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
   [[nodiscard]] constexpr auto get_sized_color_pair(const header& head) -> color_pair_type<bpp>
   {
      const auto head_color0 = std::bit_cast<color_type<4>>(head.color0);
      const auto head_color1 = std::bit_cast<color_type<4>>(head.color1);

      color_type<bpp> color0{};
      color_type<bpp> color1{};
      for (int i = 0; i < bpp; ++i)
      {
         color0.m_components[i] = head_color0.m_components[i];
         color1.m_components[i] = head_color1.m_components[i];
      }
      return { color0, color1 };
   }


   struct div_t { int quot; int rem; };
   [[nodiscard]] constexpr auto div(const int x, const int y) -> div_t
   {
      return {x/y, x%y};
   }


   template<typename T, typename color_type, typename target_type>
   constexpr auto reconstruct(
      const int element_count,
      const uint8_t* source,
      target_type& target,
      const color_type& color0,
      const color_type& color1
   ) -> void
   {
      for (int i = 0; i < element_count; ++i)
      {
         const auto [byte_index, bit_index] = div(i, 8);
         const int left_shift_amount = 7 - bit_index; // 7 is leftest bit
         const uint8_t mask = static_cast<uint8_t>(1ui8 << left_shift_amount);
         const bool binary_value = static_cast<bool>((source[byte_index] & mask) >> left_shift_amount);
         const auto replacement_color = std::bit_cast<T>(binary_value ? color1 : color0);
         target[i] = replacement_color;
      }
   }


   [[nodiscard]] constexpr auto get_byte_count_from_bit_count(
      const int bit_count
   ) -> int
   {
      auto [byte_count, bit_modulo] = div(bit_count, 8);
      if (bit_modulo > 0)
         ++byte_count;
      return byte_count;
   }

   template<typename user_type>
   [[nodiscard]] constexpr auto get_element_count(const header& head) -> int;
   [[nodiscard]] constexpr auto get_element_count(const header& head) -> int;
   
} // namespace bb::detail


template <typename T, int size>
constexpr auto bb::detail::better_array<T, size>::operator[](
   const int index
) const -> const T&
{
   return m_values[index];
}


constexpr auto bb::get_header(
   const uint64_t* source
) -> header
{
   header result;
   if (source == nullptr)
      return result;

   {
      struct front_decoder {
         uint8_t  type;
         uint8_t  bpp;
         uint8_t  padding0[2];
         uint32_t bit_count;
      };

      const front_decoder temp = std::bit_cast<front_decoder>(source[0]);
      result.type = temp.type;
      result.bpp = temp.bpp;
      result.bit_count = temp.bit_count;
   }

   if (result.type > 0)
   {
      const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(source[1]);
      result.width = temp[0];
      result.height = temp[1];
   }
   if (result.type == 2)
   {
      const auto temp = std::bit_cast<detail::better_array<uint32_t, 2>>(source[2]);
      result.color0 = temp[0];
      result.color1 = temp[1];
   }

   return result;
}


constexpr auto bb::get_header(
   const char* name
) -> header
{
   return get_header(get(name));
}


constexpr auto bb::is_image(
   const uint64_t* source
) -> bool
{
   if (source == nullptr)
      return false;
   const auto temp0 = std::bit_cast<detail::better_array<uint8_t, 8>>(source[0]);
   const uint8_t type = temp0[0];
   return type == 1 || type == 2;
}

constexpr auto bb::is_image(
   const char* name
) -> bool
{
   return is_image(get(name));
}


constexpr auto bb::get_width(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
      return 0;
   if (is_image(source) == false)
   {
      return -1;
   }
   const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(source[1]);
   return temp[0];
}


constexpr auto bb::get_width(
   const char* name
) -> int
{
   return get_width(get(name));
}


constexpr auto bb::get_height(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
      return 0;
   if (is_image(source) == false)
   {
      return -1;
   }
   const auto temp = std::bit_cast<detail::better_array<uint16_t, 4>>(source[1]);
   return temp[1];
}


constexpr auto bb::get_height(
   const char* name
) -> int
{
   return get_height(get(name));
}


#ifdef BAKERY_PROVIDE_STD_ARRAY
template<typename user_type, bb::header head, int array_size, int element_count>
constexpr auto bb::decode_to_array(
   const uint64_t(&source)[array_size]
) -> std::array<user_type, element_count>
{
   static_assert(element_count > 0, "Not enough bytes to even fill one of those types.");

   if constexpr (head.type == 2)
   {
      static_assert(sizeof(user_type) == head.bpp, "User type has different size than bpp");
      const auto [color0, color1] = detail::get_sized_color_pair<head.bpp>(head);

      // First cast into its encoded representation
      const int byte_count = detail::get_byte_count_from_bit_count(head.bit_count);
      using interm_type = detail::wrapper_type<uint8_t, byte_count>;
      const std::array<uint8_t, byte_count> interm = std::bit_cast<interm_type>(source).m_data;

      // Then reconstruct the original colors
      using target_type = std::array<user_type, element_count>;
      target_type result;
      detail::reconstruct<user_type>(element_count, &interm[0], result, color0, color1);
      return result;
   }
   else
   {
      using target_type = detail::wrapper_type<user_type, element_count>;
      return std::bit_cast<target_type>(source).m_data;
   }
}
#endif // BAKERY_PROVIDE_STD_ARRAY


#ifdef BAKERY_PROVIDE_VECTOR
template<typename user_type>
auto bb::decode_to_vector(
   const uint64_t* source
) -> std::vector<user_type>
{
   if (source == nullptr)
      return {};

   const header head = bb::get_header(source);
   const int element_count = detail::get_element_count<user_type>(head);
   const int byte_count = element_count * sizeof(user_type);

   std::vector<user_type> result(element_count);
   const uint8_t* data_start_ptr = reinterpret_cast<const uint8_t*>(&source[3]);
   if (head.type == 2)
   {
      constexpr int bpp = sizeof(user_type);
      const auto [color0, color1] = detail::get_sized_color_pair<bpp>(head);
      detail::reconstruct<user_type>(element_count, data_start_ptr, result, color0, color1);
   }
   else
   {
      std::memcpy(result.data(), data_start_ptr, byte_count);
   }
   return result;
}

template<typename user_type>
auto bb::decode_to_vector(const char* name) -> std::vector<user_type>
{
   return decode_to_vector<user_type>(get(name));
}
#endif // BAKERY_PROVIDE_VECTOR


template<typename user_type>
auto bb::decode_into_pointer(
   const uint64_t* source,
   user_type* dst
) -> void
{
   if (source == nullptr || dst == nullptr)
      return;
   const header head = bb::get_header(source);
   const int element_count = get_element_count<user_type>(head);
   const int byte_count = element_count * sizeof(user_type);
   const uint8_t* data_start_ptr = reinterpret_cast<const uint8_t*>(&source[3]);

   if (head.type == 2)
   {
      constexpr int bpp = sizeof(user_type);
      const auto [color0, color1] = detail::get_sized_color_pair<bpp>(head);
      detail::reconstruct<user_type>(element_count, data_start_ptr, dst, color0, color1);
   }
   else
   {
      std::memcpy(dst, data_start_ptr, byte_count);
   }
}


template<typename user_type>
auto bb::decode_into_pointer(
   const char* name,
   user_type* dst
) -> void
{
   return decode_into_pointer<user_type>(get(name), dst);
}


constexpr auto bb::get_element_count(
   const char* name
) -> int
{
   const uint64_t* source = get(name);
   if (source == nullptr)
      return 0;
   return detail::get_element_count(get_header(source));
}


template<typename user_type>
constexpr auto bb::get_element_count(
   const char* name
) -> int
{
   const uint64_t* source = get(name);
   if (source == nullptr)
      return 0;
   return detail::get_element_count<user_type>(get_header(source));
}


template<typename user_type>
constexpr auto bb::get_element_count(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
      return 0;
   return detail::get_element_count<user_type>(get_header(source));
}


constexpr auto bb::get_element_count(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
      return 0;
   return detail::get_element_count(get_header(source));
}


template<typename user_type>
constexpr auto bb::detail::get_element_count(
   const header& head
) -> int
{
   if (head.type == 2)
   {
      return get_element_count(head);
   }
   else
   {
      // Byte count is exact for everything but Indexed images
      const int byte_count = static_cast<int>(head.bit_count) / 8;
      return byte_count / sizeof(user_type);
   }
}


constexpr auto bb::detail::get_element_count(
   const header& head
) -> int
{
   if (head.type == 0)
   {
      throw "Can't be used on non-image payloads without a type.";
   }
   else
   {
      const int pixel_count = head.width * head.height;
      return pixel_count;
   }
}
