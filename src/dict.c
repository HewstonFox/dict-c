//
// Created by Hewston on 6/5/2022.
//

#include "dict.h"
#include <stdint.h>
#include <string.h>

uint64_t hash_key(const char *src);


typedef struct {
    char *key;
    void *value;
} DictEntry;


struct Dict_s {
    DictEntry *entries;
    size_t capacity;
    size_t length;
};


typedef struct {
    Dict dict;
    size_t index;
} DictIteratorInternals;


#define INITIAL_CAPACITY 16


static char *_strnew(size_t size) {
    char *str = (char *) malloc((size + 1));
    if (str == NULL)
        return NULL;
    for (size_t i = 0; i <= size; ++i)
        str[i] = '\0';
    return str;
}

static char *_strdup(const char *s1) {
    char *dup = _strnew(strlen(s1));
    strcpy(dup, s1);
    return dup;
}


static size_t get_dict_expanded_size(Dict d) {
    return d->capacity + INITIAL_CAPACITY;
}

static size_t get_dict_reduced_size(Dict d) {
    size_t res = d->capacity / 4 * 3;
    return res > INITIAL_CAPACITY ? res : INITIAL_CAPACITY;
}

static int dict_should_expand(Dict d) {
    return d->length >= d->capacity / 2;
}

static int dict_should_reduce(Dict d) {
    return d->capacity > INITIAL_CAPACITY && d->length < d->capacity / 2;
}

static size_t dict_get_index(size_t capacity, const char *key) {
    return (size_t) (hash_key(key) % (uint64_t) capacity);
}

static size_t entries_find_index(DictEntry *entries, size_t capacity, const char *key, int *found) {
    size_t index = dict_get_index(capacity, key);

    if (found != NULL) (*found) = 0;

    size_t init_index = index;
    while (entries[index].key != NULL) {

        if (strcmp(key, entries[index].key) == 0) {
            if (found != NULL) (*found) = 1;
            return index;
        }

        index++;
        if (index == init_index) return 0;
        if (index >= capacity) index = 0;
    }
    return index;
}

static const char *dict_set_entry(DictEntry *entries, size_t capacity, const char *key, void *value, size_t *length) {
    int found;
    size_t index = entries_find_index(entries, capacity, key, &found);
    if (found) {
        entries[index].value = value;
        return entries[index].key;
    }

    if (length != NULL) {
        key = _strdup(key);
        if (key == NULL) return NULL;
        (*length)++;
    }

    entries[index].key = (char *) key;
    entries[index].value = value;

    return key;
}

static int dict_relocate(Dict d, size_t new_capacity) {
    DictEntry *new_entries = (DictEntry *) calloc(new_capacity, sizeof(DictEntry));
    if (new_entries == NULL) return 0;
    char *k;
    for (size_t i = 0; i < d->capacity; i++) {
        DictEntry entry = d->entries[i];
        if (entry.key != NULL) {
            k = (char *) dict_set_entry(new_entries, new_capacity, entry.key, entry.value, NULL);
            if (k == NULL) break;
        }
    }
    if (k == NULL) {
        free(new_entries);
        return 0;
    }

    free(d->entries);
    d->entries = new_entries;
    d->capacity = new_capacity;
    return 1;
}

static int dict_expand(Dict d) {
    size_t new_capacity = get_dict_expanded_size(d);
    if (new_capacity < d->capacity) return 0;
    return dict_relocate(d, new_capacity);
}

static int dict_reduce(Dict d) {
    size_t new_capacity = get_dict_reduced_size(d);
    if (new_capacity > d->capacity) return 0;
    return dict_relocate(d, new_capacity);
}


size_t dict_count_entries(Dict d) {
    return d->length;
}

size_t dict_get_capacity(Dict d) {
    return d->capacity;
}

