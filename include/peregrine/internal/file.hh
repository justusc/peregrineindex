#pragma once

#include "../status_code.hh"
#include "common.hh"
#include "log.hh"
#include "system.hh"

namespace peregrine {
namespace internal {

class MmapFile;

/**
 * @class File
 * @brief Represents a file and provides operations for file handling.
 *
 * The File class encapsulates a file descriptor and provides methods for opening, closing, reading,
 * writing, seeking, and duplicating files. It also provides methods for retrieving the status of
 * the file.
 */
class File {
  int fd{-1};

  friend class MmapFile;

  template <typename T>
  static StatusCode handler(T rc, std::string_view fn) noexcept {
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(rc == T(-1))) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("File {} failed : {}"sv, fn, status);
    }
    return status;
  }

  template <typename T>
  static std::tuple<T, StatusCode> handler2(T rc, std::string_view fn) noexcept {
    StatusCode status = StatusCode::ok;
    if(PEREGRINE_UNLIKELY(rc == T(-1))) {
      status = errno_to_status();
      PEREGRINE_LOG_ERROR("File {} failed : {}"sv, fn, status);
    }
    return {rc, status};
  }

public:
  /**
   * @brief The default file flags for opening a file.
   *
   * This variable represents the default flags used when opening a file. It is set to `O_RDONLY`,
   * which stands for "read-only". This means that by default, files opened using these flags can
   * only be read.
   */
  static constexpr int default_flags = O_RDONLY;

  /**
   * @brief The `mode_t` type represents the file mode and permission bits.
   *
   * The `mode_t` type is used to specify the file mode and permission bits for file operations.
   * It is an integer type that typically represents a combination of flags defined in the
   * `sys/stat.h` header file.
   *
   * In this code snippet, the `default_mode` constant is defined as a `mode_t` value.
   * It represents the default file mode and permission bits for a file.
   * The `S_IRWXU`, `S_IRGRP`, and `S_IROTH` flags are combined using the bitwise OR operator (`|`)
   * to set the desired permissions.
   */
  static constexpr mode_t default_mode = S_IRWXU | S_IRGRP | S_IROTH;

  /**
   * @brief Default constructor.
   *
   * Creates a new File object that is not associated with any file. Call `open()` to open a file.
   */
  File() noexcept = default;

  // Copy constructor (deleted)
  File(const File&) = delete;

  /**
   * @brief Move constructor.
   *
   * Move ownership of the file to this object. `other` will be left in the default constructed
   * state.
   *
   * @param other The File to move.
   */
  File(File&& other) noexcept : fd(other.fd) { other.fd = -1; }

  /**
   * @brief Destructor.
   *
   * Closes the file, if it is open.
   */
  ~File() { close(); }

  // Assignment operators
  File& operator=(const File&) = delete;

  /**
   * @brief Move assignment operator.
   *
   * Move ownership of the file to this object. If the file is already open, it is closed before
   * moving the new file.
   *
   * @param other The File to move.
   * @return A reference to the this File.
   */
  File& operator=(File&& other) noexcept {
    close();
    fd       = other.fd;
    other.fd = -1;
    return *this;
  }

  /**
   * @brief Open a file.
   *
   * @param path The path to the file to open.
   * @param flags The flags to use when opening the file.
   * @param mode The mode to use when opening the file.
   * @return `StatusCode::ok` on success, otherwise an error code.
   */
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

  /**
   * @brief Close the file.
   *
   * @return `StatusCode::ok` on success, otherwise an error code.
   */
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

  /**
   * @brief Check if the file is open.
   *
   * @return True if the file is open, false otherwise.
   */
  bool is_open() const noexcept { return fd != -1; }

  /**
   * @brief Get the status of the file.
   *
   * @param buf The buffer to store the status in.
   * @return `StatusCode::ok` on success, otherwise an error code.
   */
  StatusCode stat(struct stat* buf) const noexcept {
    return handler(::peregrine::internal::fstat(fd, buf), "fstat"sv);
  }

  /**
   * @brief Reads data from the file into the provided buffer.
   *
   * Reads up to `count` bytes from the file into `buf`. The number of bytes read may be less than
   * `count` if the end of the file is reached.
   *
   * @param buf Pointer to the buffer where the data will be stored.
   * @param count Number of bytes to read.
   * @return A tuple containing the number of bytes read and the status code.
   */
  std::tuple<ssize_t, StatusCode> read(void* buf, size_t count) const noexcept {
    return handler2(::peregrine::internal::read(fd, buf, count), "read"sv);
  }

  /**
   * @brief Reads data from a specific position in the file into the provided buffer.
   *
   * Reads up to `count` bytes from the file into `buf` starting at the specified `offset`.
   * The number of bytes read may be less than `count` if the end of the file is reached.
   *
   * @param buf Pointer to the buffer where the data will be stored.
   * @param count Number of bytes to read.
   * @param offset The offset in the file to start reading from.
   * @return A tuple containing the number of bytes read and the status code.
   */
  std::tuple<ssize_t, StatusCode> pread(void* buf, size_t count, off_t offset) const noexcept {
    return handler2(::peregrine::internal::pread(fd, buf, count, offset), "pread"sv);
  }

