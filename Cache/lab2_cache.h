#ifndef LAB2_CACHE_H
#define LAB2_CACHE_H
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <map>
#include <sys/stat.h>
#include <stddef.h>
#include <sys/types.h>


extern "C" {

typedef struct CacheBlock {
    int fd;
    int number;
    int page_number;
    time_t data;
    time_t last_save;
    char* page;
    struct CacheBlock *prev; // Указатель на предыдущий блок (для LRU)
    struct CacheBlock *next; // Указатель на следующий блок (для LRU)
} CacheBlock;

typedef struct {
    CacheBlock *cache_head; // Голова списка кэша
    CacheBlock *cache_tail; // Хвост списка кэша
    int cache_size;      // Количество блоков в кэше
} Cache;
    int lab2_open(const char *path);
    ssize_t lab2_write(int fd, const char *buf, size_t count);
    int lab2_fsync(int fd);
off_t lab2_lseek(int fd, off_t off, int whence);
ssize_t lab2_read(int fd, char *buf, size_t count);
void lab2_close(int fd);
void addpage(int fd, CacheBlock *block, int page_num, char *page,bool t);
void del();
}
#endif // LAB2_CACHE_H