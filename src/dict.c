#include "dict.h"

typedef struct {
    char **keys;
    void **values;
    dictc_size len;
    dictc_size cap;
} DictInternals;

#define PRIVATE(x) ((DictInternals *)(x))
#define INITIAL_CAP 10

static void *d_malloc(unsigned long size) {
    return malloc(size);
}

static void d_free(void *ptr) {
    free(ptr);
}

static int strlen(const char *s) {
    int count = 0;
    while (s[count])
        count++;
    return count;
}

static char *strcpy(char *dst, const char *src) {
    char *orig = dst;
    while (*src)
        *dst++ = *src++;
    *dst = *src;
    return orig;
}

static int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

static char *strnew(const int size) {
    char *str = (char *) d_malloc((size + 1));
    if (str == NULL)
        return NULL;
    for (int i = 0; i <= size; ++i)
        str[i] = '\0';
    return str;
}

static char *strdup(const char *s1) {
    char *dup = strnew(strlen(s1));
    strcpy(dup, s1);
    return dup;
}


static dictc_size dict_key_index(DictInternals *d, const char *key) {
    if (!key || d->len == 0)
        return -1;
    for (dictc_size i = 0; i < d->len; ++i)
        if (strcmp(d->keys[i], key) == 0)
            return i;

    return -1;
}

static dictc_size get_dict_expanded_size(DictInternals *d) {
    return d->cap + INITIAL_CAP;
}

static dictc_size get_dict_reduced_size(DictInternals *d) {
    dictc_size res = d->cap / 4 * 3;
    return res > INITIAL_CAP ? res : INITIAL_CAP;
}

static int dict_should_expand(DictInternals *d) {
    return d->len == d->cap;
}

static int dict_should_reduce(DictInternals *d) {
    return d->cap > INITIAL_CAP && d->len < d->cap / 2;
}

static void dict_resize(DictInternals *d, dictc_size new_cap) {
    if (new_cap < d->len) return;
    d->cap = new_cap;
    void **new_values = (void **) d_malloc(sizeof(void *) * d->cap);
    char **new_keys = (char **) d_malloc(sizeof(char *) * d->cap);
    for (int i = 0; i < d->len; ++i) {
        new_values[i] = d->values[i];
        new_keys[i] = d->keys[i];
    }
    d_free(d->values);
    d_free(d->keys);
    d->values = new_values;
    d->keys = new_keys;
}


dictc_size dict_get_len(Dict d) {
    return PRIVATE(d)->len;
}

dictc_size dict_get_cap(Dict d) {
    return PRIVATE(d)->cap;
}

char **dict_get_keys(Dict d) {
    return PRIVATE(d)->keys;
}

void **dict_get_values(Dict d) {
    return PRIVATE(d)->values;
}

Dict create_dict() {
    DictInternals *d = PRIVATE(d_malloc(sizeof(DictInternals)));
    d->cap = INITIAL_CAP;
    d->keys = (char **) d_malloc(sizeof(char *) * d->cap);
    d->values = (void **) d_malloc(sizeof(void *) * d->cap);
    d->len = 0;
    return (Dict) d;
}

void *dict_get(Dict d, const char *key) {
    DictInternals *_d = PRIVATE(d);
    dictc_size idx = dict_key_index(_d, key);
    return idx != -1 ? _d->values[idx] : ((void *) 0);
}

void dict_set(Dict d, const char *key, void *value) {
    DictInternals *_d = PRIVATE(d);
    dictc_size exist_index = dict_key_index(d, key);
    if (exist_index != -1) {
        _d->values[exist_index] = value;
        return;
    }
    _d->keys[_d->len] = strdup(key);
    _d->values[_d->len] = value;
    _d->len++;
    if (dict_should_expand(_d)) dict_resize(_d, get_dict_expanded_size(_d));
}

void dict_remove(Dict d, const char *key, void (*cleaner)(void *)) {
    DictInternals *_d = PRIVATE(d);
    dictc_size idx = dict_key_index(_d, key);
    if (idx == -1)
        return;
    if (cleaner)
        cleaner(_d->values[idx]);
    d_free(_d->keys[idx]);
    for (dictc_size i = idx; i < _d->len - 1; ++i) {
        _d->values[i] = _d->values[i + 1];
        _d->keys[i] = _d->keys[i + 1];
    }
    _d->len--;
    if (dict_should_reduce(_d)) dict_resize(_d, get_dict_reduced_size(_d));
}

void dict_destroy(Dict d, void (*cleaner)(void *)) {
    DictInternals *_d = PRIVATE(d);
    if (cleaner)
        for (dictc_size i = 0; i < _d->len; i++)
            cleaner((_d->values)[i]);

    d_free(_d->values);
    for (dictc_size i = 0; i < _d->len; ++i)
        d_free(_d->keys[i]);
    d_free(_d->keys);
    d_free(_d);
}
