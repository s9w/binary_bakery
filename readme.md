# Binary bakery
This tool lets you embed binary files (images, fonts etc.) into your C++ *source code* and extract that binary data at compile- or runtime. There are different reasons why you might want this:

- You want to avoid the complexity and effort of loading images with external libraries
- You want faster load times, especially with tiny files
- You want less files to clutter your applications distributed directory or avoid people grabbing files from there

Binary bakery allows the data itself as well as some meta information to be available at compile-time (`constexpr`). It also offers smart integration for images, embedding the pixel colors and dimensions directly instead of just providing the file contents. So you can access image pixels without ever touching image libraries.

## Hot it works
1) A tool translates binary file(s) into C++ source code:

![](readme/encoding_video.gif)

2) You include the resulting header as well as a general decoder header into your code:
```c++
#include <bb_payload.h>
#define BAKERY_PROVIDE_VECTOR
#include <bb_decoder.h>

// All binary information can just be read as bytes.
// Interfaces other than vector also available, see further down
const std::vector<uint8_t> font_bytes = bb::decode_to_vector<uint8_t>("fancy_font.ttf");

// Images have their pixel information encoded directly
struct color { uint8_t r, g, b; };
const std::vector<color> logo_pixels = bb::decode_to_vector<color>("logo.png");

// The data as well as meta information is also available at compile time!
constexpr bb::header meta_information = bb::get_header("logo.png")
```

If decompression code is available in the target codebase, the bytestream can be compressed with that on the encoding side, resulting in less impact on the [compile metrics](#costs-and-benefits).

## Encoder interface and configuration
The tool is a simple binary that tries to encode all files from its command line parameters. There's a `binary_bakery.toml` configuration file, which 

## Decoder interface
The code *requires C++20*. All code (data and decoding functions) come in the `bb` namespace.

A typical payload header looks like this
```c++
namespace bb{
constexpr uint64_t bb_bitmap_font_16_png[]{
   0x0000060000000301, 0x0000000000080008, 0x0000000000000000, 0x0000000000000000, 
   0x0000000000000000, 0x0000000000000000, 0x00003535da3535da, /* ... */
};

// ...
} // namespace bb
```

The `binary_bakery_decoder.h` offers functions to access the header as well as three interfaces for decoding the stream:

#### `auto bb::decode_to_vector(identifier, decomp_fun) -> std::vector<T>`
Returns a vector, already in a type of your choice. If you want to use this interface, you need to `#define BAKERY_PROVIDE_VECTOR` before you include the decoder header.

#### `auto bb::decode_into_pointer(identifier, user_type* dst, decomp_fun) -> void`
Writes into a **preallocated** memory. You can always access the required decompressed size in bytes from `header::decompressed_size`.

#### `constexpr auto bb::decode_to_array(identifier) -> std::array<T, n>` :warning::warning::warning:
This returns a `std::array` of the correct type and size. The array size is determined automagically, but can be explicitly requested from `bb::get_element_count(identifier)`. If you want to use this interface, you need to `#define BAKERY_PROVIDE_STD_ARRAY` before you include the decoder header.

This function is special and dangerous: It allows `constexpr` access to the data, which might be handy for very special purposes. As a result, this returns an array, which stores its data on the stack. Stack memory is a very limited resource (often just 1MB), which can *easily* be exhausted by calling this function on a non-tiny file. That also applies to a runtime-use and will result in a stack overflow. Use this only when you know what you're doing.

Most functions have an `identifier` parameter that can either be a `const uint64_t*` to the payload, or simply the filename.

---

The interface functions all have a `decomp_fun` parameter. That is a function pointer expected to point to a function that deals with the decompression, if decompression was enabled for the payloads. Without compression, the default parameter is a `nullptr`. That function should look like this:
```c++
auto compression_fun(
   const void* src,       // Points to the compressed data
   const size_t src_size, // Size of compressed data in bytes
   void* dst,             // Pointer to the preallocated decompressed data
   const size_t dst_size  // Size of decompressed data in bytes
) -> void;
```

For zstd for example, that would typically contain a call to `ZSTD_decompress(dst, dst_size, src, src_size);`. For Lz4, that might look like `LZ4_decompress_safe(src, dst, src_size, dst_size)`.

If you want to avoid using the provided decoding header altogether, you can access the information yourself. The first 24 bytes contain the header which is defined at the top of the [`binary_bakery_decoder.h`](binary_bakery_decoder.h). Everything after that is the byte stream.

## Costs and benefits
There's two main concerns about embedding non-code data into your source code and resulting binary: Compile times and the size of the resulting binary.

The following measurements were done on a number of png files of different sizes.

The decoder code has been written with minimal includes, requiring only `<bit>`, `<cstdint>`, `<string>` and `<type_traits>`. In most cases, they are already implicitly included and shouldn't add meaningful compile time on its own.

In addition to the decoder code, it's to be expected that the binary size increases at least by the size of the data you add. Luckily compilers seem to do a good job at not adding additional overhead. Below is a graph showing the additional size on top of the size of the embedded include:

![](https://user-images.githubusercontent.com/6044318/136952444-138fa3ef-a6fc-4d14-8bcf-cff2b827dd45.png)

There's a tiny constant binary size added. But no change between payload sizes and thus is probably the decoding code being instantiated.

Compile times are another concern:

![](https://user-images.githubusercontent.com/6044318/136952514-2c6d144d-10ed-4e89-834c-ab6c10b63974.png)

They do increase, but linearly with size (this is a log scale) and moderately - a 3MB payload increases the compile time for the translation unit (TU) it's used in by one second. Keep in mind that the payload should only be included in one TU and the other TUs be compiled in parallel, **reducing the actual time penalty to 1/n** with n being the number of threads compiling. So even with a (compressed) data size of 3MB, it'll add about 0.1 seconds compile time to a real-life project.

Of interest is also the loading speed compared to traditional loading from files. Here's the decoding time relative to the decoding speed from an SSD with [stb_image](https://github.com/nothings/stb):

![](https://user-images.githubusercontent.com/6044318/136952554-5e069b3d-4ddd-4581-9b30-68125585a22a.png)

Note that "memory size" here refers to the uncompressed size. The different compression settings compress that data into smaller bytestreams which benefit and may outweigh the performance cost of decompression. This will highly depend on the data though. My test images may have been a little too easy.

For all sizes and compression methods, this was faster than files. In particular for small files, which should be the main target demographic for this tool.

All these numbers were measured with `/O2` with Visual Studio 16.11.3, a Ryzen 3800X and a Samsung 970 EVO SSD.

## Summary
This is a niche tool. File systems are a good invention and the better choice in most cases.

If this suits your needs however, the tradeoffs are manageable. If used correctly (don't include the payload header in more than one translation unit!), compile times and binary sizes don't suffer much.
