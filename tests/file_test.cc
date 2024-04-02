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

TEST_F(FileTest, OpenAlreadyOpen) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(status, peregrine::StatusCode::already_open);
  EXPECT_TRUE(file.is_open());

  status = file.close();
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, OpenWithCloseOnOpen) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto status = file.open(file_name, O_CREAT | O_RDWR | O_CLOEXEC);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  EXPECT_EQ(peregrine::internal::open.get_call_count(), 1);
  EXPECT_EQ(peregrine::internal::close.get_call_count(), 0);
  status = file.open(file_name, O_CREAT | O_RDWR, peregrine::internal::File::default_mode, true);
  EXPECT_EQ(status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());
  EXPECT_EQ(peregrine::internal::open.get_call_count(), 2);
  EXPECT_EQ(peregrine::internal::close.get_call_count(), 1);

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

  // Test write error handler
  {
    peregrine::internal::write.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    constexpr auto data                = "Hello, World!"sv;
    auto [bytes_written, write_status] = file.write(data.data(), data.size());
    EXPECT_EQ(write_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_written, -1);
  }

  // Test read error handler
  {
    peregrine::internal::read.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    std::string buffer(data.size(), '\0');
    auto [bytes_read, read_status] = file.read(buffer.data(), buffer.size());
    EXPECT_EQ(read_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_read, -1);
  }

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, PreadPWrite) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data                = "Hello, World!"sv;
  auto [bytes_written, write_status] = file.pwrite(data.data(), data.size(), 0);
  EXPECT_EQ(write_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_written, data.size());

  constexpr auto data2                 = "LL"sv;
  auto [bytes_written2, write_status2] = file.pwrite(data2.data(), data2.size(), 2);
  EXPECT_EQ(write_status2, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_written2, data2.size());

  // Flush the data to disk
  auto flush_status = file.flush();
  EXPECT_EQ(flush_status, peregrine::StatusCode::ok);

  constexpr auto expected_data = "LLo, Wor"sv;
  std::string buffer(expected_data.size(), '\0');
  auto [bytes_read, read_status] = file.pread(buffer.data(), buffer.size(), 2);
  EXPECT_EQ(read_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_read, expected_data.size());
  EXPECT_EQ(buffer, expected_data.data());

  // Test pwrite error handler
  {
    peregrine::internal::pwrite.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    constexpr auto data                = "Hello, World!"sv;
    auto [bytes_written, write_status] = file.pwrite(data.data(), data.size(), 0);
    EXPECT_EQ(write_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_written, -1);
  }

  // Test pread error handler
  {
    peregrine::internal::pread.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    std::string buffer(2, '\0');
    auto [bytes_read, read_status] = file.pread(buffer.data(), 2, 0);
    EXPECT_EQ(read_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_read, -1);
  }

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, ReadvWritev) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data = "Hello, World!"sv;
  {
    struct iovec wiov[2] = {
        {    .iov_base = const_cast<char*>(data.data()), .iov_len = 6},
        {.iov_base = const_cast<char*>(data.data() + 6), .iov_len = 7}
    };

    auto [bytes_written, write_status] = file.writev(wiov, 2);
    EXPECT_EQ(write_status, peregrine::StatusCode::ok);
    EXPECT_EQ(bytes_written, data.size());
  }

  // Flush the data to disk
  {
    auto flush_status = file.flush();
    EXPECT_EQ(flush_status, peregrine::StatusCode::ok);
  }

  // Test write error handler
  {
    peregrine::internal::writev.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    constexpr auto data  = "Hello, World!"sv;
    struct iovec wiov[2] = {
        {    .iov_base = const_cast<char*>(data.data()), .iov_len = 6},
        {.iov_base = const_cast<char*>(data.data() + 6), .iov_len = 7}
    };

    auto [bytes_written, write_status] = file.writev(wiov, 2);
    EXPECT_EQ(write_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_written, -1);
  }

  // read the data back
  char buffer[13]      = {0};
  struct iovec riov[2] = {
      {    .iov_base = buffer, .iov_len = 6},
      {.iov_base = buffer + 6, .iov_len = 7}
  };
  {
    // Seek to the beginning of the file
    auto [seek_off, seek_status] = file.seek(0, SEEK_SET);
    EXPECT_EQ(seek_status, peregrine::StatusCode::ok);
    EXPECT_EQ(seek_off, 0);

    auto [bytes_read, read_status] = file.readv(riov, 2);
    EXPECT_EQ(read_status, peregrine::StatusCode::ok);
    EXPECT_EQ(bytes_read, data.size());
    EXPECT_EQ(std::string_view(buffer, 13), data);
  }

  // Test readv error handler
  {
    peregrine::internal::readv.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    // Seek to the beginning of the file
    auto [seek_off, seek_status] = file.seek(0, SEEK_SET);
    EXPECT_EQ(seek_status, peregrine::StatusCode::ok);
    EXPECT_EQ(seek_off, 0);

    auto [bytes_read, read_status] = file.readv(riov, 2);
    EXPECT_EQ(read_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_read, -1);
  }

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, PreadvPwritev) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data  = "Hello, World!"sv;
  struct iovec wiov[2] = {
      {    .iov_base = const_cast<char*>(data.data()), .iov_len = 6},
      {.iov_base = const_cast<char*>(data.data() + 6), .iov_len = 7}
  };

  // Write the data to the file
  {
    auto [bytes_written, write_status] = file.pwritev(wiov, 2, 0);
    EXPECT_EQ(write_status, peregrine::StatusCode::ok);
    EXPECT_EQ(bytes_written, data.size());
  }

  // Flush the data to disk
  auto flush_status = file.flush();
  EXPECT_EQ(flush_status, peregrine::StatusCode::ok);

  char buffer[13]      = {0};
  struct iovec riov[2] = {
      {    .iov_base = buffer, .iov_len = 6},
      {.iov_base = buffer + 6, .iov_len = 7}
  };

  // Read the data back
  {
    auto [bytes_read, read_status] = file.preadv(riov, 2, 0);
    EXPECT_EQ(read_status, peregrine::StatusCode::ok);
    EXPECT_EQ(bytes_read, data.size());
    EXPECT_EQ(std::string_view(buffer, 13), data);
  }

  // Test preadv error handler
  {
    peregrine::internal::preadv.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    auto [bytes_read, read_status] = file.preadv(riov, 2, 0);
    EXPECT_EQ(read_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_read, -1);
  }

  // Test pwritev error handler
  {
    peregrine::internal::pwritev.mock_return_value();
    peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

    auto [bytes_written, write_status] = file.pwritev(wiov, 2, 13);
    EXPECT_EQ(write_status, peregrine::StatusCode::eacces);
    EXPECT_EQ(bytes_written, -1);
  }

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, Seek) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data                = "Hello, World!"sv;
  auto [bytes_written, write_status] = file.write(data.data(), data.size());
  EXPECT_EQ(write_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_written, data.size());

  auto [seek_off, seek_status] = file.seek(0, SEEK_SET);
  EXPECT_EQ(seek_status, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off, 0);

  // Read current position and verify
  std::string read_buffer(2, '\0');
  auto [read_bytes, read_status] = file.read(read_buffer.data(), 2);
  EXPECT_EQ(read_status, peregrine::StatusCode::ok);
  EXPECT_EQ(read_bytes, 2);
  EXPECT_EQ(read_buffer, "He"sv);

  auto [seek_off2, seek_status2] = file.seek(2, SEEK_SET);
  EXPECT_EQ(seek_status2, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off2, 2);

  read_buffer.assign(2, '\0');
  auto [read_bytes2, read_status2] = file.read(read_buffer.data(), 2);
  EXPECT_EQ(read_status2, peregrine::StatusCode::ok);
  EXPECT_EQ(read_bytes2, 2);
  EXPECT_EQ(read_buffer, "ll"sv);

  auto [seek_off3, seek_status3] = file.seek(2, SEEK_CUR);
  EXPECT_EQ(seek_status3, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off3, 6);

  // Read current position and verify
  read_buffer.assign(2, '\0');
  auto [read_bytes3, read_status3] = file.read(read_buffer.data(), 2);
  EXPECT_EQ(read_status3, peregrine::StatusCode::ok);
  EXPECT_EQ(read_bytes, 2);
  EXPECT_EQ(read_buffer, " W"sv);

  auto [seek_off4, seek_status4] = file.seek(-2, SEEK_CUR);
  EXPECT_EQ(seek_status4, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off4, 6);

  // Read current position and verify
  read_buffer.assign(2, '\0');
  auto [read_bytes4, read_status4] = file.read(read_buffer.data(), 2);
  EXPECT_EQ(read_status4, peregrine::StatusCode::ok);
  EXPECT_EQ(read_bytes2, 2);
  EXPECT_EQ(read_buffer, " W"sv);

  auto [seek_off5, seek_status5] = file.seek(-2, SEEK_END);
  EXPECT_EQ(seek_status5, peregrine::StatusCode::ok);
  EXPECT_EQ(seek_off5, data.size() - 2);

  // Read current position and verify
  read_buffer.assign(2, '\0');
  auto [read_bytes5, read_status5] = file.read(read_buffer.data(), 2);
  EXPECT_EQ(read_status5, peregrine::StatusCode::ok);
  EXPECT_EQ(read_bytes5, 2);
  EXPECT_EQ(read_buffer, "d!"sv);

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, SeekError) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::lseek.mock_return_value();
  peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

  auto [seek_off, seek_status] = file.seek(0, SEEK_SET);
  EXPECT_EQ(seek_status, peregrine::StatusCode::eacces);
  EXPECT_EQ(seek_off, -1);

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, FlushError) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  constexpr auto data                = "Hello, World!"sv;
  auto [bytes_written, write_status] = file.write(data.data(), data.size());
  EXPECT_EQ(write_status, peregrine::StatusCode::ok);
  EXPECT_EQ(bytes_written, data.size());

  peregrine::internal::fsync.mock_return_value();
  peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

  auto flush_status = file.flush();
  EXPECT_EQ(flush_status, peregrine::StatusCode::eacces);

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}

