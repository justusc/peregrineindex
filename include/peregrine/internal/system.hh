#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)
#include <mutex>
#endif // defined(PEREGRINE_MOCK_SYSTEM_CALLS)

#include "../status_code.hh"
#include "common.hh"
#include "type_traits.hh"

namespace peregrine {
namespace internal {

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)

template <typename funcT>
class MockSystemCall {
  using result_t = typename function_traits<funcT>::return_type;

  funcT func;
  std::string name;
  result_t result;
  result_t def_result;
  int return_count{};
  size_t call_count{};
  mutable std::mutex mutex{};

public:
  MockSystemCall(funcT func, result_t result, std::string name) :
      func{func}, result{result}, def_result{result} {}

  template <typename... argTs>
  result_t operator()(argTs&&... args) noexcept {
    std::lock_guard lock(mutex);
    ++call_count;
    if(return_count > 0) {
      PEREGRINE_LOG_TRACE(
          "Mock system call name={}, result={}, count={}", name, result, return_count);
      --return_count;
      return result;
    }

    return func(std::forward<argTs>(args)...);
  }

  void mock_return_value(result_t result, int count) noexcept {
    std::lock_guard lock(mutex);
    this->result = result;
    return_count = count;
  }

  void mock_return_value() noexcept { mock_return_value(def_result, 1); }

  size_t get_call_count() const noexcept {
    std::lock_guard lock(mutex);
    return call_count;
  }

  void reset() noexcept {
    std::lock_guard lock(mutex);
    result       = def_result;
    return_count = 0;
    call_count   = 0;
  }
}; // class MockSystemCall

#define PEREGRINE_MOCK_SYSTEM_CALL(func) extern MockSystemCall<decltype(&func)> func

#else

#define PEREGRINE_MOCK_SYSTEM_CALL(func)                                                           \
  template <typename... argTs>                                                                     \
  PEREGRINE_FORCE_INLINE auto func(argTs&&... args) noexcept                                       \
      -> decltype(func(std::forward<argTs>(args)...)) {                                            \
    return func(std::forward<argTs>(args)...);                                                     \
  }

#endif // !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

PEREGRINE_MOCK_SYSTEM_CALL(open);
PEREGRINE_MOCK_SYSTEM_CALL(close);
PEREGRINE_MOCK_SYSTEM_CALL(fstat);
PEREGRINE_MOCK_SYSTEM_CALL(read);
PEREGRINE_MOCK_SYSTEM_CALL(pread);
PEREGRINE_MOCK_SYSTEM_CALL(readv);
PEREGRINE_MOCK_SYSTEM_CALL(preadv);
PEREGRINE_MOCK_SYSTEM_CALL(write);
PEREGRINE_MOCK_SYSTEM_CALL(pwrite);
PEREGRINE_MOCK_SYSTEM_CALL(writev);
PEREGRINE_MOCK_SYSTEM_CALL(pwritev);
PEREGRINE_MOCK_SYSTEM_CALL(lseek);
PEREGRINE_MOCK_SYSTEM_CALL(dup);
PEREGRINE_MOCK_SYSTEM_CALL(fsync);
PEREGRINE_MOCK_SYSTEM_CALL(mmap);
PEREGRINE_MOCK_SYSTEM_CALL(munmap);

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)

extern MockSystemCall<StatusCode (*)() noexcept> errno_to_status;

// Resets all the mock system calls.
inline void reset_mocks() {
  ::peregrine::internal::open.reset();
  ::peregrine::internal::close.reset();
  ::peregrine::internal::fstat.reset();
  ::peregrine::internal::read.reset();
  ::peregrine::internal::pread.reset();
  ::peregrine::internal::readv.reset();
  ::peregrine::internal::preadv.reset();
  ::peregrine::internal::write.reset();
  ::peregrine::internal::pwrite.reset();
  ::peregrine::internal::writev.reset();
  ::peregrine::internal::pwritev.reset();
  ::peregrine::internal::lseek.reset();
  ::peregrine::internal::dup.reset();
  ::peregrine::internal::mmap.reset();
  ::peregrine::internal::munmap.reset();
  ::peregrine::internal::errno_to_status.reset();
}

#else

PEREGRINE_FORCE_INLINE StatusCode errno_to_status() noexcept {
  return static_cast<StatusCode>(static_cast<int32_t>(errno));
}

#endif // defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
