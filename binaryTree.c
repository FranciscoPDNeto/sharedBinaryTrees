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

  if (value->key < (*root)->registry.key) {
    pthread_mutex_lock(&(*root)->mutexReadersCounter);
    if (++(*root)->numReaders == 1)
      pthread_mutex_lock(&(*root)->mutex);
    pthread_mutex_unlock(&(*root)->mutexReadersCounter);

    search(value, &(*root)->left);

    pthread_mutex_lock(&(*root)->mutexReadersCounter);
    if (--(*root)->numReaders == 0)
      pthread_mutex_unlock(&(*root)->mutex);
    pthread_mutex_unlock(&(*root)->mutexReadersCounter);

  } else if (value->key > (*root)->registry.key) {

    pthread_mutex_lock(&(*root)->mutexReadersCounter);
    if (++(*root)->numReaders == 1)
      pthread_mutex_lock(&(*root)->mutex);
    pthread_mutex_unlock(&(*root)->mutexReadersCounter);

    search(value, &(*root)->right);

    pthread_mutex_lock(&(*root)->mutexReadersCounter);
    if (--(*root)->numReaders == 0)
      pthread_mutex_unlock(&(*root)->mutex);
    pthread_mutex_unlock(&(*root)->mutexReadersCounter);

  } else
    *value = (*root)->registry;
}

void insert(RegistryType value, NodePointerType *root) {
  if (*root == NULL) {
    *root = (NodePointerType)malloc(sizeof(NodeType));
    (*root)->registry = value;
    (*root)->left = NULL;
    (*root)->right = NULL;
    pthread_mutex_init(&(*root)->mutex, NULL);
    (*root)->numReaders = 0;
    pthread_mutex_init(&(*root)->mutexReadersCounter, NULL);

  } else if (value.key < (*root)->registry.key) {

    pthread_mutex_lock(&(*root)->mutex);
    insert(value, &(*root)->left);
    pthread_mutex_unlock(&(*root)->mutex);

  } else if (value.key > (*root)->registry.key) {

    pthread_mutex_lock(&(*root)->mutex);
    insert(value, &(*root)->right);
    pthread_mutex_unlock(&(*root)->mutex);

  } else
    printf("Erro : Registro ja existe na arvore\n");
}

void initRoot(NodePointerType *root) { *root = NULL; }

void previousNode(NodePointerType q, NodePointerType *r) {
  if ((*r)->right != NULL) {
    previousNode(q, &(*r)->right);
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
    printf("Erro : Registro nao esta na arvore\n");
    return;
  }

  if (value.key < (*root)->registry.key) {
    pthread_mutex_lock(&(*root)->mutex);
    removeValue(value, &(*root)->left);
    pthread_mutex_unlock(&(*root)->mutex);

  } else if (value.key > (*root)->registry.key) {

    pthread_mutex_lock(&(*root)->mutex);
    removeValue(value, &(*root)->right);
    pthread_mutex_unlock(&(*root)->mutex);

  } else if ((*root)->right == NULL) {
    Aux = *root;
    *root = (*root)->left;
    free(Aux);

  } else if ((*root)->left != NULL) {

    previousNode(*root, &(*root)->left);

  } else {

    Aux = *root;
    *root = (*root)->right;
    free(Aux);
  }
}

void testI(NodeType *root, int pai) {
  if (root == NULL)
    return;
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