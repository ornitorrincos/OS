#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

uint64_t GetNextEntry();
void initCR3();
void printCR3();
void writeCR3();
void SetVirtualAddress(uint64_t phy, uint64_t virt);
uint8_t checkIdentity(uint64_t phy);


//void * SetPagingStructs();

#endif // PAGING_H

