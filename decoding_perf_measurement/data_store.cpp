#include "data_store.h"

#include <zstd_1.5.0/zstd.h>
#include <lz4/lz4.h>

#include "..\sample_datasets\binary_bakery_payload.h"

namespace {
   auto zstd_decompression(
      const void* src,
      const size_t srcSize,
      void* dst,
      const size_t dst_capacity
   ) -> void
   {
      const size_t decompressed_bytes = ZSTD_decompress(dst, dst_capacity, src, srcSize);
      if (decompressed_bytes != dst_capacity)
      {
         std::terminate();
      }
   }


   auto lz4_decompression(
      const void* src,
      const size_t src_size,
      void* dst,
      const size_t dst_size
   ) -> void
   {
      const int decompressed_bytes = LZ4_decompress_safe(
         static_cast<const char*>(src),
         static_cast<char*>(dst),
         static_cast<int>(src_size),
         static_cast<int>(dst_size)
      );
      if (decompressed_bytes != dst_size)
      {
         std::terminate();
      }
   }
}

auto example::decode_to_vector(
   const char* name
) -> std::vector<example::user_type>
{
   //constexpr auto xxx = bb::get_pixel<user_type>(bb::bb_192_png, 4);
   //constexpr auto xxx = bb::get_pixel<user_type>(nullptr, 4);
   const auto yyy = bb::get_pixel<user_type>(nullptr, 4);

   const auto header = bb::get_header(bb::get(name));
   bb::decompression_fun_type decomp_fun = nullptr;
   if (header.compression == 1)
      decomp_fun = zstd_decompression;
   else if (header.compression == 2)
      decomp_fun = lz4_decompression;
   return bb::decode_to_vector<user_type>(bb::get(name), decomp_fun);
}
