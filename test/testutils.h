#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint32_t file_checksum(const std::string& filename);
bool are_files_identical(const std::string& file1_path, const std::string& file2_path);
int generate_file(const std::string& path, size_t size, int key);
int get_active_fd();
int get_active_fd(std::vector<int>& set);
