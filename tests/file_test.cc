#include "peregrine/internal/file.hh"

#include <gtest/gtest.h>

using namespace std::string_view_literals;
static constexpr auto file_name = "./test_file.txt"sv;

// Test case setup and teardown
class FileTest : public ::testing::Test {
public:

protected:
  void SetUp() override {
    peregrine::internal::reset_mocks();
    unlink(file_name.data());
  }

  void TearDown() override {
    peregrine::internal::reset_mocks();
    unlink(file_name.data());
  }
}; // class FileTest

TEST_F(FileTest, OpenClose) {
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
}

TEST_F(FileTest, OpenError) {
  using namespace std::string_view_literals;

  constexpr auto file_name = "/this/file/does/not/exist"sv;

  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::enoent);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, CloseWhenNotOpen) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, Stat) {
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
}

TEST_F(FileTest, StatError) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::fstat.mock_return_value();
  peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

  struct stat st;
  status = file.stat(&st);
  EXPECT_EQ(status, peregrine::StatusCode::eacces);

  status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, Move) {
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
}

TEST_F(FileTest, MoveCloseError) {
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

  peregrine::internal::close.mock_return_value();
  peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::ebadf, 1);

  other = std::move(file);
  EXPECT_FALSE(file.is_open());
  EXPECT_TRUE(other.is_open());
}

TEST_F(FileTest, ReadWrite) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data                = "Hello, World!"sv;
  auto [bytes_written, write_status] = file.write(data.data(), data.size());
  EXPECT_EQ(write_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_written, data.size());

  // Flush the data to disk
  auto flush_status = file.flush();
  EXPECT_EQ(flush_status, peregrine::StatusCode::ok);

  // Seek to the beginning of the file
  auto [seek_off, seek_status] = file.seek(0, SEEK_SET);
  EXPECT_EQ(seek_status, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off, 0);

  std::string buffer(data.size(), '\0');
  auto [bytes_read, read_status] = file.read(buffer.data(), buffer.size());
  EXPECT_EQ(read_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_read, data.size());
  EXPECT_EQ(buffer, data.data());

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}
