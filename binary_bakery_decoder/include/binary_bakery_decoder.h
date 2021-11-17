#pragma once

#include <bit>             // For std::bit_cast
#include <cstdint>         // For sized types
#include <cstring>
#ifdef __GNUG__
#include <experimental/source_location> // For source locations of errors
namespace std{
using source_location = experimental::source_location;
}
#else
#include <source_location> // For source locations of errors
#endif
#include <string>          // For std::memcpy
#include <type_traits>     // For add_pointer, just to look nice

#ifdef    BAKERY_PROVIDE_VECTOR
#include <vector>
#endif // BAKERY_PROVIDE_VECTOR


namespace bb {

   struct header {
      uint8_t  type = 0;        // 0: Generic binary
                                // 1: Image
      
      uint8_t  compression = 0; // Compression mode
                                // 0: No compression
                                // 1: zstd
                                // 2: LZ4
      uint8_t  version = 0;     // Version of the payload format. If everything goes well, this stays at 0.
      uint8_t  bpp = 0;         // For images: Number of channels [1-4]
      uint16_t width = 0;       // For images: Width in pixels [0-65535]
      uint16_t height = 0;      // For images: Height in pixels [0-65535]
      
      // Word border

      uint32_t decompressed_size = 0; // Size of the data stream (without this header)
      uint32_t compressed_size   = 0; // Without compression, this is equal to the uncompressed_size
                                      // range: [0-4096 MB]
   };
   static_assert(sizeof(header) == 2 * sizeof(uint64_t));

   // Retrieves the header from a payload.
   [[nodiscard]] constexpr auto get_header(const uint64_t* source) -> header;

   // Retrieves parts of the header. Just for convenience.
   [[nodiscard]] constexpr auto is_image  (const uint64_t* source) -> bool;
   [[nodiscard]] constexpr auto get_width (const uint64_t* source) -> int;
   [[nodiscard]] constexpr auto get_height(const uint64_t* source) -> int;

   // These methods provide easy access to the number of elements in the dataset. In images, that is equal to the number
   // of pixels. Therefore this function can be called without template parameter with image payloads.
   // In generic binaries, that's the number of elements of the target type. That needs to be provided as template
   // parameter.
   template<typename user_type>
   [[nodiscard]] constexpr auto get_element_count(const uint64_t* source) -> int;
   [[nodiscard]] constexpr auto get_element_count(const uint64_t* source) -> int;

   // Compile-time access to the stored elements. For images, provide a color type.
   template<typename user_type>
   [[nodiscard]] constexpr auto get_element(const uint64_t* source, const int index) -> user_type;

   using decompression_fun_type = std::add_pointer_t<void(const void* src, const size_t src_size, void* dst, const size_t dst_capacity)>;

#ifdef    BAKERY_PROVIDE_VECTOR
   // Returns an std::vector of provided type.
   template<typename user_type>
   [[nodiscard]] auto decode_to_vector(const uint64_t* source, decompression_fun_type decomp_fun = nullptr) -> std::vector<user_type>;
#endif // BAKERY_PROVIDE_VECTOR

   // This writes into an arbitrary container-pointer. No memory management - needs to be allocated!
   inline auto decode_into_pointer(const uint64_t* source, void* dst, decompression_fun_type decomp_fun = nullptr) -> void;

   [[nodiscard]] constexpr auto get_data_ptr(const uint64_t* source) -> const void*;

   using error_callback_type = void(*)(std::string_view msg, const std::source_location& location);
   inline error_callback_type error_callback = nullptr;

} // namespace bb


namespace bb::detail {

   inline auto error(std::string_view msg, const std::source_location& location) -> void;

   template<typename T, int size>
   struct better_array {
      T m_values[size];
      [[nodiscard]] constexpr auto operator[](const int index) const -> const T&;
      [[nodiscard]] constexpr auto operator[](const int index)       ->       T&;
   };

   // Construct objects of types that might not be default-consructible
   template<typename user_type>
   [[nodiscard]] constexpr auto get_nulled_object() -> user_type;

   template<typename user_type, int element_count>
   struct wrapper_type {
      uint64_t filler[2];
      alignas(user_type) better_array<user_type, element_count> m_data; // needs to be returned
      // End padding should be unnecessary because of array alignment
   };

   template<int bpp>
   struct color_type {
      uint8_t m_components[bpp];
   };


   struct div_t { int quot; int rem; };
   [[nodiscard]] constexpr auto div(const int x, const int y) -> div_t
   {
      return {x/y, x%y};
   }
   
} // namespace bb::detail


template <typename T, int size>
constexpr auto bb::detail::better_array<T, size>::operator[](
   const int index
) const -> const T&
{
   return m_values[index];
}


template <typename T, int size>
constexpr auto bb::detail::better_array<T, size>::operator[](
   const int index
) -> T&
{
   return m_values[index];
}


constexpr auto bb::get_header(
   const uint64_t* source
) -> header
{
   
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return header{};
   }

   header result;
   {
      struct front_decoder {
         uint8_t  type = 0;
         uint8_t  compression = 0; 
         uint8_t  version = 0;
         uint8_t  bpp = 0;
         uint16_t width = 0;
         uint16_t height = 0;
      };

      const auto temp = std::bit_cast<front_decoder>(source[0]);
      result.type = temp.type;
      result.compression = temp.compression;
      result.version = temp.version;
      result.bpp = temp.bpp;
      result.width = temp.width;
      result.height = temp.height;
   }
   {
      const auto temp = std::bit_cast<detail::better_array<uint32_t, 2>>(source[1]);
      result.decompressed_size = temp[0];
      result.compressed_size = temp[1];
   }

   return result;
}


