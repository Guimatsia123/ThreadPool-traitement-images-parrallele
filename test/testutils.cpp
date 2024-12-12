#include "testutils.h"

#include <fcntl.h>
#include <sys/resource.h>

#include <crc32.hpp>
#include <fstream>

uint32_t file_checksum(const std::string& filename) {
  std::ifstream file(filename, std::ifstream::binary);
  if (!file.is_open()) {
    return 0;
  }
  std::istreambuf_iterator<char> head(file), tail;

  uint32_t checksum = takuya_crc32<IEEE8023_CRC32_POLYNOMIAL>(0xFFFFFFFF, head, tail);
  return checksum;
}

bool are_files_identical(const std::string& file1_path, const std::string& file2_path) {
  std::ifstream file1(file1_path, std::ifstream::binary);
  std::ifstream file2(file2_path, std::ifstream::binary);

  if (!file1.is_open() || !file2.is_open()) {
    return false;
  }

  std::istreambuf_iterator<char> file1_iter(file1), file1_end;
  std::istreambuf_iterator<char> file2_iter(file2), file2_end;

  return std::equal(file1_iter, file1_end, file2_iter, file2_end);
}

int generate_file(const std::string& path, size_t size, int key) {
  std::ofstream out(path, std::ofstream::binary | std::ofstream::trunc);
  if (!out.is_open()) {
    return -1;
  }
  for (size_t i = 0; i < size; i++) {
    out << (char)(i + key);
  }
  return 0;
}

int get_active_fd() {
  std::vector<int> tmp;
  return get_active_fd(tmp);
}

int get_active_fd(std::vector<int>& fdset) {
  struct rlimit rlim;

  if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
    perror("getrlimit");
    return -1;
  }

  int lim = rlim.rlim_cur;
  if (lim > 1024)
    lim = 1024;
  for (int fd = 0; fd < lim; ++fd) {
    if (fcntl(fd, F_GETFD) != -1) {
      fdset.push_back(fd);
    }
  }

  return fdset.size();
}
