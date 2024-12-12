#include "threadpool.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

void* worker(void* arg) {
  /*
   * À compléter:
   *
   * Attendre que tous les fils d'exécution soient démarrés en utilisant une
   * barrière. Ceci permet de s'assurer que tous les fils d'exécution sont en
   * fonction quand threadpool_create() retourne.
   *
   * Dans une boucle infinie, on vérifie s'il y a des tâches dans la liste.
   * Si c'est le cas, on prend un élément de la liste et on exécute la fonction.
   * La fonction est l'argument sont spécifiés dans l'item de la liste:
   *
   *    task->func(task->arg);
   *
   * On doit attendre un élément à traiter en utilisant sem_wait().
   *
   * Attention: la liste peut être corrompue si on l'accède dans plusieurs
   * fils d'exécution en même temps. Il faut donc protéger l'accès avec un
   * verrou. Il faut également le verrou pour lire la variable pool->running.
   *
   * La boucle doit se terminer si pool->running est faux ET que la liste
   * pool->task_list est vide.
   */

  struct worker_arg* w = arg;
  struct pool* pool = w->pool;

  return NULL;
}

struct pool* threadpool_create(int nb_threads, int queue_limit) {
  /*
   * À compléter:
   *
   * Allouer la structure que la fonction retourne struct pool*
   *
   * Passer en revue tous les champs et initialiser correctement, en allouant la
   * mémoire au besoin. La taille de la file d'attente ne doit jamais dépasser
   * queue_limit.
   *
   * Initialiser le verrou lock
   * Initialiser les sémaphore work_busy et work_free
   * Initialiser la barrière
   * Créer la liste de travail task_list
   *
   * Démarrer les fils d'exécution
   *
   * Attendre la confirmation qu'ils sont tous démarrés avec la barrière. La
   * fonction doit retourner uniquement quand tous les fils ont été démarrés.
   *
   * Retourner le pointeur vers la structure.
   */

  return NULL;
}

void threadpool_add_task(struct pool* pool, func_t fn, void* arg) {
  /*
   * À compléter:
   *
   * Allouer un struct task, assigner func et arg passés en argument.
   *
   * Ensuite, créer un nouveau noeud de liste pour contenir la tâche créée.
   * Ajouter ce noeuds à la fin de la liste.
   *
   * Incrémenter le sémaphore pour réveiller un fil d'exécution.
   *
   * Attention: il faut toujours accéder à la liste avec un verrou pour éviter
   * une condition critique!
   */
}

void threadpool_join(struct pool* pool) {
  /*
   * À compléter:
   *
   * Mettre pool->running à 0 pour indiquer que l'on désire terminer
   * l'exécution. Ensuite, on incrémente le sémaphore une fois pour
   * chaque fil d'exécution, de manière à les réveiller.
   *
   * Attendre que tous les fils d'exécution se terminent, puis libérer la
   * mémoire allouée dans threadpool_create().
   *
   * Quand la fonction retourne, la structure pool n'est plus utilisable.
   */

  return;
}
