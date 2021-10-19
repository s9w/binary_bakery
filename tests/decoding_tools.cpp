#include "decoding_tools.h"

#include <exception>

#include <binary_bakery_lib/image.h>

#include <zstd.h>
#include <lz4.h>


using namespace bb;

auto tests::get_image_bytes(
   const bb::abs_file_path& file
) -> std::vector<uint8_t>
{
   image<3> im{ file, image_vertical_direction::bottom_to_top };
   return get_image_bytestream(im);
}


auto tests::get_decode_into_pointer_result(
   const uint64_t* source,
   decompression_fun_type decomp_fun
) -> std::vector<uint8_t>
{
   const auto header = get_header(source);
   const std::vector<uint8_t> bytes_from_payload(header.decompressed_size);
   decode_into_pointer(source, (void*)(bytes_from_payload.data()), decomp_fun);
   return bytes_from_payload;
}


auto tests::lz4_decompression(const void* src, const size_t src_size, void* dst, const size_t dst_size) -> void
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


auto tests::zstd_decompression(const void* src, const size_t srcSize, void* dst, const size_t dst_capacity) -> void
{
   const size_t decompressed_bytes = ZSTD_decompress(dst, dst_capacity, src, srcSize);
   if (decompressed_bytes != dst_capacity)
   {
      std::terminate();
   }
}
