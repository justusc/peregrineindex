#include "peregrine/internal/system.hh"

#include <mutex>

#include "peregrine/internal/log.hh"

namespace peregrine {
namespace internal {

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)

#define PEREGRINE_MOCK_SYSTEM_CALL_IMPL(func, return_default)                                      \
  MockSystemCall<decltype(&::func)> func { &::func, return_default, #func }

PEREGRINE_MOCK_SYSTEM_CALL_IMPL(open, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(close, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(fstat, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(read, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(pread, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(readv, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(preadv, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(write, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(pwrite, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(writev, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(pwritev, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(lseek, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(dup, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(fsync, -1);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(mmap, MAP_FAILED);
PEREGRINE_MOCK_SYSTEM_CALL_IMPL(munmap, -1);

// Do some special handling for errno_to_status since it is not a system call.
namespace {
StatusCode errno_to_status_impl() noexcept {
  return static_cast<StatusCode>(static_cast<int32_t>(errno));
}
} // namespace
MockSystemCall<decltype(&errno_to_status_impl)> errno_to_status{
    &errno_to_status_impl, StatusCode::ok, "errno_to_status"};

#endif // defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
