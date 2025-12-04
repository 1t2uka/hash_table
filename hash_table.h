#ifndef HASH_TABLE_H__
#define HASH_TABLE_H__

#include <stdio.h>

#define HT_PRIME_1 151
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 50

//键值对
typedef struct {
    char* key;
    char* value;
} ht_item;

//哈希表
typedef struct {
    size_t base_size;
    size_t size;   //桶的大小
    size_t count;  //键值对数量
    ht_item** items;    //一个指向ht_item*的指针数组，即哈希表的桶集合，为链地址法解决冲突
} ht_hash_table;


ht_hash_table* ht_new(void);
void ht_del_hash_table(ht_hash_table* ht);

//API
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);


//素数有关函数
int is_prime(const size_t x);
size_t next_prime(size_t x);

#endif
