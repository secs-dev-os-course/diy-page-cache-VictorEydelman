#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include "../Cache/lab2_cache.h"

using namespace std;
size_t fileSize=0;
void prepeare(size_t size,int fd) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  fileSize=size*512;
  char* buf = new char[fileSize];
  std::uniform_int_distribution<int64_t> dis(0, 255);
  for (size_t i = 0; i < fileSize; ++i) {
      buf[i] = static_cast<char>(dis(gen));
  }
  lab2_lseek(fd,0,SEEK_SET);
  lab2_write(fd,buf,fileSize);
  std::cout<<"File create "<<fileSize<<" KB";
}

static int ReadFileSequential(int fd) {
    const size_t bufferSize = 1024 * 16;
    char* buf=new char[bufferSize];
    for(int i=0;i<bufferSize;i++){
        buf[i]='_';
    }
    auto start = std::chrono::high_resolution_clock::now();
    lab2_read(fd,buf,bufferSize);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    size_t buf_size=fileSize;
    for(int i=0;i<bufferSize;i++){
        if(buf[i]=='_'){
            buf_size=i;
        }
    }
    return ((float)std::min(fileSize,buf_size))/((float)duration.count())/1024;
}

using namespace std;
int main(){
  int read=0;
  std::ofstream outFile("test.txt");
  outFile.close();
  int fd=lab2_open("test.txt");
  if(fd!=-1) {
      for (int i = 1; i < 500; i++) {
          prepeare(i,fd);
          auto result = ReadFileSequential(fd);
          read += result;
          cout << result << " KB/s\n";

      }
      std::cout << "Middle speed: " << read / 1000 << " KB/s\n";
      lab2_close(fd);
  }
  return 0;
}