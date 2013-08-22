#ifndef LLCC_HASHMAP_HEADER
#define LLCC_HASHMAP_HEADER

struct hashmap {
    size_t chains_size;
    struct hashmap_chain *chains;
};

struct hashmap_chain {
    char *key;
    void *value;
    struct hashmap_chain *next;
};

struct hashmap *hashmap_alloc(size_t size);
void hashmap_free(struct hashmap *map);
void hashmap_free_values(struct hashmap *map);

void hashmap_put(struct hashmap *map, char *key, void *value);
void *hashmap_get(struct hashmap *map, char *key);

#endif
