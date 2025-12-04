# Hash Table

本人在学习使用c语言时，发现其哲学为“轻量，面向过程，尽可能贴合硬件，
的可移植的汇编语言”。这种最小化且高效的哲学往往将复杂的数据结构和高级
抽象留给程序员自己实现。本人目前常常需要使用哈希表来解决字符串与
数字之间的键值对的映射关系并想利用其高效的查找效率，并以此强化学习
c语言相关语法，练习其库函数等的编写使用
[参考项目](https://github.com/jamesroutley/write-a-hash-table/)


## 概念

哈希表由“桶”数组组成，每个桶存储一个键值对。“桶”是哈希表的主体，每个元素
都是一个桶，通过将键传递给哈希函数，计算键值对在桶数组中的索引，像在数组
中使用索引那般，找到其对应值。

数组索引算法的时间复杂度为`O(1)`，故哈希表存储和检索数据十分高效

## API
+ 插入(相同键插入时会替换旧值)
`void ht_insert(ht_hash_table* ht, const char* key, const char* value);`

+ 搜索
`char* ht_search(ht_hash_table* ht, const char* key);`

+ 删除
`void ht_delete(ht_hash_table* ht, const char* key);`


