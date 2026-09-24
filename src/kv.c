#include <kv.h>
#include <string.h>

#define TOMBSTONE 0x1

size_t hash(char *val, int capacity) {
    size_t hash = 0x13371337;

    while(*val){

        hash ^= *val;
        hash = hash << 8;
        hash += *val;

        val++;
    }

    return hash % capacity;
}

// fn kv_put
// params:
//  - db: a pointer to db
//  - key: a pointer to the key value
//  - value: a pointer to the value itself
//  returns: the index of the key, 
//      on error returns -1
//      on not found returns -2

int kv_put(kv_t *db, char *key, char *value) {
    if (!db || !key || !value) return -1;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity -1; i++){
        /*
           Using a Tombstone to denote when an entry is deleted.
           Keeps track of hanging entries due to hash collision
           */

        size_t seek_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[seek_idx];

        // Use void* to cast the int for a pointer comparison
        if (entry->key 
                && entry->key != (void*)TOMBSTONE 
                &&!strcmp(entry->key, key)) {
            // Using strdup to allocate space on the heap for the string
            char *newval = strdup(value);
            if (!newval) return -1;

            entry->value = newval;
            return seek_idx;
        }

        if (!entry->key || entry->key == (void*)TOMBSTONE) {
            char *newval = strdup(value);
            char *newkey = strdup(key);

            if (!newkey || !newval) {
                free(newkey);
                free(newval);
                return -1;
            }

            entry->value = newval;
            entry->key = newkey;

            db->count++;
            return seek_idx;
        }
    }

    // database is full
    return -2;
}

kv_t *kv_init(size_t capacity) {
    if (capacity == 0) return NULL;

    kv_t *table = malloc(sizeof(kv_t));

    if (table == NULL) {
        return NULL;
    }

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(sizeof(kv_entry_t), capacity);

    if (table->entries == NULL) {
        return NULL;
    }

    return table;
}
