/**
 * @file binaryTree.c
 * @author Francisco Dias
 * @brief Versão paralela de arvore binária compartilhada, baseada na versão
 * sequencial do código do Prof. Nívio Ziviani
 * https://www2.dcc.ufmg.br/livros/algoritmos/cap5/codigo/c/5.4a5.10-arvorebinaria.c
 * @version 0.1
 * @date 2020-09-12
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX 10

typedef long TipoChave;

typedef struct TipoRegistro {
  TipoChave Chave;
  /* outros componentes */
} TipoRegistro;

typedef struct TipoNo *TipoApontador;

typedef struct TipoNo {
  TipoRegistro Reg;
  TipoApontador Esq, Dir;
  pthread_mutex_t mutex;
  int numLeitores;
  pthread_mutex_t mContatorLeitores;
} TipoNo;

typedef TipoApontador TipoDicionario;

void Pesquisa(TipoRegistro *x, TipoApontador *p) {
  if (*p == NULL) {
    printf("Erro: Registro nao esta presente na arvore\n");
    return;
  }
  if (x->Chave < (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mContatorLeitores);
    if (++(*p)->numLeitores == 1)
      pthread_mutex_lock(&(*p)->mutex);
    pthread_mutex_unlock(&(*p)->mContatorLeitores);

    Pesquisa(x, &(*p)->Esq);

    pthread_mutex_lock(&(*p)->mContatorLeitores);
    if (--(*p)->numLeitores == 0)
      pthread_mutex_unlock(&(*p)->mutex);
    pthread_mutex_unlock(&(*p)->mContatorLeitores);
    return;
  }
  if (x->Chave > (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mContatorLeitores);
    if (++(*p)->numLeitores == 1)
      pthread_mutex_lock(&(*p)->mutex);
    pthread_mutex_unlock(&(*p)->mContatorLeitores);

    Pesquisa(x, &(*p)->Dir);

    pthread_mutex_lock(&(*p)->mContatorLeitores);
    if (--(*p)->numLeitores == 0)
      pthread_mutex_unlock(&(*p)->mutex);
    pthread_mutex_unlock(&(*p)->mContatorLeitores);
  } else
    *x = (*p)->Reg;
}

void Insere(TipoRegistro x, TipoApontador *p) {
  if (*p == NULL) {
    *p = (TipoApontador)malloc(sizeof(TipoNo));
    (*p)->Reg = x;
    (*p)->Esq = NULL;
    (*p)->Dir = NULL;
    pthread_mutex_init(&(*p)->mutex, NULL);
    (*p)->numLeitores = 0;
    pthread_mutex_init(&(*p)->mContatorLeitores, NULL);
    return;
  }
  if (x.Chave < (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mutex);
    Insere(x, &(*p)->Esq);
    pthread_mutex_unlock(&(*p)->mutex);
    return;
  }
  if (x.Chave > (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mutex);
    Insere(x, &(*p)->Dir);
    pthread_mutex_unlock(&(*p)->mutex);
  } else
    printf("Erro : Registro ja existe na arvore\n");
}

void Inicializa(TipoApontador *Dicionario) { *Dicionario = NULL; }

void Antecessor(TipoApontador q, TipoApontador *r) {
  if ((*r)->Dir != NULL) {
    Antecessor(q, &(*r)->Dir);
    return;
  }
  q->Reg = (*r)->Reg;
  q = *r;
  *r = (*r)->Esq;
  free(q);
}

void Retira(TipoRegistro x, TipoApontador *p) {
  TipoApontador Aux;
  if (*p == NULL) {
    printf("Erro : Registro nao esta na arvore\n");
    return;
  }
  if (x.Chave < (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mutex);
    Retira(x, &(*p)->Esq);
    pthread_mutex_unlock(&(*p)->mutex);
    return;
  }
  if (x.Chave > (*p)->Reg.Chave) {
    pthread_mutex_lock(&(*p)->mutex);
    Retira(x, &(*p)->Dir);
    pthread_mutex_unlock(&(*p)->mutex);
    return;
  }
  if ((*p)->Dir == NULL) {
    Aux = *p;
    *p = (*p)->Esq;
    free(Aux);
    return;
  }
  if ((*p)->Esq != NULL) {
    Antecessor(*p, &(*p)->Esq);
    return;
  }
  Aux = *p;
  *p = (*p)->Dir;
  free(Aux);
}

