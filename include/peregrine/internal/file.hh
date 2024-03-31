#pragma once

#include "../status_code.hh"
#include "common.hh"
#include "log.hh"
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

  // Default constructor
  //
  // Creates a new File object that is not associated with any file. Call `open()` to open a file.
  File() noexcept = default;

  // Copy constructor (deleted)
  File(const File&) = delete;

  // Move constructor
  //
  // Move ownership of the file to this object. `other` will be left in the default constructed
  // state.
  // Parameters:
  //  - other: The File to move.
  File(File&& other) noexcept : fd(other.fd) { other.fd = -1; }

  // Destructor
  //
  // Closes the file, if it is open.
  ~File() { close(); }

  // Assignment operators
  File& operator=(const File&) = delete;

  // Move assignment operator
  //
  // Move ownership of the file to this object. If the file is already open, it is closed before
  // moving the new file.
  // Parameters:
  //  - other: The File to move.
  // Returns: A reference to the this File.
  File& operator=(File&& other) noexcept {
    close();
    fd       = other.fd;
    other.fd = -1;
    return *this;
  }

  // Open a file
  //
  // Parameters:
  //  - path: The path to the file to open.
  //  - flags: The flags to use when opening the file.
  //  - mode: The mode to use when opening the file.
  // Returns: `StatusCode::ok` on success, otherwise an error code.
  StatusCode open(std::string_view path, int flags = default_flags, mode_t mode = default_mode) {
    PEREGRINE_LOG_TRACE("Opening file: path={}, flags={}, mode={}", path, flags, mode);

    StatusCode status = StatusCode::ok;

    fd = ::peregrine::internal::open(path.data(), flags, mode);
    if(PEREGRINE_UNLIKELY(fd == -1)) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR(
          "Failed to open file: path={}, flags={}, mode={} : {}", path, flags, mode, status);
    }
    return status;
  }

  // Close the file
  //
  // Returns: `StatusCode::ok` on success, otherwise an error code.
  StatusCode close() noexcept {
    StatusCode status = StatusCode::ok;

    // Only close the file if it is open
    if(fd == -1) return status;

    // Close the file
    auto rc = ::peregrine::internal::close(fd);
    fd      = -1;
    if(PEREGRINE_UNLIKELY(rc != 0)) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("Failed to close file : {}", status);
    }
    return status;
  }

  // Returns true if the file is open.
  bool is_open() const noexcept { return fd != -1; }

  // Get the status of the file
  //
  // Parameters:
  //  - buf: The buffer to store the status in.
  // Returns: `StatusCode::ok` on success, otherwise an error code.
  StatusCode stat(struct stat* buf) const noexcept {
    StatusCode status = StatusCode::ok;

    if(auto rc = ::peregrine::internal::fstat(fd, buf); PEREGRINE_UNLIKELY(rc != 0)) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("Failed to get file status : {}", status);
    }
    return status;
  }

  // Read from the file
  //
  // Reads up to `count` bytes from the file into `buf`. The number of bytes read may be less than
  // `count` if the end of the file is reached.
  // Parameters:
  //  - buf: The buffer to read into.
  //  - count: The number of bytes to read.
  // Returns: A tuple containing the number of bytes read (zero indicates end of file) and a status
  // code. The status code is `StatusCode::ok` on success, otherwise an error code.
  std::tuple<ssize_t, StatusCode> read(void* buf, size_t count) const noexcept {
    auto rc           = ::peregrine::internal::read(fd, buf, count);
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(rc == -1)) status = errno_to_status();
    return {rc, status};
  }

  // Write to the file
  //
  // Writes up to `count` bytes from `buf` to the file. The number of bytes written may be less than
  // `count` if the file system is full.
  // Parameters:
  //  - buf: The buffer to write from.
  //  - count: The number of bytes to write.
  // Returns: A tuple containing the number of bytes written and a status code. The status code is
  // `StatusCode::ok` on success, otherwise an error code.
  std::tuple<ssize_t, StatusCode> write(const void* buf, size_t count) const noexcept {
    auto rc           = ::peregrine::internal::write(fd, buf, count);
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(rc == -1)) status = errno_to_status();
    return {rc, status};
  }

  // Seek to a position in the file
  //
  // Parameters:
  //  - offset: The offset to seek to.
  //  - whence: The origin of the seek.
  // Returns: A tuple containing the new offset (or `-1` on error) and a status code. The status
  // code is `StatusCode::ok` on success, otherwise an error code.
  std::tuple<off_t, StatusCode> seek(off_t offset, int whence) const noexcept {
    auto rc           = ::peregrine::internal::lseek(fd, offset, whence);
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(rc == -1)) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("Failed to seek in file : {}", status);
    }
    return {rc, status};
  }

  // Duplicate the file
  //
  // Duplicates the file descriptor. The new file descriptor will refer to the same file as the
  // this file.
  // Returns: A tuple containing the new file and a status code. The status code is `StatusCode::ok`
  // on success, otherwise an error code.
  std::tuple<File, StatusCode> dup() const noexcept {
    File file;
    file.fd           = ::peregrine::internal::dup(fd);
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(file.fd == -1)) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("Failed to duplicate file : {}", status);
    }
    return {std::move(file), status};
  }

}; // class File

} // namespace internal
} // namespace peregrine
