/**
 * @file binaryTree.c
 * @brief Implementação de versão paralela de arvore binária compartilhada,
 * baseada na versão sequencial do código do Prof. Nívio Ziviani
 * https://www2.dcc.ufmg.br/livros/algoritmos/cap5/codigo/c/5.4a5.10-arvorebinaria.c
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "binaryTree.h"
#include <stdio.h>
#include <stdlib.h>

void search(RegistryType *value, NodePointerType *root) {
  if (*root == NULL) {
    printf("Erro: Registro nao esta presente na arvore\n");
    return;
  }

  pthread_mutex_lock(&((*root)->mutexReadersCounter));
  if (++(*root)->numReaders == 1)
    pthread_mutex_lock(&((*root)->mutex));
  pthread_mutex_unlock(&((*root)->mutexReadersCounter));

  if (value->key < (*root)->registry.key) {

    pthread_mutex_lock(&((*root)->mutexReadersCounter));
    if (--(*root)->numReaders == 0)
      pthread_mutex_unlock(&((*root)->mutex));
    pthread_mutex_unlock(&((*root)->mutexReadersCounter));

    search(value, &(*root)->left);

  } else if (value->key > (*root)->registry.key) {

    pthread_mutex_lock(&((*root)->mutexReadersCounter));
    if (--(*root)->numReaders == 0)
      pthread_mutex_unlock(&((*root)->mutex));
    pthread_mutex_unlock(&((*root)->mutexReadersCounter));

    search(value, &(*root)->right);

  } else {
    *value = (*root)->registry;

    pthread_mutex_lock(&((*root)->mutexReadersCounter));
    if (--(*root)->numReaders == 0)
      pthread_mutex_unlock(&((*root)->mutex));
    pthread_mutex_unlock(&((*root)->mutexReadersCounter));
  }
}

void insert(RegistryType value, NodePointerType *root) {
  if (*root == NULL) {
    *root = (NodePointerType)malloc(sizeof(NodeType));
    (*root)->registry = value;
    (*root)->left = NULL;
    (*root)->right = NULL;
    pthread_mutex_init(&((*root)->mutex), NULL);
    (*root)->numReaders = 0;
    pthread_mutex_init(&((*root)->mutexReadersCounter), NULL);
    return;
  }

  pthread_mutex_lock(&((*root)->mutex));
  int isLeaf = 0;
  if (value.key < (*root)->registry.key) {

    if ((*root)->left != NULL)
      pthread_mutex_unlock(&((*root)->mutex));
    else
      isLeaf = 1;

    insert(value, &(*root)->left);

    if (isLeaf)
      pthread_mutex_unlock(&((*root)->mutex));

  } else if (value.key > (*root)->registry.key) {

    if ((*root)->right != NULL)
      pthread_mutex_unlock(&((*root)->mutex));
    else
      isLeaf = 1;

    insert(value, &(*root)->right);
    
    if (isLeaf)
      pthread_mutex_unlock(&((*root)->mutex));

  } else {
    pthread_mutex_unlock(&((*root)->mutex));
    printf("Erro : Registro %ld com o %ld ja existe na arvore\n", (*root)->registry.key, value.key);
  }
}

void initRoot(NodePointerType *root) { *root = NULL; }

void initBarreira(TBarreira *b, int n) { pthread_barrier_init(b, NULL, n); }

void barreira(TBarreira *b) { pthread_barrier_wait(b); }

void previousNode(NodePointerType q, NodePointerType *r) {
  pthread_mutex_lock(&(*r)->mutex);
  if ((*r)->right != NULL) {
    previousNode(q, &(*r)->right);
    pthread_mutex_unlock(&(*r)->mutex);
    return;
  }

  q->registry = (*r)->registry;
  q = *r;
  *r = (*r)->left;
  free(q);
}

void removeValue(RegistryType value, NodePointerType *root) {
  NodePointerType Aux;
  if (*root == NULL) {
    printf("Erro : Registro %ld nao esta na arvore\n", value.key);
    return;
  }

  pthread_mutex_lock(&((*root)->mutex));

  int isLeaf = 0;
  if (value.key < (*root)->registry.key) {

    if ((*root)->left != NULL)
      pthread_mutex_unlock(&(*root)->mutex);
    else
      isLeaf = 1;
    
    removeValue(value, &(*root)->left);

    if (isLeaf)
      pthread_mutex_unlock(&(*root)->mutex);

  } else if (value.key > (*root)->registry.key) {

    if ((*root)->right != NULL)
      pthread_mutex_unlock(&(*root)->mutex);
    else
      isLeaf = 1;

    removeValue(value, &(*root)->right);
    
    if (isLeaf)
      pthread_mutex_unlock(&(*root)->mutex);

  } else if ((*root)->right == NULL) {
    Aux = *root;
    if ((*root)->left != NULL) {
      pthread_mutex_lock(&((*root)->left->mutex));
    }
    *root = (*root)->left;
    if ((*root) != NULL)
      pthread_mutex_unlock(&((*root)->mutex));
    free(Aux);

  } else if ((*root)->left != NULL) {

    previousNode(*root, &(*root)->left);
    pthread_mutex_unlock(&((*root)->mutex));

  } else {

    Aux = *root;
    if ((*root)->right != NULL)
      pthread_mutex_lock(&((*root)->right->mutex));
    *root = (*root)->right;
    if ((*root) != NULL)
      pthread_mutex_unlock(&((*root)->mutex));
    free(Aux);
  }
}

void testI(NodeType *root, int pai) {
  if (root == NULL)
    return;
  printf("Registry Value: %ld \n", root->registry.key);
  if (root->left != NULL) {
    if (root->registry.key < root->left->registry.key) {
      printf("Erro: Pai %ld menor que filho a esquerda %ld\n",
             root->registry.key, root->left->registry.key);
      exit(1);
    }
  }
  if (root->right != NULL) {
    if (root->registry.key > root->right->registry.key) {
      printf("Erro: Pai %ld maior que filho a direita %ld\n",
             root->registry.key, root->right->registry.key);
      exit(1);
    }
  }
  testI(root->left, root->registry.key);
  testI(root->right, root->registry.key);
}

void test(NodeType *root) {
  if (root != NULL)
    testI(root, root->registry.key);
}
