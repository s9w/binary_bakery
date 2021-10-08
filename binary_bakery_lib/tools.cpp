#include "tools.h"

#include <format>

auto bb::get_ui64_str(const uint64_t value) -> std::string
{
   return std::format("{:#018x}", value);
}
