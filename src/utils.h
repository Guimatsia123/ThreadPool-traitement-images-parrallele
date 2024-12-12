#pragma once

#include <pthread.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int is_regular_file(const char* path);
int is_directory(const char* path);
int ends_with(const char* str, const char* suffix);

#ifdef __cplusplus
}
#endif
