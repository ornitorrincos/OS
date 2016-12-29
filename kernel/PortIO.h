//
// Created by ornitorrincos on 12/29/16.
//

#ifndef KERNEL_PORTIO_H
#define KERNEL_PORTIO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
  asm volatile ( "outb %0, %1"
  :
  : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port)
{
  uint8_t ret;
  asm volatile ( "inb %1, %0"
  : "=a"(ret)
  : "Nd"(port) );
  return ret;
}

#endif //KERNEL_PORTIO_H
