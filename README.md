# INF3173-243-TP3 - Traitement d'image

## Mise en situation

Vous devez réaliser le traitement de centaines d’images dans le cadre d’un pré-traitement pour l’apprentissage machine. Le code applique des filtres pour détecter les contours. Votre collègue a réalisé le prototype ieffect pour le traitement d’image. Le programme liste les images au format PNG dans le répertoire d’entrée, applique l’effet et sauvegarde le résultat dans le répertoire de sortie.
Voici le fonctionnement du programme :

```
# Télécharger les images de test
./data/fetch.sh

# Ajouter le programme dans le PATH par simplicité
source ./build/env.sh

# Traitement série des images du répertoire data
ieffect --input data/ --output results/

# Traitement multithread des images du répertoire data
ieffect --multithread --input data/ --output results/
```

Dans la version de base, chaque image est traitée l’une à la suite de l’autre en utilisant un seul processeur. Cependant, la plupart des ordinateurs ont plusieurs processeurs, et donc il est généralement possible de traiter plusieurs images en même temps.

Une technique classique pour utiliser tous les processeurs de la machine consiste à utiliser un `ThreadPool`. Il s’agit de démarrer un groupe de fils d’exécution, puis d’ajouter les tâches à traiter dans une file d'attente. Un fil d’exécution disponible dans le groupe prend une tâche et l’exécute. Plutôt que de démarrer un fil d’exécution spécifiquement pour une tâche, les fils d’exécution sont réutilisés, ce qui réduit le surcoût.

Il existes plusieurs implémentations de `ThreadPool` disponibles dans tous les environnement de programmation, mais dans ce TP, le but est de réaliser votre propre implémentation simple. Vous connaîtrez ainsi le fonctionnement interne et pourrez appliquer ces connaissances dans le futur dans une foule de domaines. 

votre implémentation doit se baser sur les sémaphores. La structure de données `struct pool` est fournie. Vous devez compléter les fonctions suivantes :

```
// Fichier threadpool.c
struct pool *threadpool_create(int num);
void threadpool_add_task(struct pool *pool, func_t fn, void *arg);
void threadpool_join(struct pool *pool);
void *worker(void *arg);

// Fichier ieffect.c
int process_multithread();
```

* La fonction threadpool_create() alloue et initialise toutes les données requises pour l’objet. Le démarrage des fils d’exécution se fait dans cette fonction. La fonction doit retourner uniquement quand tous les fils d’exécution sont réellement démarrés. En cas d’erreur, la structure doit être supprimée et un pointeur NULL doit être retourné. 
* La fonction threadpool_add_task() ajoute une tâche dans la file d’attente. Le type func_t est l’adresse de la fonction à exécuter. La fonction sera éventuellement appelée avec l’argument arg en paramètre. Cette fonction ne doit jamais bloquer, même si tous les fils d’exécution sont occupés. On doit limiter la taille de la file d’attente pour éviter de charger toutes les images en même temps en mémoire.
* La fonction threadpool_join() attend la fin du traitement de toutes les tâches en file. Lorsque toutes les tâches sont terminées, on s’assure que les fils d’exécutions soient terminés, puis on libère les ressources de `struct pool`.
* La fonction process_multithread() doit réaliser le même traitement que process_serial(), soit appliquer les effets sur les images, mais en utilisant votre implémentation de `ThreadPool`.

Voici la liste des fonctions que vous pouvez utiliser, en plus des fonctions standard, de votre l’API ThreadPool et des fonctions de gestion de liste  (voir le fichier test_list.c pour des exemples d’utilisation).

```
pthread_create()
pthread_join()
pthread_mutex_lock()
pthread_mutex_unlock()
sem_init()
sem_wait()
sem_post()
```

Le test fourni crée un `ThreadPool` et ajoute une tâche spécialement pour le test. On vérifie aussi que les fonctions soient bien appelées comme on s’attend en incrémentant le nombre d’appel total. Pour vérifier que les fils d’exécutions sont bien réutilisés, on utilise le `Thread ID` comme clé dans un tableau associatif (i.e. hash map), à laquelle on associe un compteur, incrémenté à chaque appel. Comme le TID est constant pendant la durée de vie d’un fil d’exécution, alors on peut vérifier qu’ils sont bel et bien réutilisés.

## Correction sur Hopper

Votre code sera corrigé sur le système [Hopper](https://hopper.info.uqam.ca). Le système vous permet de former votre équipe pour ce TP. Faire l'archive avec `make remise` ou `ninja remise`, puis envoyer l'archive produite sur le serveur de correction. Votre note sera attribuée automatiquement. Vous pouvez soumettre votre travail plusieurs fois et la meilleure note sera conservée. D'autres détails et instructions pour l'utilisation de ce système seront fournis.

Barème de correction:

 * Implémentation correcte de ThreadPool
 * Fonctionnement global correct des effets
 * Respect du style (voir fichier .clang-format): pénalité max 10
 * Qualité (fuite mémoire, gestion d'erreur, avertissements, etc.): pénalité max 10
 * Total sur 100 points

Le non-respect de la mise en situation pourrait faire en sorte que des tests échouent. Il est inutile de modifier les tests pour les faire passer artificiellement, car un jeu de test privé est utilisé. Se référer aux résultats de test pour le détail des éléments vérifiés et validés. En cas de problème, contactez votre enseignant.

Bon travail !

# Note sur les logiciels externes

Le code intègre les librairies et les programmes suivants.

* https://github.com/catchorg/Catch2
* https://github.com/andyleejordan/c-list
* https://github.com/tidwall/hashmap.c
* https://github.com/eternalharvest/crc32-11

