#pragma once

#define PEREGRINE_LIKELY(x)   __builtin_expect(!!(x), 1)
#define PEREGRINE_UNLIKELY(x) __builtin_expect(!!(x), 0)
