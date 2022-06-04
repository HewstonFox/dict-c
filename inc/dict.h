#pragma once

#include <stdlib.h>

typedef long int dictc_size;

typedef void *Dict;

Dict create_dict();

void *dict_get(Dict, const char *key);

void dict_set(Dict, const char *key, void *value);

void dict_remove(Dict, const char *key, void (*cleaner)(void *));

void dict_destroy(Dict, void (*cleaner)(void *));

dictc_size dict_get_len(Dict);

dictc_size dict_get_cap(Dict);

char **dict_get_keys(Dict d);

void **dict_get_values(Dict d);
