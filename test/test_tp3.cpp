#include <crc32.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <threadpool.h>
#include <unistd.h>
#include <utils.h>

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>

#include "inf3173.h"
#include "processing.h"
#include "testutils.h"
#include "threadpool.h"
#include "wrapper.h"

static const timespec jiffy = {.tv_sec = 1, .tv_nsec = 0};
static const char* img = SOURCE_DIR "/test/cat.png";
static const char* img_serial = BINARY_DIR "/test/cat-serial.png";
static const char* img_multithread = BINARY_DIR "/test/cat-multithread.png";

struct tidarg {
  pthread_mutex_t lock;
  pthread_barrier_t barrier;
  std::unordered_map<int, int> tidmap;
  int count;
};

void* worker_test(void* arg) {
  struct tidarg* info = static_cast<struct tidarg*>(arg);
  int tid = gettid();
  pthread_mutex_lock(&info->lock);
  info->tidmap[tid]++;
  info->count++;
  pthread_mutex_unlock(&info->lock);
  pthread_barrier_wait(&info->barrier);
  return NULL;
}

void* worker_queue_limit(void* arg) {
  sem_t* sem = static_cast<sem_t*>(arg);
  sleep(1);
  sem_post(sem);
  return NULL;
}

TEST_CASE("FonctionnementNormal") {
  /*
   * Ce test vérifie que les fils d'exécutions sont réutilisés en se basant sur
   * le TID.
   */
  int n_threads = 4;
  int n_cycles = 10;

  struct tidarg info;
  pthread_mutex_init(&info.lock, NULL);
  pthread_barrier_init(&info.barrier, NULL, n_threads + 1);
  info.count = 0;

  // On utilise une limite très haute pour que toutes les tâches
  // s'exécutent en même temps, requis pour que la barrière débloque
  struct pool* p = threadpool_create(n_threads, 1000);
  REQUIRE(p != nullptr);

  for (int i = 0; i < n_cycles; i++) {
    // Ajouter un item de travail par fil d'exécution
    for (int j = 0; j < n_threads; j++) {
      threadpool_add_task(p, worker_test, &info);
    }

    // Attendre le traitement de ce groupe de tâches On utilise une barrière
    // pour confirmer que tous les fils d'exécution du groupe ont été exécutés
    // en même temps.
    pthread_barrier_wait(&info.barrier);
    pthread_mutex_lock(&info.lock);

    INFO("Vérification de l'exécution de la fonction de rappel");
    REQUIRE(info.count == n_threads * (i + 1));

    INFO("Vérification de la réutilisation des fils d'exécution");
    REQUIRE(info.tidmap.size() == n_threads);

    for (auto& item : info.tidmap) {
      INFO("tid: " << item.first << " réutilisation: " << item.second);
      REQUIRE(item.second == (i + 1));
    }
    pthread_mutex_unlock(&info.lock);
  }

  threadpool_join(p);
  pthread_mutex_destroy(&info.lock);
  pthread_barrier_destroy(&info.barrier);
}

TEST_CASE("RespectLimite") {
  /*
   * Ce test vérifie que la taille maximale de la file d'attente est respectée.
   */
  int n_threads = 4;
  int n = 10;
  int queue_limit = 2;
  sem_t count;
  sem_init(&count, 0, 0);

  struct pool* p = threadpool_create(n_threads, queue_limit);
  REQUIRE(p != nullptr);

  // Ajouter plus de travail que la taille de la file
  for (int i = 0; i < n; i++) {
    threadpool_add_task(p, worker_queue_limit, &count);
  }

  // Attendre la confirmation que toutes les tâches sont terminées
  for (int i = 0; i < n; i++) {
    sem_wait(&count);
  }

  int max_size = list_size_max(p->task_list);
  threadpool_join(p);

  INFO("Verification de la taille maximale de la file d'attente");
  CHECK(max_size == queue_limit);
}

TEST_CASE("TraitementImage") {
  /*
   * Ce test vérifie que l'application de l'effet sur une image fonctionne
   * correctement (test fonctionnel).
   */

  unlink(img_serial);
  unlink(img_multithread);

  {
    struct list* work_list = list_new(NULL, free_work_item);
    struct work_item* item = make_work_item(const_cast<char*>(img),  //
                                            const_cast<char*>(img_serial));
    struct list_node* node = list_node_new(item);
    list_push_back(work_list, node);
    REQUIRE(process_serial(work_list) == 0);
    list_free(work_list);
  }

  {
    struct list* work_list = list_new(NULL, free_work_item);
    struct work_item* item = make_work_item(const_cast<char*>(img),  //
                                            const_cast<char*>(img_multithread));
    struct list_node* node = list_node_new(item);
    list_push_back(work_list, node);
    REQUIRE(process_multithread(work_list, 2, 2) == 0);
    list_free(work_list);
  }

  INFO("Vérifier que l'effet est bien appliqué sur l'image");
  REQUIRE(are_files_identical(img_serial, img_multithread) == true);
}

TEST_CASE("ImplementationInterne") {
  /*
   * Ce test vérifie que le nombre d'appels aux fonction de sémaphores de la
   * mise en situation sont respectés.
   */
  wrapper_clear();
  int n = 4;
  struct pool* pool = threadpool_create(n, 10);
  REQUIRE(pool != nullptr);

  struct tidarg info;
  pthread_mutex_init(&info.lock, NULL);
  pthread_barrier_init(&info.barrier, NULL, 1 + 1);
  info.count = 0;

  SECTION("Étape 1: Création") {
    // Note: la synchronisation par un délai c'est mal, mais
    // je n'ai pas trop le choix ici, considérant que je ne
    // contrôle pas l'implémentation de l'étudiant.
    sleep(1);
    CHECK(wrapper_sem_init_count() == 2);
    CHECK(wrapper_sem_post_count() == 0);
    CHECK(wrapper_sem_wait_count() == n);
    CHECK(wrapper_sem_destroy_count() == 0);
  }

  // Ajout d'une seule tâche, puis attendre qu'elle s'exécute
  threadpool_add_task(pool, worker_test, &info);
  pthread_barrier_wait(&info.barrier);

  SECTION("Étape 2: Utilisation") {
    sleep(1);
    CHECK(wrapper_sem_init_count() == 2);
    CHECK(wrapper_sem_post_count() == 2);      // free + busy = 2
    CHECK(wrapper_sem_wait_count() == n + 2);  // free + busy = 2
    CHECK(wrapper_sem_destroy_count() == 0);
  }

  threadpool_join(pool);

  SECTION("Étape 3: Finalisation") {
    sleep(1);
    CHECK(wrapper_sem_init_count() == 2);
    CHECK(wrapper_sem_post_count() == n + 2);
    CHECK(wrapper_sem_wait_count() == n + 2);
    CHECK(wrapper_sem_destroy_count() == 2);
  }

  pthread_mutex_destroy(&info.lock);
  pthread_barrier_destroy(&info.barrier);
}

TEST_CASE("TestDeFumee") {
  /*
   * Ce test vérifie simplement la création et la suppression, sans ajouter de tâche.
   */
  struct pool* pool = threadpool_create(4, 10);
  REQUIRE(pool != nullptr);
  threadpool_join(pool);
}
