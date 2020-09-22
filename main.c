#include <limits.h>
#include <sys/time.h>
#include "binaryTree.h"

double rand0to1() {
  double result = (double)rand() / RAND_MAX; /* Dividir pelo maior inteiro */
  if (result > 1.0)
    result = 1.0;
  return result;
}

void permut(KeyType A[], int n) {
  int i, j;
  KeyType b;
  for (i = n; i > 0; i--) {
    j = (i * rand0to1());
    b = A[i];
    A[i] = A[j];
    A[j] = b;
  }
}

int main(int argc, char *argv[]) {
  struct timeval t;
  NodeType *root;
  RegistryType x;
  KeyType vetor[MAX];
  int i, j, k, n;

  initRoot(&root);
  /* Gera uma permutação aleatoria de chaves entre 1 e MAX */
  for (i = 0; i < MAX; i++)
    vetor[i] = i + 1;
  gettimeofday(&t, NULL);
  srand((unsigned int)t.tv_usec);
  permut(vetor, MAX - 1);

  /* Insere cada chave na arvore e testa sua integridade apos cada insercao */
  for (i = 0; i < MAX; i++) {
    x.key = vetor[i];
    insert(x, &root);
    printf("Inseriu chave: %ld\n", x.key);
  }
  
  test(root);

  /* Retira uma chave aleatoriamente e realiza varias pesquisas */
  for (i = 0; i <= MAX; i++) {
    k = (int)(10.0 * rand() / (RAND_MAX + 1.0));
    n = vetor[k];
    x.key = n;
    removeValue(x, &root);
    test(root);
    printf("Retirou chave: %ld\n", x.key);
    for (j = 0; j < MAX; j++) {
      x.key = vetor[(int)(10.0 * rand() / (RAND_MAX + 1.0))];
      if (x.key != n) {
        printf("Pesquisando chave: %ld\n", x.key);
        search(&x, &root);
      }
    }
    x.key = n;
    insert(x, &root);
    printf("Inseriu chave: %ld\n", x.key);
    test(root);
  }

  /* Retira a raiz da arvore ate que ela fique vazia */
  for (i = 0; i < MAX; i++) {
    x.key = root->registry.key;
    removeValue(x, &root);
    test(root);
    printf("Retirou chave: %ld\n", x.key);
  }
  return 0;
}