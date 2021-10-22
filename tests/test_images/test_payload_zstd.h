#include <cstdint>
#include <cstring>

namespace bb{
static constexpr uint64_t bb_binary0_bin[]{
   0x0000000000000100, 0x0000010a00000100, 0x01000060fd2fb528, 0x3a0c921647db0008,
   0x07fc6f89bc3c9046, 0x26874e44c1b30b0e, 0xdb2da851f98079fd, 0xaa9d3c8a40fac673,
   0xe53b1f8d64cd3b9f, 0xd4a8e54e1e4f511d, 0x46d9bd2a404d9929, 0x7d5e469bf0c6c79c,
   0x8d3af11e64890d6b, 0x2d1140cffaf2437a, 0xaeb4e36a2f6f7532, 0x707672655b260a80,
   0x6924c2049b34b04d, 0x7e95496ae48ed936, 0x17a342a29d3b777b, 0x2213f5d991c83683,
   0xabdf2c5f611e7c79, 0x2945b7abc54e83ae, 0x1f6927522fadee66, 0xa99096ef62cd1c4b,
   0x522d81c0f922f0a3, 0x5233d8d33e9d8e0a, 0xd886b7d9f9ddfb97, 0x8be82b92665b63b7,
   0xc093e87f41ea3e77, 0x9d2b068f675bf2f5, 0xe019a8e3bf2cd520, 0x50ee8fdd366f52f1,
   0x06eda5095a3e5c2a, 0xb131610de48a3b05, 0x9a57324e30aede43, 0x0000134e0400c4e6
};
static constexpr uint64_t bb_green_png[]{
   0x0002000303000101, 0x0000001200000012, 0x004d1220fd2fb528, 0x76000100ff001800,
   0x000000000000086e
};


[[nodiscard]] static constexpr auto get_payload(
   [[maybe_unused]] std::string_view name
) -> const uint64_t*
{
   if(name == "binary0.bin")
      return &bb_binary0_bin[0];
   else if(name == "green.png")
      return &bb_green_png[0];
   else
      return nullptr;
}

} // namespace bb
