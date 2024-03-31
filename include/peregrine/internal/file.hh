#pragma once

#include "../status_code.hh"
#include "common.hh"
#include "system.hh"

namespace peregrine {
namespace internal {

class MmapFile;

// File is a wrapper class around file system files.
// If provides a thin layer around POSIX file system calls.
class File {
  int fd{-1};

  friend class MmapFile;

public:
  static constexpr int default_flags   = O_RDONLY;
  static constexpr mode_t default_mode = S_IRWXU | S_IRGRP | S_IROTH;

  // Constructors
  File() noexcept   = default;
  File(const File&) = delete;
  File(File&& other) noexcept : fd(other.fd) { other.fd = -1; }

  // Destructor
  // Closes the file if it is open.
  ~File() { close(); }

  // Assignment operators
  File& operator=(const File&) = delete;
  File& operator=(File&& other) noexcept {
    close();
    fd       = other.fd;
    other.fd = -1;
    return *this;
  }

  StatusCode open(std::string_view path, int flags = default_flags, mode_t mode = default_mode) {
    fd = ::peregrine::internal::open(path.data(), flags, mode);
    if(PEREGRINE_UNLIKELY(fd == -1)) return errno_to_status();
    return StatusCode::ok;
  }

  StatusCode close() noexcept {
    auto rc = ::peregrine::internal::close(fd);
    fd      = -1;
    if(PEREGRINE_UNLIKELY(rc != 0)) return errno_to_status();
    return StatusCode::ok;
  }

  bool is_open() const noexcept { return fd != -1; }

  StatusCode stat(struct stat* buf) const noexcept {
    if(auto rc = ::peregrine::internal::fstat(fd, buf); PEREGRINE_UNLIKELY(rc != 0))
      return errno_to_status();
    return StatusCode::ok;
  }

}; // class File

} // namespace internal
} // namespace peregrine
