#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <string>
#include <map>
#include <sys/stat.h>
#include "lab2_cache.h"
#define Page_count 100

static Cache cache = *new Cache;
std::map<int,int> offset;
using namespace std;
void printcache(){
    CacheBlock* block = cache.cache_head;
    while (true){
        cout<<block->number<<" "<<block->data<<" "<<block->fd<<" "<<block->next<<" "<<block->prev<<endl;
        if(block->next!= nullptr){
            break;
        }
        block=block->next;
    }
}
int lab2_open(const char *path) {
    int fd = open(path,    O_RDWR| O_DIRECT);
    if (fd == -1) {
        return -1;
    }
    offset[fd]=0;
    if (cache.cache_head == nullptr) {
        cout<<888<<endl;
        CacheBlock *cacheBlock = new CacheBlock;
        cacheBlock->fd = fd;
        cacheBlock->next = nullptr;
        cacheBlock->data = time(0);
        cacheBlock->prev = nullptr;
        cacheBlock->number = 0;
        cache.cache_head = cacheBlock;
        cache.cache_tail = cacheBlock;
    } else if (cache.cache_tail->number + 1 < cache.cache_size) {
        cout<<999<<endl;
        CacheBlock *cacheBlock = new CacheBlock;
        cacheBlock->fd = fd;
        cacheBlock->next = nullptr;
        cacheBlock->data = time(0);
        cacheBlock->prev = cache.cache_tail;
        cacheBlock->number = cache.cache_tail->number + 1;
        cout<<cacheBlock->page_number<<endl;
        cache.cache_tail->next = cacheBlock;
        cache.cache_tail = cacheBlock;
    } else {
        cout<<777<<endl;
        time_t min = time(0);
        CacheBlock *minblock = cache.cache_head;
        CacheBlock *block = cache.cache_head;
        while (block != nullptr) {
            if (block->data < min) {
                min = block->data;
                minblock = block;
            }
            block = block->next;
        }
        close(minblock->fd);
        minblock->fd = fd;
        minblock->data = time(0);
    }
    //printcache();
    //cout<<endl;
    return fd;
}
void lab2_close(int fd){
    CacheBlock *block = cache.cache_head;
    bool t = false;
    while (block != nullptr) {
        if(block->fd==fd){
            close(fd);
            t = true;
            if(block->next != nullptr) {
                if(block->prev != nullptr) {
                    block->prev->next = block->next;
                } else{
                    cache.cache_head=block->next;
                }
                block->next->prev = block->prev;
            } else{
                if(block->prev!= nullptr) {
                    block->prev->next = nullptr;
                }
                cache.cache_tail=block->prev;
            }
        }
        if (t){
            block->number=block->number-1;
        }
        block = block->next;
    }
}
void del(){
    if(cache.cache_tail->number>cache.cache_size){
        CacheBlock *minblock = cache.cache_head;
        CacheBlock *block = cache.cache_head;
        time_t min = time(0);
        while (block != nullptr) {
            if (block->data < min) {
                min = block->data;
                minblock = block;
            }
            block = block->next;
        }
        if(minblock->next!= nullptr){
            minblock->prev->next = minblock->next;
        } else{
            minblock->prev->next = nullptr;
        }
        if(minblock->prev!= nullptr){
            minblock->next->prev = minblock->prev;
        } else{
            minblock->next->prev = nullptr;
        }
    }
}
void addpage(int fd,CacheBlock* block, int page_num,char* page,int count,int o, bool t){
    lseek(fd, page_num*Page_count, SEEK_SET);
    if(block->page_number== -1) {
        block->page = page;
        block->page_number = page_num;
        if(block->prev!= nullptr){
            block->prev->next=block;
        } else{
            cache.cache_head=block;
        }
        if(block->next!= nullptr){
            block->next->prev=block;
        } else{
            cache.cache_tail=block;
        }
    } else{
        t= false;
        while (true){
            if(t){
                if(block->next!= nullptr) {
                    block->number+1;
                    block = block->next;
                } else{
                    break;
                }
            }else if(block->next!= nullptr && block->next->fd==fd && block->next->page_number<page_num){
                block=block->next;
            } else{
                CacheBlock* block1=block;
                block1->prev=block;
                block->next=block1;
                block1->page=page;
                block1->page_number = page_num;

                if(block->prev!= nullptr){
                    block->prev->next=block;
                }
                if(block1->next!= nullptr){
                    block1->next->prev=block1;
                }
                block=block->next;
                t= true;
            }
        }
    }
}
ssize_t lab2_read(int fd, char *buf, size_t count){
    CacheBlock *block = cache.cache_head;
    //cout<<fd;
    while (block != nullptr) {
        //cout<<block->number<<" "<<block->data<<" "<<block->fd<<" "<<block->page_number<<endl;
        if(block->fd==fd){
            break;
        }
        block=block->next;
    }

    off_t o=offset[fd];
    cout<<o<<"uuu"<<endl;
    int page_num=o/Page_count;
    bool t = false;
    char * page= new char[Page_count];
    for(int i=0;i<Page_count;i++){
        page[i]='_';
    }

    while (true){
        if(block->page_number==page_num){
            t = true;
            break;
        } else{
            if(block->next!= nullptr && block->next->fd==fd && block->next->page_number<page_num){
                block=block->next;
            } else{
                break;
            }
        }
    }

    if(!t) {
        lseek(fd, page_num*Page_count, SEEK_SET);
        ssize_t bytesRead = read(fd, page, Page_count);
        addpage(fd,block,page_num,page,count,o,t);
        for(int i=o-page_num*Page_count;i<o-page_num*Page_count+count;i++){
            buf[i-o+page_num*Page_count]=page[i];
        }
        del();
        return bytesRead;
    } else{
        for(int i=o-page_num*Page_count;i<o-page_num*Page_count+count;i++){
            cout<<i<<" "<<block->page[i]<<" "<<o<<" "<<page_num<<endl;
            buf[i-o+page_num*Page_count]=block->page[i];
        }
        del();
        cout<<static_cast<char *>(buf)<<9999999<<endl;
        return 1;
    }
}
off_t lab2_lseek(int fd, off_t off, int whence){
    struct stat fileInfo;
    if (fstat(fd, &fileInfo) == -1) {
        std::cerr << "Ошибка получения информации о файле!" << std::endl;
        return -1;
    }
    switch (whence) {
        case SEEK_SET:
            offset[fd] = off;
            break;
        case SEEK_CUR:
            offset[fd] += off;
            break;
        case SEEK_END:
            offset[fd] = fileInfo.st_size-off;
            break;
        default:
            throw std::runtime_error("Invalid lseek operation");
    }
    return off;
}

