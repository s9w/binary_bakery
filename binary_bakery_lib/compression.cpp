#include "compression.h"

#include <zstd_1.5.0/zstd.h>
#include <lz4/lz4.h>


auto bb::get_zstd_compressed(
   const std::vector<uint8_t>& input
) -> std::vector<uint8_t>
{
   const auto compress_bound = ZSTD_compressBound(input.size());

   // "Hint : compression runs faster if `dstCapacity` >=  `ZSTD_compressBound(srcSize)`"
   std::vector<uint8_t> destination(2 * compress_bound);

   // TODO make this configurable
   constexpr int compression_level = 3; // default according to documentation
   const size_t written_comp_size = ZSTD_compress(
      destination.data(),
      destination.capacity(),
      input.data(),
      input.size(),
      compression_level
   );

   if (ZSTD_isError(written_comp_size))
   {
      const char* error_name = ZSTD_getErrorName(written_comp_size);
      printf("ZSTD_compress() error: %s\n", error_name);
      return {};
   }
   destination.resize(written_comp_size);
   return destination;
}


auto bb::get_lz4_compressed(
   const std::vector<uint8_t>& input
) -> std::vector<uint8_t>
{
   std::vector<uint8_t> result(input.size());
   const int compressed_size = LZ4_compress_default(
      std::bit_cast<const char*>(input.data()),
      std::bit_cast<char*>(result.data()),
      static_cast<int>(input.size()),
      static_cast<int>(result.size())
   );
   if (compressed_size == 0)
   {
      printf("Error occured during LZ4 compression.\n");
   }

   result.resize(compressed_size);
   return result;
}
