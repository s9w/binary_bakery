#include <binary_bakery_lib/compression.h>

#include <zstd.h>
#include <lz4.h>
#include <bit>
#include <cstdio>
#include <string_view>

auto bb::get_zstd_compressed(
   const std::vector<uint8_t>& input
) -> std::vector<uint8_t>
{
   const auto compress_bound = ZSTD_compressBound(input.size());

   // "Hint : compression runs faster if `dstCapacity` >=  `ZSTD_compressBound(srcSize)`"
   std::vector<uint8_t> destination(2 * compress_bound);

   constexpr int compression_level = 3; // default according to documentation
   const size_t written_comp_size = ZSTD_compress(
      destination.data(),
      destination.size(),
      input.data(),
      input.size(),
      compression_level
   );

   if (ZSTD_isError(written_comp_size))
   {
      std::string_view error_name = ZSTD_getErrorName(written_comp_size);
      printf("ZSTD_compress() error: %s\n", error_name.data());
      return {};
   }
   destination.resize(written_comp_size);
   return destination;
}


auto bb::get_lz4_compressed(
   const std::vector<uint8_t>& input
) -> std::vector<uint8_t>
{
   const int target_size_bount = LZ4_compressBound(static_cast<int>(input.size()));
   std::vector<uint8_t> result(target_size_bount);
   
   const int compressed_size = LZ4_compress_default(
      std::bit_cast<const char*>(input.data()),
      std::bit_cast<char*>(result.data()),
      static_cast<int>(input.size()),
      static_cast<int>(result.size())
   );
   if (compressed_size == 0)
   {
      printf("Error occured during LZ4 compression.\n");
      return {};
   }

   result.resize(compressed_size);
   return result;
}
