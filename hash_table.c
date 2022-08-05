#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "hash_table.h"

struct Node
{
	hash_key_t key;
	void* data;
	struct Node* node_next;
	struct Node* node_prev;
};

struct HashTableList
{
	struct Node* head;
	struct Node* tail;
};

struct HashTable
{
	struct HashTableList* table;
	size_t table_size;
	size_t length;
#ifdef __HASH_TABLE_PTHREAD_ENABLE__
	struct {
		pthread_mutex_t mutex;
		pthread_cond_t cond_change;
		bool initialized;
	} pthread;
#endif
};

static inline size_t hash_to_index(const size_t size, const hash_key_t key)
{
	return key % size;
}

static inline struct Node* node_init(hash_key_t key, void* data)
{
	struct Node* node;

	if ((node = malloc(sizeof(*node))) == NULL)
		return NULL;

	node->key = key;
	node->data = data;
	node->node_next = NULL;
	node->node_prev = NULL;

	return node;
}

struct HashTable* hash_table_init(const size_t size)
{
	if (size < 1)
		return NULL;

	struct HashTable* hash_table = calloc(1, sizeof(*hash_table));
	if (hash_table == NULL)
		return NULL;

	struct HashTableList* table = calloc(size, sizeof(*hash_table->table));
	if (table == NULL)
		return NULL;

	hash_table->table = table;
	hash_table->table_size = size;

	return hash_table;
}

void hash_table_free(struct HashTable* hash_table)
{
	/* free HashTableList and Node(s) */
	const size_t table_size = hash_table->table_size;
	struct HashTableList* list = &hash_table->table[0];
	while (list < &hash_table->table[table_size]) {
		struct Node* node = list->head;
		
		while (node != NULL) {
			free(node);
			node = node->node_next;
		}

		list++;
	}

	free(hash_table->table);
	free(hash_table);
}

bool hash_table_insert(struct HashTable* hash_table, hash_key_t key, void* data)
{
	struct HashTableList* list = &hash_table->table[hash_to_index(hash_table->table_size, key)];

	struct Node* node_search = list->head;
	while (node_search != NULL) {
		if (node_search->key == key)
			break;
		node_search = node_search->node_next;
	}

	/* found matching key; cannot resolve hash collision */
	if (node_search != NULL)
		return false;

	struct Node* node_new = node_init(key, data);
	if (node_new == NULL)
		return false;

	/* insert node into tail */
	if (list->tail != NULL) {
		list->tail->node_next = node_new;
		list->tail = node_new;
	} else {
		list->head = node_new;
		list->tail = node_new;
	}

	hash_table->length++;

	return true;
}

void* hash_table_remove(struct HashTable* hash_table, hash_key_t key)
{
	struct HashTableList* list = &hash_table->table[hash_to_index(hash_table->table_size, key)];

	struct Node* node_search = list->head;
	while (node_search != NULL) {
		if (node_search->key == key)
			break;
		node_search = node_search->node_next;
	}

	/* found matching key */
	if (node_search != NULL) {
		/* link surrounding nodes */
		if (node_search->node_next != NULL)
			node_search->node_next->node_prev = node_search->node_prev;
		if (node_search->node_prev != NULL)
			node_search->node_prev->node_next = node_search->node_next;

		/* link list head & tail */
		if (list->head == node_search)
			list->head = node_search->node_next;
		if (list->tail == node_search)
			list->tail = node_search->node_prev;

		void* data = node_search->data;

		free(node_search);

		hash_table->length--;

		return data;
	}

	return NULL;
}

void* hash_table_get(const struct HashTable* hash_table, hash_key_t key)
{
	struct HashTableList* list = &hash_table->table[hash_to_index(hash_table->table_size, key)];

	struct Node* node_search = list->head;
	while (node_search != NULL) {
		if (node_search->key == key)
			break;
		node_search = node_search->node_next;
	}

	/* found matching key */
	if (node_search != NULL)
		return node_search->data;

	return NULL;
}

void* hash_table_find(const struct HashTable* hash_table, void* data)
{
	const size_t table_size = hash_table->table_size;
	struct HashTableList* list = &hash_table->table[0];
	while (list < &hash_table->table[table_size]) {
		struct Node* node_search = list->head;
		
		/* find matching data */
		while (node_search != NULL) {
			if (node_search->data == data)
				return node_search->data;
			node_search = node_search->node_next;
		}

		list++;
	}

	return NULL;
}

size_t hash_table_len(const struct HashTable* hash_table)
{
	return hash_table->length;
}

#ifdef __HASH_TABLE_PTHREAD_ENABLE__

bool hash_table_pthread_init(struct HashTable* hash_table)
{
	if (hash_table->pthread.initialized)
		return false;

	if (pthread_mutex_init(&hash_table->pthread.mutex, NULL) != 0)
		return false;

	if (pthread_cond_init(&hash_table->pthread.cond_change, NULL) != 0) {
		pthread_mutex_destroy(&hash_table->pthread.mutex);
		return false;
	}

	hash_table->pthread.initialized = true;

	return true;
}

void hash_table_pthread_free(struct HashTable* hash_table)
{
	pthread_mutex_destroy(&hash_table->pthread.mutex);
	pthread_cond_destroy(&hash_table->pthread.cond_change);
}

int hash_table_pthread_lock(struct HashTable* hash_table)
{
	return pthread_mutex_lock(&hash_table->pthread.mutex);
}

int hash_table_pthread_unlock(struct HashTable* hash_table)
{
	return pthread_mutex_unlock(&hash_table->pthread.mutex);
}

int hash_table_pthread_wait(struct HashTable* hash_table)
{
	return pthread_cond_wait(&hash_table->pthread.cond_change, &hash_table->pthread.mutex);
}

int hash_table_pthread_signal(struct HashTable* hash_table)
{
	return pthread_cond_signal(&hash_table->pthread.cond_change);
}

#endif