constexpr auto bb::is_image(
   const uint64_t* source
) -> bool
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return false;
   }
   const header head = get_header(source);
   return head.type == 1;
}


constexpr auto bb::get_width(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return 0;
   }
   if (is_image(source) == false)
   {
      detail::error("get_width() called on a non-image payload", std::source_location::current());
      return 0;
   }
   const header head = get_header(source);
   return head.width;
}


constexpr auto bb::get_height(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return 0;
   }
   if (is_image(source) == false)
   {
      detail::error("get_height() called on a non-image payload", std::source_location::current());
      return 0;
   }
   const header head = get_header(source);
   return head.height;
}


template<typename user_type>
constexpr auto bb::get_element(
   const uint64_t* source,
   const int index
) -> user_type
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return detail::get_nulled_object<user_type>();
   }

   if (bb::get_header(source).compression != 0)
   {
      detail::error("Payload is compressed. This interface only works with uncompressed payloads", std::source_location::current());
      return detail::get_nulled_object<user_type>();
   }

   if (index >= get_element_count<user_type>(source) || index < 0)
   {
      detail::error("Index is out of bounds", std::source_location::current());
      return detail::get_nulled_object<user_type>();
   }


   // Use intermediate type because user-type might not be default constructible and not have operator[]
   detail::better_array<uint8_t, sizeof(user_type)> proxy{};

   using word_bytes_type = detail::better_array<uint8_t, 8>;
   for (int i = 0; i < sizeof(user_type); ++i)
   {
      const int byte_offset = index * sizeof(user_type) + i;
      const auto [word_index, byte_index] = detail::div(byte_offset, static_cast<int>(sizeof(uint64_t)));
      const auto word_bytes = std::bit_cast<word_bytes_type>(source[2 + word_index]);
      proxy[i] = word_bytes[byte_index];
   }
   return std::bit_cast<user_type>(proxy);
}


#ifdef BAKERY_PROVIDE_VECTOR
template<typename user_type>
auto bb::decode_to_vector(
   const uint64_t* source,
   decompression_fun_type decomp_fun
) -> std::vector<user_type>
{
   static_assert(std::is_default_constructible_v<user_type>, "decode_to_vector() requires the type to be default constructible. If that's a problem, file an issue. Should be possible (potentially slower).");
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return {};
   }

   const header head = bb::get_header(source);
   const int element_count = get_element_count<user_type>(source);
   std::vector<user_type> result(element_count);
   
   if (head.compression == 0)
   {
      std::memcpy(result.data(), get_data_ptr(source), head.decompressed_size);
   }
   else if (head.compression > 0)
   {
      if (decomp_fun == nullptr)
      {
         detail::error("Payload is compressed, but the decompression_fun parameter is nullptr.", std::source_location::current());
         return {};
      }
      decomp_fun(get_data_ptr(source), head.compressed_size, result.data(), head.decompressed_size);
   }
   return result;
}
#endif // BAKERY_PROVIDE_VECTOR


auto bb::decode_into_pointer(
   const uint64_t* source,
   void* dst,
   decompression_fun_type decomp_fun
) -> void
{
   if (source == nullptr)
   {
      detail::error("Source is nullptr", std::source_location::current());
      return;
   }
   if (dst == nullptr)
   {
      detail::error("Destination is nullptr", std::source_location::current());
      return;
   }
   const header head = bb::get_header(source);
   if (head.compression > 0)
   {
      decomp_fun(get_data_ptr(source), head.compressed_size, dst, head.decompressed_size);
   }
   else
   {
      const int byte_count = head.decompressed_size;
      std::memcpy(dst, get_data_ptr(source), byte_count);
   }
}


constexpr auto bb::get_element_count(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return 0;
   }

   if (is_image(source) == false)
   {
      detail::error("Can't call get_element_count() without template param for non-images.", std::source_location::current());
      return 0;
   }
   const header head = get_header(source);
   return head.width * head.height;
}


template<typename user_type>
constexpr auto bb::get_element_count(
   const uint64_t* source
) -> int
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return 0;
   }
   const header head = get_header(source);
   return head.decompressed_size / sizeof(user_type);
}


template<typename user_type>
constexpr auto bb::detail::get_nulled_object() -> user_type
{
   return std::bit_cast<user_type>(detail::better_array<uint8_t, sizeof(user_type)>{});
}


constexpr auto bb::get_data_ptr(const uint64_t* source) -> const void*
{
   if (source == nullptr)
   {
      detail::error("Source was nullptr", std::source_location::current());
      return nullptr;
   }

   constexpr auto header_size = sizeof(header);
   static_assert(header_size % sizeof(uint64_t) == 0);

   return &source[sizeof(header)/sizeof(uint64_t)];
}


auto bb::detail::error(
   std::string_view msg,
   const std::source_location& location
) -> void
{
   if (error_callback != nullptr)
   {
      error_callback(msg, location);
   }
}
