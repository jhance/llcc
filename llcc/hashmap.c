#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

int hash(char *k, size_t size) {
    int hash = 7;
    size_t len = strlen(k);
    size_t i;
    for(i = 0; i < len; i++) {
        hash = 13 * hash + k[i];
    }

    return hash % size;
}

struct hashmap *hashmap_alloc(size_t size) {
    struct hashmap *h = malloc(sizeof(struct hashmap));
    h->chains_size = size;
    h->chains = malloc(sizeof(struct hashmap_chain) * size);

    return h;
}

void hashmap_free_values(struct hashmap *h) {
    size_t i;
    for(i = 0; i < h->chains_size; i++) {
        struct hashmap_chain *chain = h->chains + i;
        chain = chain->next;

        while(chain != NULL) {
            if(chain->value != NULL) {
                free(chain->value);
                chain->value = NULL;
            }

            chain = chain->next;
        }
    }
}

void hashmap_free(struct hashmap *h) {
    size_t i;
    for(i = 0; i < h->chains_size; i++) {
        struct hashmap_chain *chain = h->chains + i;
        chain = chain->next;
        
        while(chain != NULL) {
            struct hashmap_chain *current_chain = chain;
            chain = chain->next;
            free(current_chain);
        }
    }
    free(h->chains);
    free(h);
}

/* Don't put anything in the first chain item, for simplicity */
void hashmap_put(struct hashmap *h, char *k, void *v) {
    int index = hash(k, h->chains_size);
    struct hashmap_chain *chain = h->chains + index;
    struct hashmap_chain *new_chain;
    while(chain->next != NULL) {
        chain = chain->next;
    }

    new_chain = malloc(sizeof(struct hashmap_chain));
    new_chain->key = k;
    new_chain->value = v;
    new_chain->next = NULL;
    chain->next = new_chain;
}

void *hashmap_get(struct hashmap *h, char *k) {
    int index = hash(k, h->chains_size);
    struct hashmap_chain *chain = h->chains + index;

    while(chain->next != NULL) {
        chain = chain->next;

        if(!strcmp(k, chain->key)) {
            return chain->value;
        }
    }

    return NULL;
}
