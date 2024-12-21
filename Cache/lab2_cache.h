#ifndef LAB2_CACHE_H
#define LAB2_CACHE_H

#include <stddef.h>
#include <sys/types.h>


typedef struct CacheBlock {
    int fd;
    int number;
    time_t data;
    char *page;
    int page_number=-1;
    struct CacheBlock *prev; // Указатель на предыдущий блок (для LRU)
    struct CacheBlock *next; // Указатель на следующий блок (для LRU)
} CacheBlock;

typedef struct {
    CacheBlock *cache_head; // Голова списка кэша
    CacheBlock *cache_tail; // Хвост списка кэша
    int cache_size;      // Количество блоков в кэше
} Cache;

#endif // LAB2_CACHE_H