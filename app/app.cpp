#include <iostream>
#include <map>
#include "../Cache/lab2_cache.h"

static Cache cache = *new Cache;
std::map<int,int> offset;
using namespace std;
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
    for (int i=3;i<10;i++) {
        char* buf = new char[5];
        cout<<lab2_read(i,buf,5)<<static_cast<char *>(buf)<<endl;
        lab2_lseek(i,0,SEEK_SET);
    }
    for (int i=3;i<10;i++) {
        char* buf = new char[5];
        cout<<lab2_read(i,buf,5)<<static_cast<char *>(buf)<<endl;

    }
/*
    for(int i=3;i<10;i++){
        const char * buf ="ghjh";
        lab2_lseek(i,10,SEEK_SET);
        cout<<lab2_write(i,buf,4)<<endl;
    }*/
    //lab2_fsync(3);
    for (int i=3;i<10;++i) {
        lab2_close(i);
    }
    return 0;
}