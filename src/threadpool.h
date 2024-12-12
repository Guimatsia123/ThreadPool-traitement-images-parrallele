#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* (*func_t)(void*);

struct worker_arg {
  int id;
  struct pool* pool;
};

struct task {
  func_t func;
  void* arg;
};

struct pool {
  int nb_threads;
  pthread_t* threads;
  struct worker_arg* args;
  pthread_barrier_t ready;

  pthread_mutex_t lock;
  sem_t work_busy;
  sem_t work_free;

  struct list* task_list;

  int running;
};

struct pool* threadpool_create(int num, int queue_limit);
void threadpool_add_task(struct pool* pool, func_t fn, void* arg);
void threadpool_join(struct pool* pool);

#ifdef __cplusplus
}
#endif
