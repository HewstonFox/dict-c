#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    char **keys;
    void **values;
    ssize_t len;
    ssize_t cap;
} dict_t;

typedef void (*dictc_cleaner)(void *);

typedef dict_t *dict;

dict create_dict();

ssize_t dict_key_index(dict d, const char *key);

void *dict_get(dict, const char *key);

void dict_set(dict, const char *key, void *value);

void dict_remove(dict, const char *key, dictc_cleaner cleaner);

void dict_destroy(dict, dictc_cleaner cleaner);
