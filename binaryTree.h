/**
 * @file binaryTree.h
 * @brief Inteface de versão paralela de arvore binária compartilhada, baseada na versão
 * sequencial do código do Prof. Nívio Ziviani
 * https://www2.dcc.ufmg.br/livros/algoritmos/cap5/codigo/c/5.4a5.10-arvorebinaria.c
 *
 * @copyright Copyright (c) 2020
 *
 */

#include <pthread.h>

/**
 * @brief Variável que define a quantidade de elementos máximo na árvore.
 * 
 */
#define MAX 10

/**
 * @brief Tipo da chave do registro para o nó.
 * 
 */
typedef long KeyType;

/**
 * @brief Tipo do registro do nó, que no caso é composto apenas pela própria
 * chave.
 * 
 */
typedef struct RegistryType {
  KeyType key;
  /* outros componentes */
} RegistryType;

/**
 * @brief Ponteiro do Tipo nó.
 * 
 */
typedef struct NodeType *NodePointerType;

/**
 * @brief Tipo nó da arvore, composto pelo registro do nó, bem como o lado esquerdo
 * e direito da arvore binária. Como estamos querendo acesso paralelo, teremos
 * que ter o controle com o uso de mutexs de leitores e escritor.
 * 
 */
typedef struct NodeType {
  RegistryType registry;
  NodePointerType left, right;
  pthread_mutex_t mutex;
  int numReaders;
  pthread_mutex_t mutexReadersCounter;
} NodeType;

/**
 * @brief Argumentos usados para métodos de inserção e remoção de nó.
 * 
 */
typedef struct {
  RegistryType value;
  NodePointerType root;
} InsertRemoveArgs;

/**
 * @brief Argumentos usados para métodos de busca de nó.
 * 
 */
typedef struct {
  RegistryType *value;
  NodePointerType root;
} SearchArgs;

/**
 * @brief Inicializa a arvore de nós.
 * 
 * @param root Referência para a arvore que será inicializada.
 */
void initRoot(NodePointerType *root);
/**
 * @brief Pesquisa o registro passado dentro da arvore, ou ramo, passado. (Thread-safe)
 * Várias threads podem pesquisar ao mesmo tempo, mas com exclusão mútua com o 
 * insert e remove no nó.
 * 
 * @param value Registro a ser procurado na arvore.
 * @param root Arvore ou ramo a se procurar o registro.
 */
void search(RegistryType *value, NodePointerType *root);
/**
 * @brief Insere o registro passado dentro da arvore passada. (Thread-safe)
 * Só uma thread pode inserir por vez, e com exclusão mútua com o search e 
 * removeValue no nó.
 * 
 * @param value Registro a ser inserido à arvore.
 * @param root Arvore ou ramo a se inserir o registro.
 */
void insert(RegistryType value, NodePointerType *root);
/**
 * @brief Remove o registro passado dentro da arvore passada. (Thread-safe)
 * Só uma thread pode remover por vez, e com exclusão mútua com o search e insert
 * no nó.
 * 
 * @param value Registro a ser removido da arvore.
 * @param root Arvore ou ramo a se remover o registro.
 */
void removeValue(RegistryType value, NodePointerType *root);
/**
 * @brief Realiza um teste de corretude na arvore.
 * 
 * @param root Arvore a ser realizada o teste.
 */
void test(NodeType *root);