const char **dict_get_keys(Dict d) {
    if (d == NULL) return NULL;

    DictIterator di = dict_iterator_create(d);
    if (di == NULL) return NULL;

    const char **keys = (const char **) calloc(dict_count_entries(d), sizeof(const char *));

    size_t cursor = 0;
    while (dict_iterator_next(di))
        keys[cursor++] = di->key;

    dict_iterator_destroy(di);

    return keys;
}

void **dict_get_values(Dict d) {
    if (d == NULL) return NULL;

    DictIterator di = dict_iterator_create(d);
    if (di == NULL) return NULL;

    void **values = (void **) calloc(dict_count_entries(d), sizeof(void *));

    size_t cursor = 0;
    while (dict_iterator_next(di))
        values[cursor++] = di->value;

    dict_iterator_destroy(di);

    return values;
}


Dict dict_create(void) {
    Dict d = (Dict) malloc(sizeof(struct Dict_s));
    if (d == NULL)
        return NULL;


    d->length = 0;
    d->capacity = INITIAL_CAPACITY;

    d->entries = (DictEntry *) calloc(d->capacity, sizeof(DictEntry));
    if (d->entries == NULL) {
        free(d);
        return NULL;
    }

    return d;
}

void dict_destroy(Dict d, void (*cleaner)(void *, const char *)) {
    if (d == NULL) return;

    for (size_t i = 0; i < d->capacity; i++) {
        DictEntry e = d->entries[i];
        char *key = e.key;

        if (key == NULL) continue;

        if (cleaner != NULL) cleaner(e.value, key);
        free(key);
    }

    free(d->entries);
    free(d);
}


const char *dict_set(Dict d, const char *key, void *value) {
    if (d == NULL) return NULL;
    if (dict_should_expand(d) && !dict_expand(d)) return NULL;
    return dict_set_entry(d->entries, d->capacity, key, value, &d->length);
}

void *dict_get(Dict d, const char *key) {
    if (d == NULL) return NULL;

    int found;
    size_t index = entries_find_index(d->entries, d->capacity, key, &found);

    if (found) return d->entries[index].value;

    return NULL;
}

void dict_remove(Dict d, const char *key, DictEntryCleaner cleaner) {
    if (d == NULL) return;

    int found;
    size_t index = entries_find_index(d->entries, d->capacity, key, &found);
    if (!found) return;

    DictEntry e = d->entries[index];
    if (cleaner)
        cleaner(e.value, e.key);
    free(e.key);


    d->entries[index] = (DictEntry) {NULL, NULL};
    d->length--;

    size_t capacity = d->capacity;
    index = index + 1 >= capacity ? 0 : index + 1;

    size_t init_index = index;
    while (d->entries[index].key != NULL) {
        size_t new_index = entries_find_index(d->entries, capacity, d->entries[index].key, NULL);
        if (index != new_index && d->entries[new_index].key == NULL) {
            d->entries[new_index] = d->entries[index];
            d->entries[index] = (DictEntry) {NULL, NULL};
        }
        index++;
        if (index == init_index) break;
        if (index >= capacity) index = 0;
    }

    if (dict_should_reduce(d)) dict_reduce(d);
}

DictIterator dict_iterator_create(Dict d) {
    DictIterator di = malloc(sizeof(struct DictIterator_s));
    di->value = NULL;
    di->key = NULL;
    DictIteratorInternals *internals = (DictIteratorInternals *) malloc(sizeof(DictIteratorInternals));
    internals->dict = d;
    internals->index = 0;
    di->_internals = (void *) internals;
    return di;
}

void dict_iterator_destroy(DictIterator di) {
    if (di == NULL) return;

    if (di->_internals != NULL)
        free(di->_internals);

    free(di);
}

int dict_iterator_next(DictIterator di) {
    DictIteratorInternals *dii = (DictIteratorInternals *) (di->_internals);
    Dict d = dii->dict;
    while (dii->index < d->capacity) {
        size_t i = dii->index;
        dii->index++;
        DictEntry e = d->entries[i];

        if (e.key == NULL) continue;

        di->key = e.key;
        di->value = e.value;
        return 1;
    }
    return 0;
}
