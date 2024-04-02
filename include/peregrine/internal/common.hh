#pragma once

#define PEREGRINE_LIKELY(x)    __builtin_expect(!!(x), 1)
#define PEREGRINE_UNLIKELY(x)  __builtin_expect(!!(x), 0)

#define PEREGRINE_FORCE_INLINE __attribute__((always_inline)) inline

#include <string>
#include <string_view>

namespace peregrine {

using namespace std::string_view_literals;
using namespace std::string_literals;

} // namespace peregrine
