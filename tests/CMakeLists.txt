# tests/CMakeLists.txt

enable_testing()

# And compiler definitions for testing
add_compile_definitions(-DPEREGRINE_MOCK_SYSTEM_CALLS=1)


add_executable(
  peregrine_test
  peregrine_test.cc
)
target_include_directories(
  peregrine_test
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(
  peregrine_test
  peregrine
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(peregrine_test)

# add target for running test
add_custom_target(
  check
  COMMAND peregrine_test
  DEPENDS peregrine_test
)
