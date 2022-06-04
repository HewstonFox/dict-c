#pragma once

#include <stdlib.h>

typedef long int dictc_size;

typedef void *Dict;

/**
 * Create and initialise Dict instance.
 * @return Dict
 */
Dict create_dict();

/**
 * Get value by "key".
 * @param d Dict to get value
 * @param key
 * @return void*
 */
void *dict_get(Dict d, const char *key);

/**
 * Set key-value pair. If key is already exist will overwrite it`s value and may cause memory leaks.
 * @param d Dict to set value
 * @param key
 * @param value
 */
void dict_set(Dict d, const char *key, void *value);

/**
 * Remove key-value pair.
 * @param d Dict to remove key-value pair
 * @param key
 * @param cleaner May be NULL. Function to delocate value (for example "free")
 */
void dict_remove(Dict d, const char *key, void (*cleaner)(void *));

/**
 * Destroy Dict instance. If "cleaner" is not NULL will call it with all the values stored to delocate them.
 * @param d Dict to destroy
 * @param cleaner May be NULL. Function to delocate all stored values (for example "free")
 */
void dict_destroy(Dict d, void (*cleaner)(void *));

/**
 * Get count of stored key-value pairs.
 * @param d
 * @return dictc_size
 */
dictc_size dict_get_len(Dict d);

/**
 * Get capacity of current Dict instance.
 * @param d
 * @return dictc_size
 */
dictc_size dict_get_cap(Dict d);

/**
 * Get array of keys of current Dict instance.
 * @param d
 * @return
 */
char **dict_get_keys(Dict d);

/**
 * Get array of values of current Dict instance.
 * @param d
 * @return
 */
void **dict_get_values(Dict d);
