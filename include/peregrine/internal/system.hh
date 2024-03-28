#pragma

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../status_code.hh"
#include "common.hh"

namespace peregrine {
namespace internal {

#if !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

// Wrapper around the `open()` system call
// - Parameters:
//  - path: The path to the file to open.
//  - flags: The flags to use when opening the file.
//  - mode: The mode to use when opening the file.
// - Returns: The file descriptor of the opened file, or -1 on error.
inline int open(const char* path, int flags, mode_t mode) noexcept {
  return ::open(path, flags, mode);
}

// Wrapper around the `close()` system call
// - Parameters:
//  - fd: The file descriptor to close.
// - Returns: 0 on success, -1 on error.
inline int close(int fd) noexcept { return ::close(fd); }

// Wrapper around the `fstat()` system call.
// - Parameters:
//  - fd: The file descriptor to get the status of.
//  - buf: The buffer to store the status in.
// - Returns: 0 on success, -1 on error.
inline int fstat(int fd, struct stat* buf) noexcept { return ::fstat(fd, buf); }

// Wrapper around the `mmap()` system call
// - Parameters:
//  - addr: The address to map the file to.
//  - length: The length of the mapping.
//  - prot: The protection flags for the mapping.
//  - flags: The flags for the mapping.
//  - fd: The file descriptor of the file to map.
//  - offset: The offset in the file to start the mapping.
// - Returns: The address of the mapping, or MAP_FAILED on error.
inline void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
  return ::mmap(addr, length, prot, flags, fd, offset);
}

// Wrapper around the `munmap()` system call.
// - Parameters:
//  - addr: The address of the mapping to unmap.
//  - length: The length of the mapping.
// - Returns: 0 on success, -1 on error.
inline int munmap(void* addr, size_t length) { return ::munmap(addr, length); }

// Converts the current value of `errno` to a `StatusCode`.
// - Returns: The `StatusCode` corresponding to the current value of `errno`.
inline StatusCode errno_to_status() noexcept { return errno_to_status(errno); }

#else

// Set the mock return value for `open()` system call
// - Parameters:
//  - result: The value to return when `open()` is called.
//  - count: The number of times to return the value.
void mock_open_return_value(int result, int count = 1);

// Wrapper around the `open()` system call.
//
// This function is a wrapper around the `open()` system call. It is used to mock the system call
// for testing purposes.
// - Parameters:
//  - path: The path to the file to open.
//  - flags: The flags to use when opening the file.
//  - mode: The mode to use when opening the file.
// - Returns: The file descriptor of the opened file, or -1 on error.
int open(const char* path, int flags, mode_t mode) noexcept;

// Set the mock return value for `close()` system call
// - Parameters:
//  - result: The value to return when `close()` is called.
//  - count: The number of times to return the value.
void mock_close_return_value(int result, int count = 1);

// Mock the `close()` system call.
//
// This function is used to mock the `close()` system call for testing purposes.
// - Parameters:
//  - fd: The file descriptor to close.
// - Returns: 0 on success, -1 on error.
int close(int fd) noexcept;

// Set the mock return value for `fstat()` system call
//
// This function is used to mock the return value of the `fstat()` system call for testing purposes.
// - Parameters:
//  - result: The value to return when `fstat()` is called.
//  - count: The number of times to return the value.
void mock_fstat_return_value(int result, int count = 1);

// Mock the `fstat()` system call.
//
// This function is used to mock the `fstat()` system call for testing purposes.
// - Parameters:
//  - fd: The file descriptor to get the status of.
//  - buf: The buffer to store the status in.
// - Returns: 0 on success, -1 on error.
int fstat(int fd, struct stat* buf) noexcept;

// Set the mock return value for `mmap()` system call
// - Parameters:
//  - value: The value to return when `mmap()` is called. Typically this is `MAP_FAILED`.
//  - count: The number of times to return the value.
void mock_mmap_return_value(void* result = MAP_FAILED, int count = 1);

// Wrapper around the `mmap()` system call.
//
// This function is a wrapper around the `mmap()` system call. It is used to mock the system call
// for testing purposes.
// - Parameters:
//  - addr: The address to map the file to.
//  - length: The length of the mapping.
//  - prot: The protection flags for the mapping.
//  - flags: The flags for the mapping.
//  - fd: The file descriptor of the file to map.
//  - offset: The offset in the file to start the mapping.
// - Returns: The address of the mapping, or MAP_FAILED on error.
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);

// Set the mock return value for `munmap()` system call
//
// This function is used to mock the return value of the `munmap()` system call for testing
// purposes.
// - Parameters:
//  - result: The value to return when `munmap()` is called.
//  - count: The number of times to return the value.
void mock_munmap_return_value(int result, int count = 1);

// Mock the `munmap()` system call.
//
// This function is used to mock the `munmap()` system call for testing purposes.
// - Parameters:
//  - addr: The address of the mapping to unmap.
//  - length: The length of the mapping.
// - Returns: 0 on success, -1 on error.
int munmap(void* addr, size_t length) noexcept;

// Set the mock return value for `errno_to_status()` function.
//
// This function is used to mock the return value of the `errno_to_status()` function for testing
// purposes. It should be called in conjunction with other system call mocking functions.
// - Parameters:
//  - result: The value to return when `errno_to_status()` is called.
//  - count: The number of times to return the value.
void mock_errno_to_status_return_value(StatusCode result, int count = 1);

// Mock the `errno_to_status()` function.
//
// This function is used to mock the `errno_to_status()` function for testing purposes.
// - Returns: The `StatusCode` corresponding to the current value of `errno`.
StatusCode errno_to_status() noexcept;

#endif // !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
