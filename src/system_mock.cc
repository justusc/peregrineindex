#include "peregrine/internal/system.hh"

#include <mutex>

namespace peregrine {
namespace internal {

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)

namespace {

// Track mocked return values for system calls
int open_return_value{-1};
int open_return_count{0};
std::mutex open_mutex;

int close_return_value{-1};
int close_return_count{0};
std::mutex close_mutex;

int fstat_return_value{-1};
int fstat_return_count{0};
std::mutex fstat_mutex;

void* mmap_return_value{MAP_FAILED};
int mmap_return_count{0};
std::mutex mmap_mutex;

int munmap_return_value{-1};
int munmap_return_count{0};
std::mutex munmap_mutex;

StatusCode mock_status_return_value{StatusCode::ok};
int mock_status_count{0};
std::mutex mock_status_mutex;

template <typename resultT, typename funcT, typename... argTs>
resultT mock_system_call(
    std::mutex& mutex, resultT& result, int& count, funcT func, argTs... args) {
  std::lock_guard lock(mutex);
  if(count > 0) {
    --count;
    return result;
  }
  return func(std::forward<argTs>(args)...);
}

} // namespace

void mock_open_return_value(int result, int count) {
  std::lock_guard lock(open_mutex);
  open_return_value = result;
  open_return_count = count;
}

int open(const char* path, int flags, mode_t mode) noexcept {
  return mock_system_call(
      open_mutex, open_return_value, open_return_count, ::open, path, flags, mode);
}

void mock_close_return_value(int result, int count) {
  std::lock_guard lock(close_mutex);
  close_return_value = result;
  close_return_count = count;
}

int close(int fd) noexcept {
  return mock_system_call(close_mutex, close_return_value, close_return_count, ::close, fd);
}

void mock_fstat_return_value(int result, int count) {
  std::lock_guard lock(fstat_mutex);
  fstat_return_value = result;
  fstat_return_count = count;
}

int fstat(int fd, struct stat* buf) noexcept {
  return mock_system_call(fstat_mutex, fstat_return_value, fstat_return_count, ::fstat, fd, buf);
}

void mock_mmap_return_value(void* result, int count) {
  std::lock_guard lock(mmap_mutex);
  mmap_return_value = result;
  mmap_return_count = count;
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept {
  return mock_system_call(mmap_mutex, mmap_return_value, mmap_return_count, ::mmap, addr, length,
      prot, flags, fd, offset);
}

void mock_munmap_return_value(int result, int count = 1) {
  std::lock_guard lock(munmap_mutex);
  munmap_return_value = result;
  munmap_return_count = count;
}

int munmap(void* addr, size_t length) noexcept {
  return mock_system_call(
      munmap_mutex, munmap_return_value, munmap_return_count, ::munmap, addr, length);
}

void mock_errno_to_status_return_value(StatusCode result, int count) {
  std::lock_guard lock(mock_status_mutex);
  mock_status_return_value = result;
  mock_status_count        = count;
}

StatusCode errno_to_status() noexcept {
  {
    std::lock_guard lock(mock_status_mutex);
    if(mock_status_count > 0) {
      --mock_status_count;
      return mock_status_return_value;
    }
  }
  return errno_to_status(errno);
}

#endif // !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
