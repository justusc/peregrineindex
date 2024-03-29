#include "peregrine/internal/file.hh"

#include <gtest/gtest.h>

TEST(FileTest, OpenClose) {
  using namespace std::string_view_literals;

  constexpr auto file_name = "./test_file.txt"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  // Use POSIX file system calls to verify the file was created
  struct stat st;
  auto rc = stat(file_name.data(), &st);
  EXPECT_EQ(rc, 0);

  status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());

  // Remove the file
  rc = std::remove(file_name.data());
  EXPECT_EQ(rc, 0);
}
