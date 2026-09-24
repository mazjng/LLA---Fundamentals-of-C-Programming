#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *table = kv_init(1024);
    printf("%p\n", table);
    printf("%ld\n", table->capacity);

    kv_put(table, "hehe", "haha");
    kv_put(table, "hehe", "hoho");
    kv_put(table, "lala", "haha");

    char *val = kv_get(table, "hehe");
    char *val2 = kv_get(table, "lala");
    char *val3 = kv_get(table, "dosent exist");

    printf("%s | %s | %s |", val, val2, val3);
}
