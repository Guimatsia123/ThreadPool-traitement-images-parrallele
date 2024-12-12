#pragma once
#include <pthread.h>
#include <sys/types.h>

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

// Cette structure est propriétaire des chaines
// Utiliser strdup() si la chaine d'origine ne
// doit pas être libérée. Voir make_work_item()
struct work_item {
  char* input_file;
  char* output_file;
};

struct work_item* make_work_item(char* input_file,  //
                                 char* output_dir);

struct work_item* make_work_item_dup(char* input_file,  //
                                     char* output_dir,  //
                                     int duplicate);

void free_work_item(void* item);
int process_multithread(struct list* items, int nb_thread, int queue_limit);
int process_serial(struct list* items);

#ifdef __cplusplus
}
#endif
