#include <cstdint>
#include <vector>
#include <string> // std::memcpy, should be in <vector> already

// TODO this maybe conditional of constexpr is wanted
#include <array>
#include <bit> // for std::bit_cast, should be in <array> already


namespace inliner {
   struct meta {
      uint8_t type = -1;        // 0: Generic binary
                                // 1: image
                                // 2: dual-image (TODO)

      uint8_t bpp = -1;         // Number of channels (1-4)
      uint8_t padding0[6]{};

      uint16_t width = -1;      // Width in pixels
      uint16_t height = -1;     // Height in pixels
      uint16_t byte_count = -1; // Number of bytes stored
      uint16_t padding1 = -1;

      uint32_t color0 = -1;     // Replacement colors
      uint32_t color1 = -1;     // Replacement colors
   };
   static_assert(sizeof(meta) == 24);


   constexpr uint64_t input[]{
      // replace
      0x0000000000000301, 0x00000000006400c8, 0x0000000300000000, 0xfdfdfdfdcd0380ff
   };


   [[nodiscard]] constexpr auto decode_meta(const uint64_t* ptr) -> meta
   {
      meta result;

      const std::array<uint8_t, 8> temp0 = std::bit_cast<std::array<uint8_t, 8>>(ptr[0]);
      result.type = temp0[0];
      result.bpp = temp0[1];

      const std::array<uint16_t, 4> temp1 = std::bit_cast<std::array<uint16_t, 4>>(ptr[1]);
      result.width = temp1[0];
      result.height = temp1[1];
      result.byte_count = temp1[2];

      const std::array<uint32_t, 2> temp2 = std::bit_cast<std::array<uint32_t, 2>>(ptr[2]);
      result.color0 = temp2[0];
      result.color1 = temp2[1];

      return result;
   }

}


namespace user {
   struct user_type {
      uint8_t r, g, b;
   };
   //const inliner::runtime_target_type<user_type> x = inliner::get_runtime<user_type>();
}


#include <doctest/doctest.h>

TEST_CASE("dummy") {
   //sizeof(inliner::meta);
   //constexpr auto o0 = offsetof(inliner::meta, type);
   //constexpr auto o1 = offsetof(inliner::meta, bpp);
   //constexpr auto o2 = offsetof(inliner::meta, waste0);
   constexpr int mmax = std::numeric_limits<uint16_t>::max();

   constexpr inliner::meta result = inliner::decode_meta(inliner::input);
   int end = 0;
}
