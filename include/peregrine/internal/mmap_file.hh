#pragma once

#include "file.hh"

namespace peregrine {
namespace internal {

class MmapFile {
  void* data{nullptr};
  size_t size{0};

public:
  constexpr static int default_prot  = PROT_READ;
  constexpr static int default_flags = MAP_PRIVATE;

  // Construct a new MmapFile
  //
  // The file is not mapped until `map()` is called.
  MmapFile() noexcept = default;

  MmapFile(const MmapFile&) = delete;

  // Move constructor
  //
  // Move ownership of the mapped file to this object. `other` will be left in the default
  // constructed state.
  // - Parameters:
  //  - other: The MmapFile to move.
  MmapFile(MmapFile&& other) noexcept : data(other.data), size(other.size) {
    other.data = nullptr;
    other.size = 0;
  }

  // Destructor
  ~MmapFile() { unmap(); }

  MmapFile& operator=(const MmapFile&) = delete;

  // Move assignment operator
  //
  // Move ownership of the mapped file to this object. If the file is already mapped, it is
  // unmapped before moving the new file.
  // - Parameters:
  //  - other: The MmapFile to move.
  // - Returns: A reference to the moved MmapFile.
  MmapFile& operator=(MmapFile&& other) noexcept {
    unmap();
    data       = other.data;
    size       = other.size;
    other.data = nullptr;
    other.size = 0;
    return *this;
  }

  // Check if the file is open
  // - Returns: True if the file is mapped, otherwise false.
  bool is_open() const noexcept { return data != nullptr; }

  // Get the size of the mapped file
  // - Returns: The size of the mapped file, or zero if the file is not mapped.
  size_t size() const noexcept { return size; }

  // Map a file into memory
  //
  // This function is the same as `map()` with `offset` and `length`, except `offset` is `0` and
  // `length` is the size of the file as reported by `fstat()`.
  // - Parameters:
  //  - file: The file to map. The file must be open.
  //  - prot: The protection of the mapping. This can be a combination of PROT_READ, PROT_WRITE, or
  //    PROT_EXEC.
  //  - flags: The flags of the mapping. This can be a combination of MAP_SHARED, MAP_PRIVATE,
  //    MAP_FIXED, or MAP_ANONYMOUS.
  // - Returns: An error status if the mmap call fails, otherwise StatusCode::ok.
  // - Note: `file` may be closed after the mapping is created.
  StatusCode map(File& file, int prot = default_prot, int flags = default_flags) noexcept {
    struct stat st;
    if(auto status = file.stat(&st); PEREGRINE_UNLIKELY(status != StatusCode::ok)) return status;
    return map(file, 0, st.st_size, prot, flags);
  }

  // Map a file into memory
  //
  // This function creates a new mapping in the virtual address space of the calling process. If
  // there is already a mapping at the given address, it is replaced. If the mmap call fails, the
  // function returns an error status, and the previous mapping will be unaffected. Errors from
  // unmap are ignored. If you want to check for these errors, explicitly call unmap before calling
  // map().
  // - Parameters:
  //  - file: The file to map. The file must be open.
  //  - offset: The offset in the file where the mapping should start. This must be less than the
  //    file size.
  //  - length: The length of the mapping. This must be greater than zero and less than or equal to
  //    the file size minus the offset.
  //  - prot: The protection of the mapping. This can be a combination of PROT_READ, PROT_WRITE, or
  //    PROT_EXEC.
  //  - flags: The flags of the mapping. This can be a combination of MAP_SHARED, MAP_PRIVATE,
  //    MAP_FIXED, or MAP_ANONYMOUS.
  // - Returns: An error status if the mmap call fails, otherwise StatusCode::ok.
  // - Note: `file` may be closed after the mapping is created.
  StatusCode map(File& file, off_t offset, size_t length, int prot = default_prot,
      int flags = default_flags) noexcept {
    void* const ptr = ::peregrine::internal::mmap(nullptr, length, prot, flags, file.fd, offset);
    if(PEREGRINE_UNLIKELY(ptr == MAP_FAILED)) return errno_to_status();

    unmap();
    data = ptr;
    size = length;
    return StatusCode::ok;
  }

  // Unmap the file from memory
  //
  // This function removes the mapping for the specified address range from the virtual address
  // space of the calling process. If the unmap call fails, the function returns an error status,
  // and the mapping will be unaffected.
  // - Returns: An error status if the munmap call fails, otherwise StatusCode::ok.
  StatusCode unmap() noexcept {
    if(!is_open()) return StatusCode::ok;
    if(auto rc = ::peregrine::internal::munmap(data, size); PEREGRINE_UNLIKELY(rc != 0))
      return errno_to_status();
    data = nullptr;
    size = 0;
    return StatusCode::ok;
  }

}; // class MmapFile

} // namespace internal
} // namespace peregrine
