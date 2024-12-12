#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void* worker(void* arg) {
    struct worker_arg* w = arg;
    struct pool* pool = w->pool;

    // Synchronisation des threads avec une barrière
    pthread_barrier_wait(&pool->ready);

    while (1) {
        // Attendre qu'une tâche soit disponible
        sem_wait(&pool->work_busy);

        pthread_mutex_lock(&pool->lock);
        // Vérifier si le pool doit s'arrêter et si la liste est vide
        if (!pool->running && list_empty(pool->task_list)) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        // Récupérer une tâche depuis la liste
        struct list_node* node = list_pop_front(pool->task_list);
        pthread_mutex_unlock(&pool->lock);

        if (node) {
            // Exécuter la fonction associée à la tâche
            struct task* task = (struct task*)node->data;
            task->func(task->arg);
            free(task);
            free(node);
        }

        // Indiquer qu'une place est libre dans la file
        sem_post(&pool->work_free);
    }

    return NULL;
}

struct pool* threadpool_create(int nb_threads, int queue_limit) {
    // Allouer la mémoire pour la structure du pool
    struct pool* pool = malloc(sizeof(struct pool));
    if (!pool) return NULL;

    pool->nb_threads = nb_threads;
    pool->threads = malloc(nb_threads * sizeof(pthread_t));
    pool->args = malloc(nb_threads * sizeof(struct worker_arg));
    pool->task_list = list_new(NULL, free);
    pool->running = 1;
    pool->task_list->max_size = queue_limit;

    // Vérifier si toutes les allocations ont réussi
    if (!pool->threads || !pool->args || !pool->task_list) {
        free(pool->threads);
        free(pool->args);
        list_free(pool->task_list);
        free(pool);
        return NULL;
    }

    // Initialiser les verrous et les sémaphores
    pthread_mutex_init(&pool->lock, NULL);
    sem_init(&pool->work_busy, 0, 0);
    sem_init(&pool->work_free, 0, queue_limit);
    pthread_barrier_init(&pool->ready, NULL, nb_threads);

    // Créer les threads et les associer au pool
    for (int i = 0; i < nb_threads; i++) {
        pool->args[i].id = i;
        pool->args[i].pool = pool;
        pthread_create(&pool->threads[i], NULL, worker, &pool->args[i]);
    }

    // Attendre que tous les threads soient prêts
    pthread_barrier_wait(&pool->ready);

    return pool;
}

void threadpool_add_task(struct pool* pool, func_t fn, void* arg) {
    // Créer une nouvelle tâche
    struct task* new_task = malloc(sizeof(struct task));
    if (!new_task) return;

    new_task->func = fn;
    new_task->arg = arg;

    // Créer un noeud pour la tâche
    struct list_node* node = list_node_new(new_task);
    if (!node) {
        free(new_task);
        return;
    }

    // Attendre qu'une place soit disponible dans la file
    sem_wait(&pool->work_free);

    pthread_mutex_lock(&pool->lock);
    // Ajouter la tâche à la fin de la liste
    list_push_back(pool->task_list, node);
    pthread_mutex_unlock(&pool->lock);

    // Signaler qu'une nouvelle tâche est disponible
    sem_post(&pool->work_busy);
}

void threadpool_join(struct pool* pool) {
    pthread_mutex_lock(&pool->lock);
    // Signaler aux threads qu'ils doivent s'arrêter
    pool->running = 0;
    pthread_mutex_unlock(&pool->lock);

    // Réveiller tous les threads en attente
    for (int i = 0; i < pool->nb_threads; i++) {
        sem_post(&pool->work_busy);
    }

    // Attendre la fin de tous les threads
    for (int i = 0; i < pool->nb_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // Libérer les ressources
    pthread_mutex_destroy(&pool->lock);
    sem_destroy(&pool->work_busy);
    sem_destroy(&pool->work_free);
    pthread_barrier_destroy(&pool->ready);

    list_free(pool->task_list);
    free(pool->threads);
    free(pool->args);
    free(pool);
}
