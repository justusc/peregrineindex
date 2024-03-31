#include "peregrine/internal/system.hh"

#include <mutex>

#include "peregrine/internal/log.hh"

namespace peregrine {
namespace internal {

#if defined(PEREGRINE_MOCK_SYSTEM_CALLS)

namespace {

StatusCode errno_to_status_impl() noexcept {
  return static_cast<StatusCode>(static_cast<int32_t>(errno));
}

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

template <typename funcT>
class MockSystemCall {
  using result_t = typename function_traits<funcT>::return_type;

  std::string name;
  funcT func;
  result_t result;
  result_t def_result;
  int return_count{};
  size_t call_count{};
  mutable std::mutex mutex{};

public:
  MockSystemCall(funcT func, result_t result, std::string name) :
      name{std::move(name)}, func{func}, result{result}, def_result{result} {}

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

MockSystemCall<decltype(&::open)> open_mock{&::open, -1, "open"};
MockSystemCall<decltype(&::close)> close_mock{&::close, -1, "close"};
MockSystemCall<decltype(&::fstat)> fstat_mock{&::fstat, -1, "fstat"};
MockSystemCall<decltype(&::read)> read_mock{&::read, -1, "read"};
MockSystemCall<decltype(&::write)> write_mock{&::write, -1, "write"};
MockSystemCall<decltype(&::lseek)> lseek_mock{&::lseek, -1, "lseek"};
MockSystemCall<decltype(&::dup)> dup_mock{&::dup, -1, "dup"};
MockSystemCall<decltype(&::mmap)> mmap_mock{&::mmap, MAP_FAILED, "mmap"};
MockSystemCall<decltype(&::munmap)> munmap_mock{&::munmap, -1, "munmap"};
MockSystemCall<decltype(&errno_to_status_impl)> errno_to_status_mock{
    &errno_to_status_impl, StatusCode::ok, "errno_to_status"};

} // namespace

void mock_reset() {
  open_mock.reset();
  close_mock.reset();
  fstat_mock.reset();
  read_mock.reset();
  write_mock.reset();
  lseek_mock.reset();
  dup_mock.reset();
  mmap_mock.reset();
  munmap_mock.reset();
  errno_to_status_mock.reset();
}

void mock_open_return_value(int result, int count) { open_mock.mock_return_value(result, count); }
size_t mock_open_call_count() { return open_mock.get_call_count(); }
int open(const char* path, int flags, mode_t mode) noexcept { return open_mock(path, flags, mode); }

void mock_close_return_value(int result, int count) { close_mock.mock_return_value(result, count); }
size_t mock_close_call_count() { return close_mock.get_call_count(); }
int close(int fd) noexcept { return close_mock(fd); }

void mock_fstat_return_value(int result, int count) { fstat_mock.mock_return_value(result, count); }
size_t mock_fstat_call_count() { return fstat_mock.get_call_count(); }
int fstat(int fd, struct stat* buf) noexcept { return fstat_mock(fd, buf); }

void mock_read_return_value(int result, int count) { read_mock.mock_return_value(result, count); }
size_t mock_read_call_count() { return read_mock.get_call_count(); }
ssize_t read(int fd, void* buf, size_t count) noexcept { return read_mock(fd, buf, count); }

void mock_write_return_value(int result, int count) { write_mock.mock_return_value(result, count); }
size_t mock_write_call_count() { return write_mock.get_call_count(); }
ssize_t write(int fd, const void* buf, size_t count) noexcept { return write_mock(fd, buf, count); }

void mock_lseek_return_value(off_t result, int count) {
  lseek_mock.mock_return_value(result, count);
}
size_t mock_lseek_call_count() { return lseek_mock.get_call_count(); }
off_t lseek(int fd, off_t offset, int whence) noexcept { return lseek_mock(fd, offset, whence); }

void mock_dup_return_value(int result, int count) { dup_mock.mock_return_value(result, count); }
size_t mock_dup_call_count() { return dup_mock.get_call_count(); }
int dup(int oldfd) noexcept { return dup_mock(oldfd); }

void mock_mmap_return_value(void* result, int count) { mmap_mock.mock_return_value(result, count); }
size_t mock_mmap_call_count() { return mmap_mock.get_call_count(); }
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept {
  return mmap_mock(addr, length, prot, flags, fd, offset);
}

void mock_munmap_return_value(int result, int count) {
  munmap_mock.mock_return_value(result, count);
}
size_t mock_munmap_call_count() { return munmap_mock.get_call_count(); }
int munmap(void* addr, size_t length) noexcept { return munmap_mock(addr, length); }

void mock_errno_to_status_return_value(StatusCode result, int count) {
  errno_to_status_mock.mock_return_value(result, count);
}
size_t mock_errno_to_status_call_count() { return errno_to_status_mock.get_call_count(); }
StatusCode errno_to_status() noexcept { return errno_to_status_mock(); }

#endif // !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
