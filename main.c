#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "binaryTree.h"

#define NUM_THREADS 4

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
  insert(insertionPhaseArgs->vetor[insertionPhaseArgs->threadIndex], insertionPhaseArgs->root);
  barreira(insertionPhaseArgs->barrier);
}

void *removeSearchInsertPhase(void *phaseArgs) {
  PhaseArgs *removeSearchInsertPhaseArgs = (PhaseArgs *) phaseArgs;
  int i, j, k, n;
  RegistryType aux;
  RegistryType *vetor = removeSearchInsertPhaseArgs->vetor;
  k = (int)(10.0 * rand() / (RAND_MAX + 1.0));
  aux = vetor[k];
  removeValue(vetor[k], removeSearchInsertPhaseArgs->root);
  printf("Retirou chave: %ld\n", vetor[k].key);
  for (j = 0; j < MAX; j++) {
    k = (int)(10.0 * rand() / (RAND_MAX + 1.0));
    if (vetor[k].key != aux.key) {
      printf("Pesquisando chave: %ld\n", vetor[k].key);
      search(&vetor[k], removeSearchInsertPhaseArgs->root);
    }
  }
  insert(aux, removeSearchInsertPhaseArgs->root);
  printf("Inseriu chave: %ld\n", aux.key);
  barreira(removeSearchInsertPhaseArgs->barrier);
}

void *removalPhase(void *phaseArgs) {
  PhaseArgs *removalPhaseArgs = (PhaseArgs *) phaseArgs;
  removeValue(removalPhaseArgs->vetor[removalPhaseArgs->threadIndex], removalPhaseArgs->root);
  barreira(removalPhaseArgs->barrier);
}

int main(int argc, char *argv[]) {
  struct timeval t;
  NodePointerType root;
  RegistryType vetor[MAX];
  TBarreira barrier;
  int i;

  // TODO Não deve ter essa quantidade de threads por conta do overhead.
  pthread_t threads[MAX];

  initBarreira(&barrier, MAX + 1);
  initRoot(&root);
  /* Gera uma permutação aleatoria de chaves entre 1 e MAX */
  for (i = 0; i < MAX; i++)
    vetor[i].key = i + 1;
  gettimeofday(&t, NULL);
  srand((unsigned int)t.tv_usec);
  permut(vetor, MAX - 1);

  /* Insere cada chave na arvore e testa sua integridade apos cada insercao */
  PhaseArgs phaseArgs[MAX];
  for (i = 0; i < MAX; i++) {
    phaseArgs[i].vetor = vetor;
    phaseArgs[i].barrier = &barrier;
    phaseArgs[i].root = &root;
    phaseArgs[i].threadIndex = i;
    pthread_create(&(threads[i]), NULL, insertionPhase, &(phaseArgs[i]));
  }
  barreira(&barrier);
  test(root);

  /* Retira uma chave aleatoriamente e realiza varias pesquisas */
  // for (i = 0; i < MAX; i++) {
  //   pthread_create(&(threads[i]), NULL, removeSearchInsertPhase, &(phaseArgs[i]));
  // }
  // barreira(&barrier);
  // test(root);

  /* Retira a raiz da arvore ate que ela fique vazia */
  for (i = 0; i < MAX; i++) {
    pthread_create(&(threads[i]), NULL, removalPhase, &(phaseArgs[i]));
  }
  barreira(&barrier);
  test(root);

  return 0;
}
