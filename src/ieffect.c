#define _GNU_SOURCE
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>

#include "processing.h"
#include "utils.h"

struct app {
  char* input;
  char* output;
  int multithread;
  int queue_limit;
  struct list* work_list;
  int nb_threads;
};

void print_usage() {
  fprintf(stderr, "Usage: %s [-iomnlh]\n", "ieffect");
}

int main(int argc, char** argv) {
  int ret = 1;
  printf("ieffect\n");

  struct option options[] = {
      {"input", 1, 0, 'i'},        //
      {"output", 1, 0, 'o'},       //
      {"multithread", 0, 0, 'm'},  //
      {"nb-thread", 1, 0, 'n'},    //
      {"queue-limit", 1, 0, 'l'},  //
      {"help", 0, 0, 'h'},         //
      {0, 0, 0, 0}                 //
  };

  struct app app = {
      .input = NULL,               //
      .output = NULL,              //
      .multithread = 0,            //
      .queue_limit = 10,           //
      .nb_threads = get_nprocs(),  //
  };

  int opt;
  int idx;
  while ((opt = getopt_long(argc, argv, "i:o:n:l:mh", options, &idx)) != -1) {
    switch (opt) {
      case 'i':
        app.input = optarg;
        break;
      case 'o':
        app.output = optarg;
        break;
      case 'l': {
        app.queue_limit = atoi(optarg);
      } break;
      case 'm':
        app.multithread = 1;
        break;
      case 'n':
        app.nb_threads = atoi(optarg);
        break;
      case 'h':
        print_usage();
        return 0;
      default:
        printf("Erreur: option inconnue\n");
        print_usage();
        return 1;
    }
  }

  // Affichage des options
  {
    printf("options\n");
    printf(" input           : %s\n", app.input);
    printf(" output          : %s\n", app.output);
    printf(" multithread     : %d\n", app.multithread);
    printf(" queue_limit     : %d\n", app.queue_limit);
    printf(" nb_threads      : %d\n", app.nb_threads);
  }

  app.work_list = list_new(NULL, free_work_item);

  if (app.nb_threads < 1 || app.nb_threads > 128) {
    printf("Erreur: le nombre de fils d'exécution doit être compris entre 1 et 128\n");
    goto out_list;
  }

  if (!app.input || !app.output) {
    print_usage();
    goto out_list;
  }

  // Si la sortie est un fichier regulier, c'est invalide
  if (is_regular_file(app.output)) {
    print_usage();
    goto out_list;
  }

  // Creer le repertoire de sortie si necessaire
  if (!is_directory(app.output)) {
    if (mkdir(app.output, 0755) < 0) {
      perror("mkdir");
      goto out_list;
    }
  }

  // Lister les fichiers si input est un repertoire
  if (is_regular_file(app.input)) {
    if (!ends_with(app.input, ".png")) {
      printf("Seulement les fichiers PNG sont supportés\n");
      goto out_list;
    }

    struct work_item* item = make_work_item(app.input, app.output);
    list_push_back(app.work_list, list_node_new(item));
  } else if (is_directory(app.input)) {
    // Lister le repertoire
    struct dirent* entry;
    DIR* dir = opendir(app.input);
    if (!dir) {
      perror("opendir");
      goto out_list;
    }

    while ((entry = readdir(dir)) != NULL) {
      if (ends_with(entry->d_name, ".png")) {
        char* tmp_input;
        char* tmp_output;
        asprintf(&tmp_input, "%s/%s", app.input, entry->d_name);
        asprintf(&tmp_output, "%s/%s", app.output, entry->d_name);
        struct work_item* item = make_work_item_dup(tmp_input, tmp_output, 0);
        struct list_node* node = list_node_new(item);
        list_push_back(app.work_list, node);
      }
    }
    closedir(dir);
  } else {
    printf("Type de fichier inconnu");
    goto out_list;
  }

  printf("Nombre d'images à traiter: %lu\n", list_size(app.work_list));

  if (app.multithread) {
    ret = process_multithread(app.work_list, app.nb_threads, app.queue_limit);
  } else {
    ret = process_serial(app.work_list);
  }

out_list:
  list_free(app.work_list);

  printf("Fin du programme\n");
  return ret;
}
