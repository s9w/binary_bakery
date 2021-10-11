#include "compression.h"

#include <zstd_1.5.0/zstd.h>

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
