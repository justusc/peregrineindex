#pragma once

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

// Wrapper around the `read()` system call.
// - Parameters:
//  - fd: The file descriptor to read from.
//  - buf: The buffer to read into.
//  - count: The number of bytes to read.
// - Returns: The number of bytes read, or -1 on error.
inline ssize_t read(int fd, void* buf, size_t count) noexcept { return ::read(fd, buf, count); }

// Wrapper around the `write()` system call.
// - Parameters:
//  - fd: The file descriptor to write to.
//  - buf: The buffer to write from.
//  - count: The number of bytes to write.
// - Returns: The number of bytes written, or -1 on error.
inline ssize_t write(int fd, const void* buf, size_t count) noexcept {
  return ::write(fd, buf, count);
}

// Wrapper around the `lseek()` system call.
// - Parameters:
//  - fd: The file descriptor to seek in.
//  - offset: The offset to seek to.
//  - whence: The origin of the seek.
// - Returns: The new offset in the file, or -1 on error.
inline off_t lseek(int fd, off_t offset, int whence) noexcept {
  return ::lseek(fd, offset, whence);
}

// Wrapper around the `dup()` system call.
// - Parameters:
//  - oldfd: The file descriptor to duplicate.
// - Returns: The new file descriptor, or -1 on error.
inline int dup(int oldfd) noexcept { return ::dup(oldfd); }

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
inline StatusCode errno_to_status() noexcept {
  return static_cast<StatusCode>(static_cast<int32_t>(errno));
}

#else

// Set the mock return value for `open()` system call
// - Parameters:
//  - result: The value to return when `open()` is called.
//  - count: The number of times to return the value.
void mock_open_return_value(int result = -1, int count = 1);

// Get the number of times `open()` was called.
size_t mock_open_call_count();

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
void mock_close_return_value(int result = -1, int count = 1);

// Get the number of times `close()` was called.
size_t mock_close_call_count();

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
void mock_fstat_return_value(int result = -1, int count = 1);

// Get the number of times `fstat()` was called.
size_t mock_fstat_call_count();

// Mock the `fstat()` system call.
//
// This function is used to mock the `fstat()` system call for testing purposes.
// - Parameters:
//  - fd: The file descriptor to get the status of.
//  - buf: The buffer to store the status in.
// - Returns: 0 on success, -1 on error.
int fstat(int fd, struct stat* buf) noexcept;

// Set the mock return value for `read()` system call
//
// This function is used to mock the return value of the `read()` system call for testing purposes.
// - Parameters:
//  - result: The value to return when `read()` is called.
//  - count: The number of times to return the value.
void mock_read_return_value(int result = -1, int count = 1);

// Get the number of times `read()` was called.
size_t mock_read_call_count();

// Wrapper around the `read()` system call.
// - Parameters:
//  - fd: The file descriptor to read from.
//  - buf: The buffer to read into.
//  - count: The number of bytes to read.
// - Returns: The number of bytes read, or -1 on error.
ssize_t read(int fd, void* buf, size_t count) noexcept;

// Set the mock return value for `write()` system call
//
// This function is used to mock the return value of the `write()` system call for testing purposes.
// - Parameters:
//  - result: The value to return when `write()` is called.
//  - count: The number of times to return the value.
void mock_write_return_value(int result = -1, int count = 1);

// Get the number of times `write()` was called.
size_t mock_write_call_count();

// Wrapper around the `write()` system call.
// - Parameters:
//  - fd: The file descriptor to write to.
//  - buf: The buffer to write from.
//  - count: The number of bytes to write.
// - Returns: The number of bytes written, or -1 on error.
ssize_t write(int fd, const void* buf, size_t count) noexcept;

void mock_lseek_return_value(off_t result = -1, int count = 1);

// Get the number of times `lseek()` was called.
size_t mock_lseek_call_count();

// Wrapper around the `lseek()` system call.
// - Parameters:
//  - fd: The file descriptor to seek in.
//  - offset: The offset to seek to.
//  - whence: The origin of the seek.
// - Returns: The new offset in the file, or -1 on error.
off_t lseek(int fd, off_t offset, int whence) noexcept;

void mock_dup_return_value(int result = -1, int count = 1);

// Get the number of times `dup()` was called.
size_t mock_dup_call_count();

// Wrapper around the `dup()` system call.
// - Parameters:
//  - oldfd: The file descriptor to duplicate.
// - Returns: The new file descriptor, or -1 on error.
int dup(int oldfd) noexcept;

// Set the mock return value for `mmap()` system call
// - Parameters:
//  - value: The value to return when `mmap()` is called. Typically this is `MAP_FAILED`.
//  - count: The number of times to return the value.
void mock_mmap_return_value(void* result = MAP_FAILED, int count = 1);

// Get the number of times `mmap()` was called.
size_t mock_mmap_call_count();

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
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) noexcept;

// Set the mock return value for `munmap()` system call
//
// This function is used to mock the return value of the `munmap()` system call for testing
// purposes.
// - Parameters:
//  - result: The value to return when `munmap()` is called.
//  - count: The number of times to return the value.
void mock_munmap_return_value(int result = -1, int count = 1);

// Get the number of times `munmap()` was called.
size_t mock_munmap_call_count();

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

// Get the number of times `errno_to_status()` was called.
size_t mock_errno_to_status_call_count();

// Mock the `errno_to_status()` function.
//
// This function is used to mock the `errno_to_status()` function for testing purposes.
// - Returns: The `StatusCode` corresponding to the current value of `errno`.
StatusCode errno_to_status() noexcept;

#endif // !defined(PEREGRINE_MOCK_SYSTEM_CALLS)

} // namespace internal
} // namespace peregrine