ssize_t lab2_write(int fd, const char *buf, size_t count){
    CacheBlock *block = cache.cache_head;
    while (block != nullptr) {
        //cout<<block->number<<" "<<block->data<<" "<<block->fd<<" "<<block->page_number<<endl;
        if(block->fd==fd){
            break;
        }
        block=block->next;
    }
    off_t o=offset[fd];
    cout<<o<<"uuu"<<endl;
    int page_num=o/Page_count;
    bool t = false;
    while (true){
        if(block->page_number==page_num){
            t = true;
            break;
        } else{
            if(block->next!= nullptr && block->next->fd==fd && block->next->page_number<page_num){
                block=block->next;
            } else{
                break;
            }
        }
    }
    char * page= new char[Page_count];
    for(int i=0;i<Page_count;i++){
        page[i]='_';
    }
    if(t) {
        for (int i = o - page_num * Page_count; i < o - page_num * Page_count + count; i++) {
            block->page[i] = buf[i - o + page_num * Page_count];
        }
    } else{
        lseek(fd, page_num*Page_count, SEEK_SET);
        ssize_t bytesRead = read(fd, page, Page_count);
        addpage(fd,block,page_num,page,count,o,t);
        for (int i = o - page_num * Page_count; i < o - page_num * Page_count + count; i++) {
            block->page[i] = buf[i - o + page_num * Page_count];
        }
        del();
    }
    cout<<static_cast<char *>(block->page);
    return 1;
}
int lab2_fsync(int fd){
    CacheBlock *block = cache.cache_head;
    while (block != nullptr) {
        //cout<<block->number<<" "<<block->data<<" "<<block->fd<<" "<<block->page_number<<endl;
        if(block->fd==fd){
            break;
        }
        block=block->next;
    }
    while (block->fd==fd){
        if(block->page_number!=-1){
            int page_size=Page_count;
            cout<<endl;
            for(int i=0;i<Page_count;i++){
                cout<<block->page[i];
                if(block->page[i]=='_'){
                    page_size=i;
                    break;
                }
            }
            cout<<endl;
            lseek(fd,block->page_number*Page_count,SEEK_SET);
            cout<<page_size;
            write(fd,block->page,page_size);
            block=block->next;
        } else{
            break;
        }
    }
    return 0;
}

int main() {
    cache.cache_head=NULL;
    cache.cache_tail=NULL;
    cache.cache_size=100;

    std::cout<<lab2_open("..\\t.txt");
    std::cout<<lab2_open("..\\t.txt");
    std::cout<<lab2_open("..\\t.txt");

    std::cout<<lab2_open("..\\t.txt");
    std::cout<<lab2_open("..\\t.txt");
    std::cout<<lab2_open("..\\t.txt");

    std::cout<<lab2_open("..\\t.txt");
    //printcache();
    for (int i=3;i<10;i++) {
        char* buf = new char[5];
        cout<<lab2_read(i,buf,5)<<static_cast<char *>(buf)<<endl;
        lab2_lseek(i,0,SEEK_SET);
    }
    for (int i=3;i<10;i++) {
        char* buf = new char[5];
        cout<<lab2_read(i,buf,5)<<static_cast<char *>(buf)<<endl;

    }
    //printcache();

    for(int i=3;i<10;i++){
        const char * buf ="ghjh";
        lab2_lseek(i,10,SEEK_SET);
        cout<<lab2_write(i,buf,4)<<endl;
    }/*
    for (int i=3;i<10;i++) {
        void* buf = malloc(5);
        cout<<lab2_read(i,buf,5)<<endl;
    }*/
    lab2_fsync(3);
    for (int i=3;i<10;++i) {
        lab2_close(i);
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}