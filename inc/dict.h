//
// Created by Hewston on 6/5/2022.
//

#pragma once

#include <stdlib.h>

typedef struct Dict_s *Dict;
typedef struct DictIterator_s *DictIterator;

typedef void (*DictEntryCleaner)(void *value, const char *key);

struct DictIterator_s {
    const char *key;
    void *value;

    void *_internals; // do not touch
};

/**
 * Create and initialise Dict instance.
 * @return Dict
 */
Dict dict_create(void);

/**
 * Get value by key.
 * @param d Dict to get value
 * @param key
 * @return void*
 */
void *dict_get(Dict d, const char *key);

/**
 * Set key-value pair.
 * If key is already exist will overwrite it`s value and may cause memory leaks.
 * @param d Dict to set value
 * @param key
 * @param value
 * @return Entry key on success and NULL otherwise
 */
const char *dict_set(Dict d, const char *key, void *value);

/**
 * Remove key-value pair.
 * @param d Dict to remove key-value pair
 * @param key
 * @param cleaner May be NULL. Function to delocate value (for example "free"),
 * also supports 2nd argument - entry key.
 * @example
 *
 * void cleaner(void *value, const char *key) {
 *     if (strcmp(key, "key2") == 0)
 *         free(value)
 * }
 *
 * int main() {
 *     dict_remove(d, "key1", NULL);
 *     dict_remove(d, "key2", cleaner);
 *     dict_remove(d, "key3", (DictEntryCleaner)(free));
 * }
 *
 */
void dict_remove(Dict d, const char *key, DictEntryCleaner cleaner);

/**
 * Destroy Dict instance.
 * If "cleaner" is not NULL will call it with all the values stored to delocate them.
 * @param d Dict to destroy
 * @param cleaner May be NULL. Function to delocate all stored values (for example "free")
 */
void dict_destroy(Dict d, DictEntryCleaner cleaner);

/**
 * Get count of stored entries.
 * @param d
 * @return number of entries
 */
size_t dict_count_entries(Dict d);

/**
 * Get capacity of current Dict instance.
 * @param d
 * @return capacity
 */
size_t dict_get_capacity(Dict d);

/**
 * Get array of keys of current Dict instance.
 * @param d
 * @return array of keys
 */
const char **dict_get_keys(Dict d);

/**
 * Get array of values of current Dict instance.
 * @param d
 * @return array of values
 */
void **dict_get_values(Dict d);

/**
 * Get iterator for current Dict instance
 * @param d
 * @return iterator
 */
DictIterator dict_iterator_create(Dict d);

/**
 * Destroy dict iterator
 * @param di
 */
void dict_iterator_destroy(DictIterator di);

/**
 * Dict iterators next
 * @param di
 * @return 1 if iteration continues, 0 if iteration ended
 * @example
 *
 * DictIterator di = dict_iterator_create(d);
 * while (dict_iterator_next(di))
 *      printf("%s: %s\n", di->key, (char *) di->value);
 * dict_iterator_destroy(di);
 *
 */
int dict_iterator_next(DictIterator di);
