#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include "9cc.h"

// Vector
Vector *vector_init(size_t nmemb) {
    Vector *v = calloc(1, sizeof(Vector));
    v->nmemb = nmemb;
    size_t nalloc = 4;
    while (nalloc < nmemb) nalloc <<= 1;
    v->nalloc = nalloc;
    v->data = calloc(nalloc, sizeof(void *));
    return v;
}

void *vector_at(Vector *v, size_t index) {
    return v->data[index];
}

void *vector_front(Vector* v) {
    return v->data[0];
}

void *vector_back(Vector* v) {
    return v->data[v->nmemb-1];
}

bool vector_empty(Vector* v) {
    return v->nmemb == 0;
}

size_t vector_size(Vector *v) {
    return v->nmemb;
}

void _vector_realloc(Vector *v, size_t n) {
    void **data = calloc(n, sizeof(void*));
    assert(v->nmemb <= n);
    memcpy(data, v->data, (v->nmemb)*sizeof(void *));
    free(v->data);
    v->data = data;
    v->nalloc = n;
}

void vector_push_back(Vector *v, void *val) {
    if (v->nmemb >= v->nalloc) {
        _vector_realloc(v, (v->nalloc)<<1);
    }
    v->data[v->nmemb] = val;
    ++(v->nmemb);
}

void vector_pop_back(Vector *v) {
    void *b = vector_back(v);
    free(b);
    b = NULL;
    --(v->nmemb);
    if (v->nalloc > 4 && v->nmemb < (v->nalloc)>>1) {
        _vector_realloc(v, (v->nalloc)>>1);
    }
}

void vector_free(Vector *v) {
    for (int i = 0; i < v->nmemb; ++i) {
        void *p = vector_at(v, i);
        free(p);
    }
    free(v->data);
    free(v);
    v = NULL;
}

// Map
Map *map_init() {
    Map *map = calloc(1, sizeof(Map));
    map->size = 4;
    map->nentry = 0;
    map->root = calloc(map->size, sizeof(EntryList *));
    return map;
}

int _map_hash(const char *key, int maxval) {
    long h = 0;
    const long b = 137;
    for (int i = 0; key[i]; ++i) {
        h = h * b + key[i];
        h %= maxval;
    }
    return (int)h;
}

void _map_insert_list(EntryList **root, const char *key, void *val, int hash) {
    EntryList *head = calloc(1, sizeof(EntryList));
    head->key = key;
    head->val = val;
    head->next = root[hash];
    root[hash] = head;
}

void _map_rehash(Map *map, int size) {
    EntryList **root = calloc(size, sizeof(EntryList *));
    for (int i = 0; i < map->size; ++i) {
        EntryList *head = map->root[i];
        while (head) {
            int h = _map_hash(head->key, size);
            _map_insert_list(root, head->key, head->val, h);
            EntryList *next = head->next;
            free(head);
            head = next;
        }
    }
    free(map->root);
    map->root = root;
    map->size = size;
}

bool map_empty(const Map *map) {
    return map->nentry == 0;
}

size_t map_size(const Map *map) {
    return map->nentry;
}

void *map_find(const Map *map, const char *key) {
    int h = _map_hash(key, map->size);
    EntryList *head = map->root[h];
    while (head) {
        if ((strcmp(key, head->key) == 0)) {
            return head->val;
        }
        head = head->next;
    }
    return NULL;
}

void map_insert(Map *map, const char *key, void *val) {
    if ((map->size)>>1 < map->nentry) {
        _map_rehash(map, (map->size)<<1);
    }

    void *p = map_find(map, key);
    if (p) {
        free(p);
        p = val;
    } else {
        int h = _map_hash(key, map->size);
        _map_insert_list(map->root, key, val, h);
        ++(map->nentry);
    }
}

void map_remove(Map *map, const char *key) {
    if ((map->size > 4) && map->nentry < (map->size)>>2)  {
        _map_rehash(map, (map->size)>>1);
    }

    int h = _map_hash(key, map->size);
    EntryList *head = map->root[h];
    EntryList *prev = NULL;
    while (head) {
        if ((strcmp(key, head->key) == 0)) {
            if(prev){
                prev->next = head->next;
            } else {
                map->root[h] = head->next;
            }
            free(head);
            --(map->nentry);
            return;
        }
        prev = head;
        head = head->next;
    }
}

// test
void test_vector(int n, int seed) {
    Vector *v = vector_init(0);

    int **a = calloc(n, sizeof(int*));
    srand(seed);

    for (int i = 0; i < n; ++i) {
        a[i] = malloc(sizeof(int));
        *a[i] = rand();
    }

    long start, end;
    start = clock();
    for (int i = 0; i < n; ++i) {
        vector_push_back(v, a[i]);
    }
    end = clock();
    assert(vector_size(v) == n);
    printf("push time : %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < n; ++i) {
        assert(a[i] == vector_at(v, i));
    }

    start = clock();
    for (int i = 0; i < n; ++i) {
        vector_pop_back(v);
    }
    end = clock();
    printf("pop time : %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    vector_free(v);

    printf("ok\n");
}

void test_gen_randtxt(int length, char *str) {
    const char *charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWxYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; ++i) {
        str[i] = charset[rand() % sizeof(charset)];
    }
    str[length] = '\0';
}


void test_map(int n, int seed) {
    srand(seed);
    const size_t length = 16;
    char **randtxts = calloc(n, sizeof(char *));
    int **arr = calloc(n, sizeof(int *));
    for (int i = 0; i < n; ++i) {
        randtxts[i] = calloc(length, sizeof(char));
        test_gen_randtxt(length, randtxts[i]);
        arr[i] = calloc(1, sizeof(int));
        *arr[i] = rand();
    }

    Map *map = map_init();

    long start, end;
    start = clock();
    for (int i = 0; i < n; ++i){
        map_insert(map, randtxts[i], arr[i]);
    }
    end = clock();
    assert(map_size(map) == n);
    printf("insert time : %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < n; ++i){
        void *p = map_find(map, randtxts[i]);
        assert(*(int *)p == *arr[i]);
    }
    end = clock();
    printf("find time : %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < n; ++i){
        map_remove(map, randtxts[i]);
    }
    end = clock();
    assert(map_empty(map));
    printf("remove time : %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("ok\n");
    return;
}