void Central(TipoApontador p) {
  if (p == NULL)
    return;
  Central(p->Esq);
  printf("%ld\n", p->Reg.Chave);
  Central(p->Dir);
}

void TestaI(TipoNo *p, int pai) {
  if (p == NULL)
    return;
  if (p->Esq != NULL) {
    if (p->Reg.Chave < p->Esq->Reg.Chave) {
      printf("Erro: Pai %ld menor que filho a esquerda %ld\n", p->Reg.Chave,
             p->Esq->Reg.Chave);
      exit(1);
    }
  }
  if (p->Dir != NULL) {
    if (p->Reg.Chave > p->Dir->Reg.Chave) {
      printf("Erro: Pai %ld maior que filho a direita %ld\n", p->Reg.Chave,
             p->Dir->Reg.Chave);
      exit(1);
    }
  }
  TestaI(p->Esq, p->Reg.Chave);
  TestaI(p->Dir, p->Reg.Chave);
}

void Testa(TipoNo *p) {
  if (p != NULL)
    TestaI(p, p->Reg.Chave);
}

double rand0a1() {
  double resultado = (double)rand() / RAND_MAX; /* Dividir pelo maior inteiro */
  if (resultado > 1.0)
    resultado = 1.0;
  return resultado;
}

void Permut(TipoChave A[], int n) {
  int i, j;
  TipoChave b;
  for (i = n; i > 0; i--) {
    j = (i * rand0a1());
    b = A[i];
    A[i] = A[j];
    A[j] = b;
  }
}

int main(int argc, char *argv[]) {
  struct timeval t;
  TipoNo *Dicionario;
  TipoRegistro x;
  TipoChave vetor[MAX];
  int i, j, k, n;

  Inicializa(&Dicionario);
  /* Gera uma permutação aleatoria de chaves entre 1 e MAX */
  for (i = 0; i < MAX; i++)
    vetor[i] = i + 1;
  gettimeofday(&t, NULL);
  srand((unsigned int)t.tv_usec);
  Permut(vetor, MAX - 1);

  /* Insere cada chave na arvore e testa sua integridade apos cada insercao */
  for (i = 0; i < MAX; i++) {
    x.Chave = vetor[i];
    Insere(x, &Dicionario);
    printf("Inseriu chave: %d\n", x.Chave);
    Testa(Dicionario);
  }

  /* Retira uma chave aleatoriamente e realiza varias pesquisas */
  for (i = 0; i <= MAX; i++) {
    k = (int)(10.0 * rand() / (RAND_MAX + 1.0));
    n = vetor[k];
    x.Chave = n;
    Retira(x, &Dicionario);
    Testa(Dicionario);
    printf("Retirou chave: %ld\n", x.Chave);
    for (j = 0; j < MAX; j++) {
      x.Chave = vetor[(int)(10.0 * rand() / (RAND_MAX + 1.0))];
      if (x.Chave != n) {
        printf("Pesquisando chave: %ld\n", x.Chave);
        Pesquisa(&x, &Dicionario);
      }
    }
    x.Chave = n;
    Insere(x, &Dicionario);
    printf("Inseriu chave: %ld\n", x.Chave);
    Testa(Dicionario);
  }

  /* Retira a raiz da arvore ate que ela fique vazia */
  for (i = 0; i < MAX; i++) {
    x.Chave = Dicionario->Reg.Chave;
    Retira(x, &Dicionario);
    Testa(Dicionario);
    printf("Retirou chave: %ld\n", x.Chave);
  }
  return 0;
}