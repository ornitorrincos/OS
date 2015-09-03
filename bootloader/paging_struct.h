#ifndef PAGING_STRUCT_H
#define PAGING_STRUCT_H

#include <efi.h>
#include <efilib.h>
#include <stdint.h>

typedef UINT64 uchar; // need to replace this for other more sensible thing or location

UINT64 GetVMCPUID(); // implemented in misc.s

extern uint64_t maxneg;

typedef struct _CPUIDsizes
{
  UINT64 PhysicalAddress:8;
  UINT64 VirtualAddress:8;
  UINT64 Padding:48;
} CPUIDsizes;

// these entries are for 4KB pages


//                                Virtual pointer bits breakdown (4KB pages)
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |     63 - 48   |   47 - 39   |   38 - 30  |  29 - 21  |  20 - 12  |       11 - 0          |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+
//    |   sign extend | PML4 offset | PDP offset | PD offset | PT offset | physical page offset  |
//    +---------------+-------------+------------+-----------+-----------+-----------------------+

//                                Virtual pointer bits breakdown (2MB pages)
//    +---------------+-------------+------------+-----------+-----------------------------------+
//    |     63 - 48   |   47 - 39   |   38 - 30  |  29 - 21  |              20 - 0               |
//    +---------------+-------------+------------+-----------+-----------------------------------+
//    |   sign extend | PML4 offset | PDP offset | PD offset |        physical page offset       |
//    +---------------+-------------+------------+-----------+-----------------------------------+


// paging structure:

// CR3
// the contents of this struct go into CR3
typedef struct __attribute__((packed))
{
  uint64_t reserved1:3;
  uint64_t PWT:1;        // page level writethough
  uint64_t PCD:1;        // page level cache disable
  uint64_t reserved2:7;
  uint64_t base_addr:40; // base address to the table of PML4 entries
  uint64_t reserved3:13;
} s_CR3;

// Virtual Pointer
#define VP_OFF_PHY   0
#define VP_OFF_PT   12
#define VP_OFF_PD   21
#define VP_OFF_PDP  30
#define VP_OFF_PML4 39
#define VP_OFF_SIGN 48

#define GetPhyOffset(x)  (x & 0xFFF)
#define GetPTOffset(x)   ((x >> VP_OFF_PT) & 0x1FF)
#define GetPDOffset(x)   ((x >> VP_OFF_PD) & 0x1FF)
#define GetPDPOffset(x)  ((x >> VP_OFF_PDP) & 0x1FF)
#define GetPML4Offset(x) ((x > VP_OFF_PML4) & 0x1FF)
#define GetSignOffset(x) ((x > VP_OFF_SIGN) & 0xFFFF)


// CR3
#define CR3_PWT (1 << 3)
#define CR3_PCD (1 << 4)
#define CR3_ADDR_SHIFT 12

// Page Entry
#define PE_P   (1 <<  0)
#define PE_RW  (1 <<  1)
#define PE_US  (1 <<  2)
#define PE_PWT (1 <<  3)
#define PE_PCD (1 <<  4)
#define PE_A   (1 <<  5)
#define PE_D   (1 <<  6)
#define PE_PS  (1 <<  7)
#define PE_G   (1 <<  8)
#define PE_BA  (1 << 12)
#define PE_NX  (1 << 63)

#define MaskTable(x)   (x & maxneg)
#define MaskPhyAddr(x) (x & 0xFFFFFFF000)
#define OffsetAddr(x)  (x << PE_BA)


// PML4E

typedef struct __attribute__((packed))
{
  uint64_t P:1; // present bit
  uint64_t RW:1; // read write (if 0 write protected)
  uint64_t US:1; // user/supervisor if 0 no user mode access
  uint64_t PWT:1; // page level write though
  uint64_t PCD:1; // page level cache disable
  uint64_t A:1; // accesed
  uint64_t PS:1; // value must be 1 for 1GB pages
  uint64_t MBZ:2;
  uint64_t AVL:3;
  uint64_t PDPBA:40; // physical address of the table pointed by this entry
  uint64_t available:11; // need to look into what values go here
  uint64_t NX:1; // execute disable
} s_PML4E;



// PDPE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t PS:1; // value must be 1 for 2MB pages
  uint64_t zero:1; // zero
  uint64_t MBZ:1;
  uint64_t AVL:3;
  uint64_t PDBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PDPE;

// PDE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t ignored1:1;
  uint64_t zero:1;
  uint64_t ignored2:1;
  uint64_t AVL:3;
  uint64_t PTBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PDE;

// PTE

typedef struct __attribute__((packed))
{
  uint64_t P:1;
  uint64_t RW:1;
  uint64_t US:1;
  uint64_t PWT:1;
  uint64_t PCD:1;
  uint64_t A:1;
  uint64_t D:1;
  uint64_t PAT:1;
  uint64_t G:1;
  uint64_t AVL:3;
  uint64_t PPBA:40;
  uint64_t available:11;
  uint64_t NX:1;
} s_PTE;

typedef struct __attribute__((packed))
{
  uint64_t offset:12;
  uint64_t PT:9;
  uint64_t PD:9;
  uint64_t PDP:9;
  uint64_t PML4:9;
  uint64_t sign:16;
} s_VPTR;

#endif // PAGING_STRUCT_H

