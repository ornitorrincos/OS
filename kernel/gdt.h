#pragma once

#include <stdint.h>

int init_gdt();
void create_descriptor(int type, uint32_t base, uint32_t limit, uint8_t access_byte, uint16_t flag);
void create_system_segment_descriptor();
