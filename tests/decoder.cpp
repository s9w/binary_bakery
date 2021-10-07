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
   static_assert(sizeof(meta) == 24);

   template<typename T, int i>
   struct std_array {
      T m_values[i];
      [[nodiscard]] constexpr auto operator[](const int index) const -> const T&
      {
         return m_values[index];
      }
   };


   [[nodiscard]] constexpr auto decode_meta(
      const uint64_t* ptr
   ) -> meta
   {
      meta result;

      const std_array<uint8_t, 8> temp0 = std::bit_cast<std_array<uint8_t, 8>>(ptr[0]);
      result.type = temp0[0];
      result.bpp = temp0[1];

      const std_array<uint16_t, 4> temp1 = std::bit_cast<std_array<uint16_t, 4>>(ptr[1]);
      result.width = temp1[0];
      result.height = temp1[1];
      result.byte_count = temp1[2];

      const std_array<uint32_t, 2> temp2 = std::bit_cast<std_array<uint32_t, 2>>(ptr[2]);
      result.color0 = temp2[0];
      result.color1 = temp2[1];

      return result;
   }


   [[nodiscard]] constexpr auto is_image(
      const uint64_t* ptr
   ) -> bool
   {
      const std_array<uint8_t, 8> temp0 = std::bit_cast<std_array<uint8_t, 8>>(ptr[0]);
      const uint8_t type = temp0[0];
      return type == 0 || type == 1;
   }


   [[nodiscard]] constexpr auto get_width(
      const uint64_t* ptr
   ) -> int
   {
      if(is_image(ptr) == false)
      {
         return -1;
      }
      const std_array<uint16_t, 4> temp1 = std::bit_cast<std_array<uint16_t, 4>>(ptr[1]);
      return temp1[0];
   }


   [[nodiscard]] constexpr auto get_height(
      const uint64_t* ptr
   ) -> int
   {
      if (is_image(ptr) == false)
      {
         return -1;
      }
      const std_array<uint16_t, 4> temp1 = std::bit_cast<std_array<uint16_t, 4>>(ptr[1]);
      return temp1[1];
   }


   template<typename user_type, int element_count>
   struct wrapper_type {
      uint64_t fillter[3];
      std_array<user_type, element_count> m_data; // needs to be returned
      // todo end padding
   };


   template<typename user_type, int byte_count, int source_array_size, int element_count = byte_count/sizeof(user_type)>
   [[nodiscard]] constexpr auto decode_content(
      const uint64_t(&source_array)[source_array_size]
   ) -> std_array<user_type, element_count>
   {
      using target_type = wrapper_type<user_type, element_count>;

      target_type result = std::bit_cast<target_type>(source_array);
      return result.m_data;
   }

} // namespace inliner


namespace user
{
   struct user_type {
      uint64_t xxx;
      //uint8_t r, g, b;
   };

   auto user_fun()
   {
      #include "user_in_test.cpp"
      constexpr inliner::meta decoded_meta = inliner::decode_meta(input0);
      static_assert(decoded_meta.byte_count == 8);
      const auto content = inliner::decode_content<user_type, decoded_meta.byte_count>(input0);

      int end = 0;
   }
}


#include <doctest/doctest.h>

TEST_CASE("dummy")
{
   user::user_fun();

   int end = 0;
}
