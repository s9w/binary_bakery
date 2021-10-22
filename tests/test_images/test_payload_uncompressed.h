#include <cstdint>
#include <cstring>

namespace bb{
static constexpr uint64_t bb_binary0_bin[]{
   0x0000000000000000, 0x0000010000000100, 0x90463a0c921647db, 0x0b0e07fc6f89bc3c,
   0x79fd26874e44c1b3, 0xc673db2da851f980, 0x3b9faa9d3c8a40fa, 0x511de53b1f8d64cd,
   0x9929d4a8e54e1e4f, 0xc79c46d9bd2a404d, 0x0d6b7d5e469bf0c6, 0x437a8d3af11e6489,
   0x75322d1140cffaf2, 0x0a80aeb4e36a2f6f, 0xb04d707672655b26, 0xd9366924c2049b34,
   0x777b7e95496ae48e, 0x368317a342a29d3b, 0x7c792213f5d991c8, 0x83aeabdf2c5f611e,
   0xee662945b7abc54e, 0x1c4b1f6927522fad, 0xf0a3a99096ef62cd, 0x8e0a522d81c0f922,
   0xfb975233d8d33e9d, 0x63b7d886b7d9f9dd, 0x3e778be82b92665b, 0xf2f5c093e87f41ea,
   0xd5209d2b068f675b, 0x52f1e019a8e3bf2c, 0x5c2a50ee8fdd366f, 0x3b0506eda5095a3e,
   0xde43b131610de48a, 0xc4e69a57324e30ae
};
static constexpr uint64_t bb_red_png[]{
   0x0002000303000001, 0x0000001200000012, 0x00ff0000ff0000ff, 0xff0000ff0000ff00,
   0x0000000000000000
};

[[nodiscard]] static constexpr auto get_payload(
   [[maybe_unused]] std::string_view name
) -> const uint64_t*
{
   if(name == "binary0.bin")
      return &bb_binary0_bin[0];
   else if(name == "red.png")
      return &bb_red_png[0];
   else
      return nullptr;
}

} // namespace bb
