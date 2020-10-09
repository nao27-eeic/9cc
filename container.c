#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include "9cc.h"

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
