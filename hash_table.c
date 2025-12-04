#include "hash_table.h"
#include <bits/types/siginfo_t.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <strings.h>
#include <stdio.h>

static ht_item HT_DELETE_ITEM = {NULL, NULL};

static void ht_resize_up(ht_hash_table* ht);
static void ht_resize_down(ht_hash_table* ht);
static void ht_del_item(ht_item* i);
static size_t ht_get_hash(const char* s, const size_t num_buckets, const size_t attempt); 
static void* xmalloc(size_t size);
static void* xcalloc(size_t nmemb, size_t size);

static char* str_dup(const char* s) {
    if(s == NULL)
        return NULL;
    size_t n = strlen(s) + 1;
    char* dst = xmalloc(n);
    memcpy(dst,s,n);
    return dst;
}

//xmalloc函数
void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if(ptr == NULL){
        fprintf(stderr, "Error: Memory allocation failed (size: %zu bytes)\n",size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

//xcalloc 函数
void * xcalloc(size_t nmemb, size_t size) {
    void* ptr = calloc(nmemb, size);
    if(ptr == NULL){
        fprintf(stderr, "xcalloc: Memory allocation failed:(nmemb=%zu, size=%zu)\n", nmemb, size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

//初始化ht_item函数
static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = xmalloc(sizeof(ht_item)); //分配与ht_item大小相同内存块
    i->key = str_dup(k); //调用malloc分配sizeof(k)+1后调用strcpy复制k到新内存中
    i->value = str_dup(v);
    return i;
}

static ht_hash_table* ht_new_sized(const size_t base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(base_size);

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

ht_hash_table* ht_new(void) {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const size_t base_size) {
    if(base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }

    ht_hash_table* new_ht = ht_new_sized(base_size);

    for(size_t i = 0; i < ht->size; ++i){
        ht_item* item = ht->items[i];
        if(item == NULL || item == &HT_DELETE_ITEM)
            continue;

        ht_item* new_item = ht_new_item(item->key, item->value);
        size_t index = ht_get_hash(new_item->key, new_ht->size,0);
        size_t j = 1;
        
        while(new_ht->items[index] != NULL) {
            index = ht_get_hash(new_item->key,new_ht->size,j);
            ++j;
        }
        new_ht->items[index] = new_item;
        new_ht->count++;
    }

    for(size_t i = 0; i < ht->size; ++i) {
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETE_ITEM)
            ht_del_item(item);
    }
    free(ht->items);

    ht->items = new_ht->items;
    ht->size = new_ht->size;
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;
    free(new_ht);
}

static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

//ht_hash_table删除函数
void ht_del_hash_table(ht_hash_table* ht) {
    for(size_t i = 0; i < ht->size; ++i) {
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETE_ITEM){
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

//哈希函数
static size_t ht_hash(const char* s, const size_t a, const size_t m) {
    if(m <= 0) return 0;
    unsigned long hash = 0;
    const unsigned char* p = (const unsigned char*)s;
    while(*p){
        hash = (hash * (unsigned long long)a + (unsigned long long)(*p)) % (unsigned long long)m;
        ++p;
    }
    return (size_t)hash;
}

static size_t ht_get_hash(const char* s, const size_t num_buckets, const size_t attempt) {
    if(num_buckets <= 1) return 0;
    size_t hash_a = ht_hash(s,(unsigned long)HT_PRIME_1,num_buckets);
    size_t hash_b = ht_hash(s,(unsigned long)HT_PRIME_2,num_buckets - 1);

    size_t step = hash_b + 1;
    unsigned long long combined = (unsigned long long)hash_a + (unsigned long long)attempt * (unsigned long long)step;
    unsigned long long idx = combined % (unsigned long long)num_buckets;
    return (size_t)idx;
}

//API
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    const int load = ht->count * 100 / ht->size;
    if(load > 70)
        ht_resize_up(ht);
    ht_item* item = ht_new_item(key,value);
    size_t index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    size_t i = 1;
    while(cur_item != NULL && cur_item != &HT_DELETE_ITEM) {
        if(strcmp(cur_item->key, key) == 0){
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        ++i;
    }
    ht->items[index] = item;
    ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key) {
    size_t index = ht_get_hash(key,ht->size, 0);
    ht_item* item = ht->items[index];
    size_t i = 1;
    while(item != NULL) {
        if(item != &HT_DELETE_ITEM) {
            if(strcmp(item->key,key) == 0) {
                return item->value;
            }
        }
        index = ht_get_hash(key,ht->size,i);
        item = ht->items[index];
        ++i;
    }
    return NULL;
}


void ht_delete(ht_hash_table* ht, const char* key) {
    const int load = (int)(ht->count * 100ULL) / ht->size;
    if(load < 10)
        ht_resize_down(ht);
    size_t index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    size_t i = 1;
    while(item != NULL) {
        if(item != &HT_DELETE_ITEM) {
            if(strcmp(item->key, key) == 0){
                ht_del_item(item);
                ht->items[index] = &HT_DELETE_ITEM;
                ht->count--;
                return;
            }
        }
        index = ht_get_hash(key,ht->size,i);
        item = ht->items[index];
        ++i;
    }
}



/* 
 * return if x is prime
 *  returns:
 *  1 : prime
 *  0 : not prime
 *  -1: undefined(etc: x < 2)
 * */
int is_prime(const size_t x) {
    if(x < 2)
        return -1;
    if(x < 4)
        return 1;
    if((x % 2) == 0)
        return 0;
    for(int i = 3; i <= floor(sqrt((double)x)); i += 2){
        if((x % i) == 0)    
            return 0;
    }
    return 1;
}

/*
 * return next prime after x or x(if x is prime)
 * */
size_t next_prime(size_t x) {
    while(is_prime(x) != 1)
        ++x;
    return x;
}

static void ht_resize_up(ht_hash_table* ht) {
    const size_t new_size = ht->base_size * 2;
    ht_resize(ht,new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
    const size_t new_size = ht->base_size / 2;
    ht_resize(ht,new_size);
}