  /**
   * @brief Reads data from the file into multiple buffers.
   *
   * Reads data from the file into multiple buffers specified by `iov` starting at the specified
   * `offset`. The total number of bytes read may be less than the sum of the sizes of the buffers
   * if the end of the file is reached.
   *
   * @param iov An array of iovec structures specifying the buffers and their sizes.
   * @param iovcnt The number of iovec structures in the array.
   * @return A tuple containing the number of bytes read and the status code.
   */
  std::tuple<ssize_t, StatusCode> readv(const struct iovec* iov, int iovcnt) const noexcept {
    return handler2(::peregrine::internal::readv(fd, iov, iovcnt), "readv"sv);
  }

  /**
   * @brief Reads data from multiple positions in the file into multiple buffers.
   *
   * Reads data from the file into multiple buffers specified by `iov` starting at the specified
   * `offsets`. The total number of bytes read may be less than the sum of the sizes of the buffers
   * if the end of the file is reached.
   *
   * @param iov An array of iovec structures specifying the buffers and their sizes.
   * @param iovcnt The number of iovec structures in the array.
   * @param offset The offset in the file to start reading from.
   * @return A tuple containing the number of bytes read and the status code.
   */
  std::tuple<ssize_t, StatusCode> preadv(
      const struct iovec* iov, int iovcnt, off_t offset) const noexcept {
    return handler2(::peregrine::internal::preadv(fd, iov, iovcnt, offset), "preadv"sv);
  }

  /**
   * @brief Writes data from the provided buffer to the file.
   *
   * Writes up to `count` bytes from `buf` to the file.
   *
   * @param buf Pointer to the buffer containing the data to write.
   * @param count Number of bytes to write.
   * @return A tuple containing the number of bytes written and the status code.
   */
  std::tuple<ssize_t, StatusCode> write(const void* buf, size_t count) const noexcept {
    return handler2(::peregrine::internal::write(fd, buf, count), "write"sv);
  }

  /**
   * @brief Writes data from the provided buffer to a specific position in the file.
   *
   * Writes up to `count` bytes from `buf` to the file starting at the specified `offset`.
   *
   * @param buf Pointer to the buffer containing the data to write.
   * @param count Number of bytes to write.
   * @param offset The offset in the file to start writing from.
   * @return A tuple containing the number of bytes written and the status code.
   */
  std::tuple<ssize_t, StatusCode> pwrite(
      const void* buf, size_t count, off_t offset) const noexcept {
    return handler2(::peregrine::internal::pwrite(fd, buf, count, offset), "pwrite"sv);
  }

  /**
   * @brief Writes data from multiple buffers to the file.
   *
   * Writes data from multiple buffers specified by `iov` to the file starting at the specified
   * `offset`.
   *
   * @param iov An array of iovec structures specifying the buffers and their sizes.
   * @param iovcnt The number of iovec structures in the array.
   * @return A tuple containing the number of bytes written and the status code.
   */
  std::tuple<ssize_t, StatusCode> writev(const struct iovec* iov, int iovcnt) const noexcept {
    return handler2(::peregrine::internal::writev(fd, iov, iovcnt), "writev"sv);
  }

  /**
   * @brief Writes data from multiple buffers to multiple positions in the file.
   *
   * Writes data from multiple buffers specified by `iov` to the file starting at the specified
   * `offsets`.
   *
   * @param iov An array of iovec structures specifying the buffers and their sizes.
   * @param iovcnt The number of iovec structures in the array.
   * @param offset The offset in the file to start writing from.
   * @return A tuple containing the number of bytes written and the status code.
   */
  std::tuple<ssize_t, StatusCode> pwritev(
      const struct iovec* iov, int iovcnt, off_t offset) const noexcept {
    return handler2(::peregrine::internal::pwritev(fd, iov, iovcnt, offset), "pwritev"sv);
  }

  /**
   * @brief Changes the file offset.
   *
   * Sets the file offset to the specified `offset`.
   *
   * @param offset The new file offset.
   * @param whence The starting position for the offset calculation.
   * @return The new file offset on success, otherwise -1.
   */
  std::tuple<off_t, StatusCode> seek(off_t offset, int whence) noexcept {
    return handler2(::peregrine::internal::lseek(fd, offset, whence), "lseek"sv);
  }

  /** @brief Duplicate the file
   *
   * Duplicates the file descriptor. The new file descriptor will refer to the same file as the
   * this file.
   *
   * @return A tuple containing the new file and a status code. The status code is `StatusCode::ok`
   * on success, otherwise an error code.
   */
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

  /**
   * @brief Flush written data to the disk.
   *
   * @return `StatusCode::ok` on success, otherwise an error code.
   */
  StatusCode flush() const noexcept { return handler(::peregrine::internal::fsync(fd), "fsync"sv); }

}; // class File

} // namespace internal
} // namespace peregrine
