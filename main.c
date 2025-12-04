#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int main(void) {
    ht_hash_table* ht = ht_new();

    /* 插入 */
    ht_insert(ht, "name", "alice");
    ht_insert(ht, "city", "beijing");
    ht_insert(ht, "lang", "c");

    /* 查找并打印 */
    char* v;
    v = ht_search(ht, "name");
    printf("name -> %s\n", v ? v : "(null)");

    v = ht_search(ht, "city");
    printf("city -> %s\n", v ? v : "(null)");

    v = ht_search(ht, "missing");
    printf("missing -> %s\n", v ? v : "(null)");

    /* 更新 */
    ht_insert(ht, "name", "bob");
    printf("name -> %s (after update)\n", ht_search(ht, "name"));

    /* 删除并验证 */
    ht_delete(ht, "city");
    printf("city -> %s (after delete)\n", ht_search(ht, "city"));

    /* 大量插入以触发 resize（可选）*/
    for(int i = 0; i < 200; ++i) {
        char k[32], val[32];
        snprintf(k, sizeof(k), "k%d", i);
        snprintf(val, sizeof(val), "v%d", i);
        ht_insert(ht, k, val);
    }
    printf("search k42 -> %s\n", ht_search(ht, "k42"));

    ht_del_hash_table(ht);
    return 0;
}

/*
编译运行（在项目目录）:
gcc -std=c11 -O2 hash_table.c test_hash_table.c -o test_hash_table
./test_hash_table
*/