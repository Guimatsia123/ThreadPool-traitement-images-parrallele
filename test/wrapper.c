#define _GNU_SOURCE
#include "wrapper.h"

#include <dlfcn.h>
#include <pthread.h>

static _Atomic int fread_cnt;
static _Atomic int fwrite_cnt;
static _Atomic int read_cnt;
static _Atomic int write_cnt;
static _Atomic int mmap_cnt;
static _Atomic int munmap_cnt;
static _Atomic int fread_size_max;
static _Atomic int read_size_max;
static _Atomic int write_size_max;
static _Atomic int pipe_cnt;
static _Atomic int sem_init_cnt;
static _Atomic int sem_post_cnt;
static _Atomic int sem_wait_cnt;
static _Atomic int sem_destroy_cnt;

static size_t (*fread_func)(void* ptr, size_t size, size_t nmemb, FILE* stream);
static size_t (*fwrite_func)(const void* ptr, size_t size, size_t nmemb, FILE* stream);
static ssize_t (*read_func)(int fd, void* ptr, size_t size);
static ssize_t (*write_func)(int fd, const void* ptr, size_t size);
static ssize_t (*mmap_func)(void* addr, size_t len, int prot, int flags, int fd, off_t off);
static ssize_t (*munmap_func)(void* ptr, size_t size);
static int (*pipe_func)(int fd[2]);
static int (*sem_init_func)(void* sem, int pshared, unsigned int value);
static int (*sem_post_func)(void* sem);
static int (*sem_wait_func)(void* sem);
static int (*sem_destroy_func)(void* sem);

static pthread_mutex_t lock;

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  pthread_mutex_lock(&lock);
  fread_cnt++;
  if (size * nmemb > fread_size_max) {
    fread_size_max = size * nmemb;
  }
  if (!fread_func) {
    fread_func = dlsym(RTLD_NEXT, "fread");
  }
  pthread_mutex_unlock(&lock);
  return fread_func(ptr, size, nmemb, stream);
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
  pthread_mutex_lock(&lock);
  fwrite_cnt++;
  if (!fwrite_func) {
    fwrite_func = dlsym(RTLD_NEXT, "fwrite");
  }
  pthread_mutex_unlock(&lock);
  return fwrite_func(ptr, size, nmemb, stream);
}

ssize_t read(int fd, void* ptr, size_t size) {
  pthread_mutex_lock(&lock);
  read_cnt++;
  if (size > read_size_max) {
    read_size_max = size;
  }
  if (!read_func) {
    read_func = dlsym(RTLD_NEXT, "read");
  }
  pthread_mutex_unlock(&lock);
  return read_func(fd, ptr, size);
}

ssize_t write(int fd, const void* ptr, size_t size) {
  pthread_mutex_lock(&lock);
  write_cnt++;
  if (size > write_size_max) {
    write_size_max = size;
  }
  if (!write_func) {
    write_func = dlsym(RTLD_NEXT, "write");
  }
  pthread_mutex_unlock(&lock);
  return write_func(fd, ptr, size);
}

ssize_t mmap(void* addr, size_t len, int prot, int flags, int fd, off_t off) {
  pthread_mutex_lock(&lock);
  mmap_cnt++;
  if (!mmap_func) {
    mmap_func = dlsym(RTLD_NEXT, "mmap");
  }
  pthread_mutex_unlock(&lock);
  return mmap_func(addr, len, prot, flags, fd, off);
}

ssize_t munmap(void* ptr, size_t size) {
  pthread_mutex_lock(&lock);
  munmap_cnt++;
  if (!munmap_func) {
    munmap_func = dlsym(RTLD_NEXT, "munmap");
  }
  pthread_mutex_unlock(&lock);
  return munmap_func(ptr, size);
}

int pipe(int fd[2]) {
  pthread_mutex_lock(&lock);
  pipe_cnt++;
  if (!pipe_func) {
    pipe_func = dlsym(RTLD_NEXT, "pipe");
  }
  pthread_mutex_unlock(&lock);
  return pipe_func(fd);
}

int sem_init(sem_t* sem, int pshared, unsigned int value) {
  pthread_mutex_lock(&lock);
  sem_init_cnt++;
  if (!sem_init_func) {
    sem_init_func = dlsym(RTLD_NEXT, "sem_init");
  }
  pthread_mutex_unlock(&lock);
  return sem_init_func(sem, pshared, value);
}

int sem_post(sem_t* sem) {
  pthread_mutex_lock(&lock);
  sem_post_cnt++;
  if (!sem_post_func) {
    sem_post_func = dlsym(RTLD_NEXT, "sem_post");
  }
  pthread_mutex_unlock(&lock);
  return sem_post_func(sem);
}

int sem_wait(sem_t* sem) {
  pthread_mutex_lock(&lock);
  sem_wait_cnt++;
  if (!sem_wait_func) {
    sem_wait_func = dlsym(RTLD_NEXT, "sem_wait");
  }
  pthread_mutex_unlock(&lock);
  return sem_wait_func(sem);
}

int sem_destroy(sem_t* sem) {
  pthread_mutex_lock(&lock);
  sem_destroy_cnt++;
  if (!sem_destroy_func) {
    sem_destroy_func = dlsym(RTLD_NEXT, "sem_destroy");
  }
  pthread_mutex_unlock(&lock);
  return sem_destroy_func(sem);
}

void wrapper_clear() {
  pthread_mutex_lock(&lock);
  fread_cnt = 0;
  fwrite_cnt = 0;
  read_cnt = 0;
  write_cnt = 0;
  mmap_cnt = 0;
  munmap_cnt = 0;
  fread_size_max = 0;
  read_size_max = 0;
  write_size_max = 0;
  pipe_cnt = 0;
  sem_init_cnt = 0;
  sem_post_cnt = 0;
  sem_wait_cnt = 0;
  sem_destroy_cnt = 0;
  pthread_mutex_unlock(&lock);
}

int wrapper_fread_count() {
  return fread_cnt;
}
int wrapper_fwrite_count() {
  return fwrite_cnt;
}
int wrapper_read_count() {
  return read_cnt;
}
int wrapper_write_count() {
  return write_cnt;
}
int wrapper_mmap_count() {
  return mmap_cnt;
}
int wrapper_munmap_count() {
  return munmap_cnt;
}
int wrapper_fread_size_max() {
  return fread_size_max;
}
int wrapper_read_size_max() {
  return read_size_max;
}
int wrapper_write_size_max() {
  return write_size_max;
}
int wrapper_pipe_count() {
  return pipe_cnt;
}
int wrapper_sem_init_count() {
  return sem_init_cnt;
}
int wrapper_sem_post_count() {
  return sem_post_cnt;
}
int wrapper_sem_wait_count() {
  return sem_wait_cnt;
}
int wrapper_sem_destroy_count() {
  return sem_destroy_cnt;
}
