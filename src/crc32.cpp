#include "crc32.hpp"

#include "crc32.h"

static int crc32_call_count = 0;
static int crc32_max_len = 0;

uint32_t crc32(uint32_t hash, void* buf, size_t len) {
  crc32_call_count++;
  if (len > crc32_max_len) {
    crc32_max_len = len;
  }
  return takuya_crc32<IEEE8023_CRC32_POLYNOMIAL>(hash, (uint8_t*)buf, (uint8_t*)buf + len);
}
