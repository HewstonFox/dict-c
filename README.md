<h1 align="center">dict-c</h1>
<h2 align="center">C library for dictionaries</h2>
<br>
<br>
dict-c adds key-value like data structure.

To compile the library you need `make` and `clang` installed. <br>
Just run `make` in the root of the project directory.

To use the library include just created `./dictc.a` while compiling your project.

In your code you will need to include `./inc/dict.h` header.

Available functions:

```c
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
 * Set key-value pair. If key is already exist will overwrite it`s value and may cause memory leaks.
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
 * Destroy Dict instance. If "cleaner" is not NULL will call it with all the values stored to delocate them.
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
 *     printf("%s: %s\n", di->key, (char *) di->value);
 * dict_iterator_destroy(di);
 * 
 */
int dict_iterator_next(DictIterator di);
```

Usage example

```c
#include <stdio.h>
#include <string.h>
#include <inc/dict.h>

void keyed_free(void *value, const char *key) {
    printf("delocating %s\n", key);
    free(value);
}

int main() {
    Dict d = dict_create();

    // Default capacity is 16,
    // but since there is no entries yet len is 0
    printf("cap: %ld, len: %ld\n", dict_get_capacity(d), dict_count_entries(d));
    // cap: 16, len: 0

    // Add 200 entries.
    for (int i = 0; i < 200; ++i) {
        char key[11];
        char buff[13];
        sprintf(key, "%d", i);
        sprintf(buff, "$%d$", i);

        dict_set(d, key, strdup(buff));

        printf("added %d: %s\n", i, (char *) dict_get(d, key));
    }
    // If there is not enough capacity to add new pair (entries count > capacity / 2)
    // capacity expands by 16.
    printf("cap: %ld, len: %ld\n", dict_get_capacity(d), dict_count_entries(d));
    // cap: 400, len: 200

    // Remove 150 entries
    for (int i = 0; i < 150; ++i) {
        char key[10];
        sprintf(key, "%d", i);

        dict_remove(d, key, (DictEntryCleaner) free);

        printf("removed %d: %s\n", i, (char *) dict_get(d, key));
    }
    // But if "capacity / 2 > entries count",
    // capacity decreases to "capacity / 4 * 3".
    // Minimal capacity is 16.
    printf("cap: %ld, len: %ld\n", dict_get_capacity(d), dict_count_entries(d));
    // cap: 93, len: 50

    // Iterate entries
    DictIterator di = dict_iterator_create(d);
    while (dict_iterator_next(di))
        printf("%s: %s\n", di->key, (char *) di->value);
    dict_iterator_destroy(di);

    // Print available keys
    const char **keys = dict_get_keys(d);
    size_t len = dict_count_entries(d);
    for (size_t i = 0; i < len; ++i) {
        if (i) printf(", ");
        printf("%s", keys[i]);
    }
    printf("\n");
    // Since it's just array we can use "free" to delocate it.
    free(keys);

    // Print available values
    // In our case we have all values "char *" type,
    // but types can be mixed as well
    const char **values = (const char **) dict_get_values(d);
    for (size_t i = 0; i < len; ++i) {
        if (i) printf(", ");
        printf("%s", values[i]);
    }
    printf("\n");
    // Since it's just array we can use "free" to delocate it.
    free(values);

    // Destroy dict with all pairs
    // (each value will be delocated with "keyed_free" in this example).
    // If you don`t need to delocate data,
    // second argument can be "NULL".
    dict_destroy(d, keyed_free);

    // Destroyed dict can`t be used again,
    // but you can do "d = dict_create();"

    return 0;
}
```
