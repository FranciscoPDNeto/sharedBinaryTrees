#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "binaryTree.h"

#define NUM_THREADS 8

/**
 * @brief Argumentos usados para chamadas dos métodos das diferentes fases
 * pelas threads.
 *
 */
typedef struct {
  RegistryType *vetor;
  TBarreira *barrier;
  NodePointerType *root;
  int threadIndex;
} PhaseArgs;


double rand0to1() {
  double result = (double)rand() / RAND_MAX; /* Dividir pelo maior inteiro */
  if (result > 1.0)
    result = 1.0;
  return result;
}

void permut(RegistryType A[], int n) {
  int i, j;
  RegistryType b;
  for (i = n; i > 0; i--) {
    j = (i * rand0to1());
    b.key = A[i].key;
    A[i].key = A[j].key;
    A[j].key = b.key;
  }
}

void *insertionPhase(void *phaseArgs) {
  PhaseArgs *insertionPhaseArgs = (PhaseArgs *) phaseArgs;
  int i;
  int threadIndex = insertionPhaseArgs->threadIndex;
  int startValue = (threadIndex * (MAX / NUM_THREADS));
  int endValue = threadIndex != NUM_THREADS - 1 ?
    ((threadIndex + 1) * (MAX / NUM_THREADS))
    :
    ((threadIndex + 1) * (MAX / NUM_THREADS)) + (MAX % NUM_THREADS);
  for (i = startValue; i < endValue; i++) {
    printf("Inseriu chave: %ld\n", insertionPhaseArgs->vetor[i].key);
    insert(insertionPhaseArgs->vetor[i], insertionPhaseArgs->root);
  }
  barreira(insertionPhaseArgs->barrier);
}

void *removeSearchInsertPhase(void *phaseArgs) {
  PhaseArgs *removeSearchInsertPhaseArgs = (PhaseArgs *) phaseArgs;
  int threadIndex = removeSearchInsertPhaseArgs->threadIndex;
  int dataPartitionAmount = MAX / NUM_THREADS;
  int startValue = (threadIndex * dataPartitionAmount);
  int i, j, k, n;
  RegistryType aux;
  RegistryType *vetor = removeSearchInsertPhaseArgs->vetor;
  for (i = 0; i < dataPartitionAmount; i++) {
    k = (int)((float) dataPartitionAmount * rand() / (RAND_MAX + 1.0)) + startValue;
    aux = vetor[k];
    removeValue(vetor[k], removeSearchInsertPhaseArgs->root);
    printf("Retirou chave: %ld\n", vetor[k].key);
    for (j = 0; j < MAX; j++) {
      k = (int)((float) dataPartitionAmount * rand() / (RAND_MAX + 1.0)) + startValue;
      if (vetor[k].key != aux.key) {
        printf("Pesquisando chave: %ld\n", vetor[k].key);
        search(&vetor[k], removeSearchInsertPhaseArgs->root);
      }
    }
    insert(aux, removeSearchInsertPhaseArgs->root);
    printf("Inseriu chave: %ld\n", aux.key);
  }
  barreira(removeSearchInsertPhaseArgs->barrier);
}

void *removalPhase(void *phaseArgs) {
  PhaseArgs *removalPhaseArgs = (PhaseArgs *) phaseArgs;
  int i;
  int threadIndex = removalPhaseArgs->threadIndex;
  int startValue = (threadIndex * (MAX / NUM_THREADS));
  int endValue = threadIndex != NUM_THREADS - 1 ?
    ((threadIndex + 1) * (MAX / NUM_THREADS))
    :
    ((threadIndex + 1) * (MAX / NUM_THREADS)) + (MAX % NUM_THREADS);
  for (i = startValue; i < endValue; i++) {
    printf("Tirando o valor %ld\n", removalPhaseArgs->vetor[i].key);
    removeValue(removalPhaseArgs->vetor[i], removalPhaseArgs->root);
  }
  barreira(removalPhaseArgs->barrier);
}

int main(int argc, char *argv[]) {
  clock_t start, end;
  struct timeval t;
  NodePointerType root;
  RegistryType vetor[MAX];
  TBarreira barrier;
  int i;
  double timeToFinish;

  // TODO Não deve ter essa quantidade de threads por conta do overhead.
  pthread_t threads[NUM_THREADS];

  initBarreira(&barrier, NUM_THREADS + 1);
  initRoot(&root);
  /* Gera uma permutação aleatoria de chaves entre 1 e MAX */
  for (i = 0; i < MAX; i++)
    vetor[i].key = i + 1;
  gettimeofday(&t, NULL);
  srand((unsigned int)t.tv_usec);
  permut(vetor, MAX - 1);

  /* Insere cada chave na arvore e testa sua integridade apos cada insercao */
  start = clock();
  PhaseArgs phaseArgs[NUM_THREADS];
  for (i = 0; i < NUM_THREADS; i++) {
    phaseArgs[i].vetor = vetor;
    phaseArgs[i].barrier = &barrier;
    phaseArgs[i].root = &root;
    phaseArgs[i].threadIndex = i;
    pthread_create(&(threads[i]), NULL, insertionPhase, &(phaseArgs[i]));
  }
  barreira(&barrier);

  end = clock();
  timeToFinish = ((double) end - start) / CLOCKS_PER_SEC;
  printf("Fase de inserção encerrada com %f operações por segundo.\n", ((double) MAX) / timeToFinish);

  test(root);

  /* Retira uma chave aleatoriamente e realiza varias pesquisas */
  start = clock();
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_create(&(threads[i]), NULL, removeSearchInsertPhase, &(phaseArgs[i]));
  }
  barreira(&barrier);

  end = clock();
  timeToFinish = ((double) end - start) / CLOCKS_PER_SEC;
  printf("Fase de inserção, pesquisa e remoção encerrada com %f operações por segundo.\n", (MAX * (MAX + 2)) / timeToFinish);

  test(root);

  /* Retira a raiz da arvore ate que ela fique vazia */
  start = clock();
  for (i = 0; i < NUM_THREADS; i++) {
    pthread_create(&(threads[i]), NULL, removalPhase, &(phaseArgs[i]));
  }
  barreira(&barrier);

  end = clock();
  timeToFinish = ((double) end - start) / CLOCKS_PER_SEC;
  printf("Fase de remoção encerrada com %f operações por segundo.\n", ((double)MAX) / timeToFinish);

  test(root);

  return 0;
}
