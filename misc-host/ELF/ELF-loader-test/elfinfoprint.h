#ifndef ELFINFOPRINT_H
#define ELFINFOPRINT_H


#include "elf64header.h"

void ElfInfoPrint(Elf64_Ehdr * header);

Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr);
Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, int idx);
char *elf_str_table(Elf64_Ehdr *hdr);
#endif // ELFINFOPRINT_H
