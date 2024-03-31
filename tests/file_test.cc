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

TEST(FileTest, OpenError) {
  using namespace std::string_view_literals;

  constexpr auto file_name = "/this/file/does/not/exist"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::enoent);
  EXPECT_FALSE(file.is_open());
}

TEST(FileTest, CloseWhenNotOpen) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST(FileTest, Stat) {
  using namespace std::string_view_literals;

  constexpr auto file_name = "./test_file.txt"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  // Test when the file is not open
  struct stat st;
  auto status = file.stat(&st);
  EXPECT_EQ(status, peregrine::StatusCode::ebadf);

  status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  status = file.stat(&st);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_EQ(st.st_size, 0);
  EXPECT_EQ(st.st_mode & S_IFREG, S_IFREG);
  EXPECT_EQ(st.st_mode & S_IRUSR, S_IRUSR);
  EXPECT_EQ(st.st_mode & S_IWUSR, S_IWUSR);

  status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());

  // Remove the file
  auto rc = std::remove(file_name.data());
  EXPECT_EQ(rc, 0);
}

TEST(FileTest, StatError) {

  using namespace std::string_view_literals;

  constexpr auto file_name = "./test_file.txt"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::mock_fstat_return_value();
  peregrine::internal::mock_errno_to_status_return_value(peregrine::StatusCode::eacces, 1);

  struct stat st;
  status = file.stat(&st);
  EXPECT_EQ(status, peregrine::StatusCode::eacces);

  status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());

  // Remove the file
  auto rc = std::remove(file_name.data());
  EXPECT_EQ(rc, 0);
}

TEST(FileTest, Move) {
  using namespace std::string_view_literals;

  constexpr auto file_name = "./test_file.txt"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::File other;
  EXPECT_FALSE(other.is_open());

  other = std::move(file);
  EXPECT_FALSE(file.is_open());
  EXPECT_TRUE(other.is_open());

  status = other.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(other.is_open());

  // Remove the file
  auto rc = std::remove(file_name.data());
  EXPECT_EQ(rc, 0);
}

TEST(FileTest, MoveCloseError) {
  using namespace std::string_view_literals;

  constexpr auto file_name       = "./test_file.txt"sv;
  constexpr auto other_file_name = "./test_other_file.txt"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::File other;
  EXPECT_FALSE(other.is_open());
  other.open(other_file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);

  peregrine::internal::mock_close_return_value();
  peregrine::internal::mock_errno_to_status_return_value(peregrine::StatusCode::ebadf, 1);

  other = std::move(file);
  EXPECT_FALSE(file.is_open());
  EXPECT_TRUE(other.is_open());

  // Remove the file
  auto rc = std::remove(file_name.data());
  EXPECT_EQ(rc, 0);
}
