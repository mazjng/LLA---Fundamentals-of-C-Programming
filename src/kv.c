#include <kv.h>
#include <string.h>


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

// fn kv_free
// params:
//  - db: a pointer to db
//  returns: 0 on success, -1 on error
int kv_free(kv_t *db) {
    if (!db) return -1;

    for (int i = 0; i < db->capacity -1; i++) {
        kv_entry_t *entry = &db->entries[i];

        if (entry->key && entry->key != TOMBSTONE) {
            free(entry->key);
            free(entry->value);

            entry->key = NULL;
            entry->value = NULL;

            db->count--;
        }
    }

    free(db->entries);
    free(db);

    return 0;
}

// fn kv_delete
// params:
//  - db: a pointer to db
//  - key: a pointer to key to delete
//  returns: 0 on success, -1 on error
int kv_delete(kv_t *db, char* key) {
    if (!db || !key) return -1;

    size_t idx = hash(key, db->capacity);

    for(int i = 0; i < db->capacity -1; i++) {
        
        size_t seek_idx = (idx + i) % db->capacity;
        kv_entry_t *entry = &db->entries[seek_idx];

        if (entry->key == NULL) {
            return -1;
        }

        if (entry->key
            && entry->key != TOMBSTONE
            && !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);

            db->count--;

            entry->key = TOMBSTONE;
            entry->value = NULL;

            return 0;
        }
    }

    return -1;
}

// fn kv_get
// params:
//  - db: a pointer to db
//  - key: a pointer to the key value
//  returns: pointer to the key, 
//      NULL if not found
char *kv_get(kv_t *db, char *key) {

    if (!db || !key) return NULL;

    size_t idx = hash(key, db->capacity);

    for (int i = 0; i < db->capacity -1; i++){
        
        size_t seek_idx = (idx + i) % db->capacity;
        kv_entry_t *entry = &db->entries[seek_idx];
        
        // no key
        if (entry->key == NULL) {
            return NULL;
        }
        
        // keys match
        if (entry->key 
            && entry->key != TOMBSTONE
            && !strcmp(entry->key, key)) {
            return entry->value;
        }

        // keep seeking on TOMBSTONE
    }

    return NULL;
}

// fn kv_put
// params:
//  - db: a pointer to db
//  - key: a pointer to the key value
//  - value: a pointer to the value itself
//  returns: on success returns 0, 
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
        
        // found the slot, occupied and the key matches
        // Use void* to cast the int for a pointer comparison
        // REMEBER: strcmp returns 0 for equal strings
        if (entry->key 
                && entry->key != TOMBSTONE 
                &&!strcmp(entry->key, key)) {
            // Using strdup to allocate space on the heap for the string
            char *newval = strdup(value);
            if (!newval) return -1;
            
            free(entry->value);
            entry->value = newval;
            return 0;
        }
        
        // found the slot, its empty of tombstone
        if (!entry->key || entry->key == TOMBSTONE) {
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
            return 0;
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
