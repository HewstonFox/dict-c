#include "dict.h"

static char *clonestr(const char *src) {
    size_t len = strlen(src);
    char *dst = (char *) malloc(sizeof(char) * (len + 1));
    strcpy(dst, src);
    return dst;
}

dict create_dict() {
    dict d = (dict) malloc(sizeof(dict_t));
    d->cap = 10;
    d->keys = (char **) malloc(sizeof(char *) * d->cap);
    d->values = (void **) malloc(sizeof(void *) * d->cap);
    d->len = 0;
    return d;
}

ssize_t dict_key_index(dict d, const char *key) {
    if (!key || d->len == 0) return -1;
    for (ssize_t i = 0; i < d->len; ++i)
        if (strcmp(d->keys[i], key) == 0) return i;

    return -1;
}

void *dict_get(dict d, const char *key) {
    ssize_t idx = dict_key_index(d, key);
    return idx != -1 ? d->values[idx] : NULL;
}

void dict_set(dict d, const char *key, void *value) {
    ssize_t exist_index = dict_key_index(d, key);
    if (exist_index != -1) {
        d->values[exist_index] = value;
        return;
    }
    d->keys[d->len] = clonestr(key);
    d->values[d->len] = value;
    d->len++;
    if (d->len == d->cap) {
        d->cap = d->cap * 2;
        void **new_values = (void **) malloc(sizeof(void *) * d->cap);
        char **new_keys = (char **) malloc(sizeof(char *) * d->cap);
        for (int i = 0; i < d->len; ++i) {
            new_values[i] = d->values[i];
            new_keys[i] = d->keys[i];
        }
        free(d->values);
        free(d->keys);
        d->values = new_values;
        d->keys = new_keys;
    }
}

void dict_remove(dict d, const char *key, dictc_cleaner cleaner) {
    ssize_t idx = dict_key_index(d, key);
    if (idx == -1) return;
    if (cleaner) cleaner(d->values[idx]);
    free(d->keys[idx]);
    for (ssize_t i = idx; i < d->len - 1; ++i) {
        d->values[i] = d->values[i + 1];
        d->keys[i] = d->keys[i + 1];
    }
    d->len--;
}

void dict_destroy(dict d, dictc_cleaner cleaner) {
    if (cleaner)
        for (ssize_t i = 0; i < d->len; i++)
            cleaner((d->values)[i]);

    free(d->values);
    for (ssize_t i = 0; i < d->len; ++i)
        free(d->keys[i]);
    free(d->keys);
    free(d);
}


