#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

/* NOTE: Not using linkedlist.h here because of the overhead cost that would
 * come with using Element->HashNode, since Element does not have a member
 * suitable for storing the hash key.
*/

#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

#define __HASH_TABLE_PTHREAD_ENABLE__ /* can disable compiling with pthread support */

typedef int hash_key_t;

struct HashTable;

struct HashTable* hash_table_init(size_t size);
void hash_table_free(struct HashTable* hash_table);
bool hash_table_insert(struct HashTable* hash_table, hash_key_t key, void* data);
void* hash_table_remove(struct HashTable* hash_table, hash_key_t key);
void* hash_table_get(const struct HashTable* hash_table, hash_key_t key);
void* hash_table_find(const struct HashTable* hash_table, void* data);
size_t hash_table_len(const struct HashTable* hash_table);

#ifdef __HASH_TABLE_PTHREAD_ENABLE__
bool hash_table_pthread_init(struct HashTable* hash_table);
void hash_table_pthread_free(struct HashTable* hash_table);
int hash_table_pthread_lock(struct HashTable* hash_table);
int hash_table_pthread_unlock(struct HashTable* hash_table);
int hash_table_pthread_wait(struct HashTable* hash_table);
int hash_table_pthread_signal(struct HashTable* hash_table);
#endif

#endif