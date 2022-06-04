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
```

Usage example

```c
#include <stdio.h>
#include <string.h>
#include <inc/dict.h>

int main() {
    Dict d = create_dict();
    
    // Default capacity is 10, 
    // but since there is no pairs yet len is 0
    printf("cap: %ld, len: %ld\n", dict_get_cap(d), dict_get_len(d));
    // cap: 10, len: 0
    
    // Add 200 pairs.
    for (int i = 0; i < 200; ++i) {
        char i_c[10];
        sprintf(i_c, "%d", i);
        dict_set(d, i_c, strdup(i_c));
        printf("added %d: %s\n", i, (char *) dict_get(d, i_c));
    }
    // If there is not enough capacity to add new pair
    // capacity expands by 10.
    printf("cap: %ld, len: %ld\n", dict_get_cap(d), dict_get_len(d));
    // cap: 210, len: 200
    
    // Remove 150 pairs
    for (int i = 0; i < 150; ++i) {
        char i_c[10];
        sprintf(i_c, "%d", i);
        dict_remove(d, i_c, free);
        printf("removed %d\n", i);
    }
    // But if "capacity / 2 > len", 
    // capacity decreases to "capacity / 4 * 3".
    // Minimal capacity is 10.
    printf("cap: %ld, len: %ld\n", dict_get_cap(d), dict_get_len(d));
    // cap: 87, len: 50

    // Printing all available pairs
    dictc_size len = dict_get_len(d);
    char **keys = dict_get_keys(d);
    // In our case we have all values "char*" type, 
    // but types can be mixed as well
    char **values = (char **) dict_get_values(d);
    for (int i = 0; i < len; ++i) {
        printf("%s: %s\n", keys[i], values[i]);
    }

    // Destroy dict with all pairs 
    // (each value will be delocated with "free").
    // If you don`t need to delocate data, 
    // second argument can be "NULL".
    dict_destroy(d, free);
    
    // Destroyed dict can`t be used again,
    // but you can do "d = create_dict();"
    
    return 0;
}
```
