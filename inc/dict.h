#pragma once

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char **keys;
    void **values;
    ssize_t len;
    ssize_t cap;
} Dict;

typedef void (*dictc_cleaner)(void *);

Dict *create_dict();

ssize_t dict_key_index(Dict *d, const char *key);

void *dict_get(Dict *, const char *key);

void dict_set(Dict *, const char *key, void *value);

void dict_remove(Dict *, const char *key, dictc_cleaner cleaner);

void dict_destroy(Dict *, dictc_cleaner cleaner);
