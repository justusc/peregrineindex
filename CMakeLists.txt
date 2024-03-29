cmake_minimum_required(VERSION 3.15)

project(peregrine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "-Wall")

include(CheckCXXSourceCompiles)

# Check for platform and add vector flags
if(DEFINED DISABLE_SIMD AND DISABLE_SIMD)
  message(STATUS "SIMD disabled")
  add_compile_definitions(-DPEREGRINE_DISABLE_SIMD=1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")

  set(CMAKE_REQUIRED_FLAGS "-mfpu=neon")
  check_cxx_source_compiles("#include <arm_neon.h>\nint main() { uint8x16_t a = vdupq_n_u8(0); return 0; }" NEON_SUPPORTED)

  if(NOT NEON_SUPPORTED)
    message(STATUS "SIMD disabled")
    add_compile_definitions(-DPEREGRINE_DISABLE_SIMD=1)
  else()
    message(STATUS "SIMD enabled (NEON)")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
  endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")

  # Check for SSE2 and SSE4.1
  set(CMAKE_REQUIRED_FLAGS "-msse2")
  check_simd_extension(SSE2 "#include <emmintrin.h>\nint main() { __m128i a = _mm_setzero_si128(); return 0; }")
  set(CMAKE_REQUIRED_FLAGS "-msse4.1")
  check_simd_extension(SSE4_1 "#include <smmintrin.h>\nint main() { __builtin_ia32_pinsrd(); return 0; }")
  if(SSE4_1_SUPPORTED)
    message(STATUS "SIMD enabled (SSE4.1)")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
  elseif(SSE2_SUPPORTED)
    message(STATUS "SIMD enabled (SSE2)")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
  else()
    message(STATUS "SIMD disabled")
    add_compile_definitions(-DPEREGRINE_DISABLE_SIMD=1)
  endif()
  
else()
  message(STATUS "SIMD disabled")
  add_compile_definitions(-DPEREGRINE_DISABLE_SIMD=1)
endif()

# Add source code paths
include_directories(include)
add_subdirectory(src)

# Testing setup
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
