#include <iostream>
#include <cstdio>
#include <cstdint>

typedef struct __attribute__((packed))
{
  unsigned long page:21;
  unsigned long PD:9;
  unsigned long PDP:9;
  unsigned long PML4:9;
  unsigned long sign:16;
} s_vptr;

union v
{
s_vptr vptr;
unsigned long ptr;
};

int main()
{
  // 0x0FFFFFFFF; below 4GB
  //unsigned long vaddr = 0x40000000; // 1GB // 0x100000000; // 4GB

  unsigned long vaddr = 0x280000000; // 10GB
  
  uint64_t kr = 0xffffffff7fffffff; // -2GB mark

  //v * p = (v*)&vaddr;
  v * p = (v*)&kr;

  std::cout << "sign " << p->vptr.sign << std::endl;
  std::cout << "PML4 " << p->vptr.PML4 << std::endl;
  std::cout << "PDP " << p->vptr.PDP << std::endl;
  std::cout << "PD " << p->vptr.PD << std::endl;
  std::cout << "Page " << p->vptr.page << std::endl;  

  
  printf("%p\n", p->ptr);
  //std::cout << "Pointer " << p->ptr << std::endl;
  
  v p2;
  p2.ptr = 0;
  p2.vptr.PML4 = 1;
  
  printf("p2: %p\n", p2.ptr);
  
  return 0;
}