TEST_F(FileTest, Dup) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  auto [dup_file, dup_status] = file.dup();
  EXPECT_TRUE(dup_file.is_open());
  EXPECT_EQ(dup_status, peregrine::StatusCode::ok);

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());

  EXPECT_TRUE(dup_file.is_open());

  auto close_status2 = dup_file.close();
  EXPECT_EQ(close_status2, peregrine::StatusCode::ok);
  EXPECT_FALSE(dup_file.is_open());
}

TEST_F(FileTest, DupError) {
  peregrine::internal::File file;
  EXPECT_FALSE(file.is_open());

  // Test dup with an unopened file
  {
    auto [dup_file, status] = file.dup();
    EXPECT_FALSE(dup_file.is_open());
    EXPECT_EQ(status, peregrine::StatusCode::ebadf);
  }

  auto open_status = file.open(file_name, O_CREAT | O_RDWR);
  EXPECT_EQ(open_status, peregrine::StatusCode::ok);
  EXPECT_TRUE(file.is_open());

  peregrine::internal::dup.mock_return_value();
  peregrine::internal::errno_to_status.mock_return_value(peregrine::StatusCode::eacces, 1);

  auto [dup_file, dup_status] = file.dup();
  EXPECT_FALSE(dup_file.is_open());
  EXPECT_EQ(dup_status, peregrine::StatusCode::eacces);

  auto close_status = file.close();
  EXPECT_EQ(close_status, peregrine::StatusCode::ok);
  EXPECT_FALSE(file.is_open());
}
