#ifndef ELF64HEADER_H
#define ELF64HEADER_H

#include "datatypes.h"

#define EI_NIDENT 16

// ELF header
typedef struct {
        unsigned char   e_ident[EI_NIDENT];
        Elf64_Half      e_type;
        Elf64_Half      e_machine;
        Elf64_Word      e_version;
        Elf64_Addr      e_entry;
        Elf64_Off       e_phoff;
        Elf64_Off       e_shoff;
        Elf64_Word      e_flags;
        Elf64_Half      e_ehsize;
        Elf64_Half      e_phentsize;
        Elf64_Half      e_phnum;
        Elf64_Half      e_shentsize;
        Elf64_Half      e_shnum;
        Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

// Section header
typedef struct {
        Elf64_Word	    sh_name;
        Elf64_Word	    sh_type;
        Elf64_Xword	    sh_flags;
        Elf64_Addr	    sh_addr;
        Elf64_Off	      sh_offset;
        Elf64_Xword	    sh_size;
        Elf64_Word	    sh_link;
        Elf64_Word	    sh_info;
        Elf64_Xword	    sh_addralign;
        Elf64_Xword	    sh_entsize;
} Elf64_Shdr;

// indentification struct
enum Elf_Ident {
  EI_MAG0		= 0, // 0x7F
  EI_MAG1		= 1, // 'E'
  EI_MAG2		= 2, // 'L'
  EI_MAG3		= 3, // 'F'
  EI_CLASS	= 4, // Architecture (32/64)
  EI_DATA		= 5, // Byte Order
  EI_VERSION	= 6, // ELF Version
  EI_OSABI	= 7, // OS Specific
  EI_ABIVERSION	= 8, // OS Specific
  EI_PAD		= 9  // Padding
};

// MAgic Numbers
#define ELFMAG0	0x7F
#define ELFMAG1	'E'
#define ELFMAG2	'L'
#define ELFMAG3	'F'

#define ELFCLASS64 2


enum Elf_Type {
  ET_NONE		= 0, // Unkown Type
  ET_REL		= 1, // Relocatable File
  ET_EXEC		= 2  // Executable File
};


// Machine
#define EM_X86_64 62

// version
#define EV_CURRENT	1

// Section header specific values
# define SHN_UNDEF	(0x00) // Undefined/Not present

enum ShT_Types {
  SHT_NULL	= 0,
  SHT_PROGBITS	= 1,
  SHT_SYMTAB	= 2,
  SHT_STRTAB	= 3,
  SHT_RELA	= 4,
  SHT_NOBITS	= 8,
  SHT_REL		= 9,
};

enum ShT_Attributes {
  SHF_WRITE	= 0x01,
  SHF_ALLOC	= 0x02
};



#endif // ELF64HEADER_H

