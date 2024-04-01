#pragma once

namespace peregrine {
namespace internal {

template <typename T>
struct function_traits;

template <typename returnT, typename... Args>
struct function_traits<returnT (*)(Args...)> {
  using return_type = returnT;
}; // struct function_traits

template <typename returnT, typename... Args>
struct function_traits<returnT (*)(Args...) noexcept> {
  using return_type = returnT;
}; // struct function_traits

template <typename returnT, typename arg1T, typename arg2T>
struct function_traits<returnT (*)(arg1T, arg2T, ...)> {
  using return_type = returnT;
}; // struct function_traits

} // namespace internal
} // namespace peregrine
