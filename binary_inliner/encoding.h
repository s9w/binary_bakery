#pragma once

#include <string>

#include "payload.h"


namespace inliner {

   [[nodiscard]] auto encode(const std::string& filename) -> payload;

}
