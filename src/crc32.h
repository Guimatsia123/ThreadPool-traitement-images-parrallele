#pragma once

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32(uint32_t hash, void* buf, size_t len);

#ifdef __cplusplus
}
#endif
