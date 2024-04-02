# tests/CMakeLists.txt

enable_testing()

# Add a switch to enable coverage compilation flags
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

add_executable(
  peregrine_test
  peregrine_test.cc
  file_test.cc
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

# Add coverage compilation and linking flags if enabled
if(ENABLE_COVERAGE)
  target_compile_options(peregrine_test PRIVATE --coverage)
  target_link_options(peregrine_test PRIVATE --coverage)

  # Check for gcov, lcov, and genhtml to generate coverage reports
  find_program(GCOV gcov REQUIRED DOC "Path to gcov, required for ENABLE_COVERAGE")
  find_program(LCOV lcov REQUIRED DOC "Path to lcov, required for ENABLE_COVERAGE")
  find_program(GENHTML genhtml REQUIRED DOC "Path to genhtml, required for ENABLE_COVERAGE")

endif()

include(GoogleTest)
gtest_discover_tests(peregrine_test)

# Conditionally add coverage commands to the check target based on ENABLE_COVERAGE
if(ENABLE_COVERAGE)
  add_custom_target(check
    COMMAND peregrine_test
    COMMAND ${CMAKE_COMMAND} -E make_directory coverage
    COMMAND ${LCOV} --capture --ignore-errors inconsistent --directory . --output-file coverage/coverage.info
    COMMAND ${LCOV} --ignore-errors inconsistent --ignore-errors unused --remove coverage/coverage.info '/usr/*' '/opt/*' '${CMAKE_BINARY_DIR}/_deps/*' '*/c++/*' --output-file coverage/filtered.info
    COMMAND ${GENHTML} coverage/filtered.info --ignore-errors inconsistent --output-directory coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS peregrine_test
  )
else()
  add_custom_target(check
    COMMAND peregrine_test
    DEPENDS peregrine_test
  )
endif()

set_target_properties(
  peregrine_test check
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)
