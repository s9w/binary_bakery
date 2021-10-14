#pragma once

#include <vector>

#define BAKERY_PROVIDE_VECTOR
#include <binary_bakery_decoder.h>

namespace bb {
   struct abs_file_path;
}

namespace tests
{

   auto get_image_bytes(const bb::abs_file_path& file) -> std::vector<uint8_t>;

   template<typename T>
   auto get_decode_to_vector_result(const uint64_t*, bb::decompression_fun_type decomp_fun) -> std::vector<uint8_t>;

   auto get_decode_into_pointer_result(const uint64_t* source, bb::decompression_fun_type decomp_fun) -> std::vector<uint8_t>;

   auto lz4_decompression(const void* src, const size_t src_size, void* dst, const size_t dst_size) -> void;
   auto zstd_decompression(const void* src, const size_t srcSize, void* dst, const size_t dst_capacity) -> void;

}

template<typename T>
auto tests::get_decode_to_vector_result(
   const uint64_t* data,
   bb::decompression_fun_type decomp_fun
) -> std::vector<uint8_t>
{
   const std::vector<T> decoded = bb::decode_to_vector<T>(data, decomp_fun);
   const size_t byte_count = decoded.size() * sizeof(T);
   std::vector<uint8_t> bytes_from_payload(byte_count);
   std::memcpy(bytes_from_payload.data(), decoded.data(), byte_count);
   return bytes_from_payload;
}