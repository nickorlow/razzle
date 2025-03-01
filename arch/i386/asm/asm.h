#pragma once

#include <stdint.h>

struct regs {
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

void __sl_acquire(uint32_t *);
void __sl_release(uint32_t *);

static inline void outb(int port, int val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline unsigned char inb(int port) {
  unsigned char val;
  __asm__ volatile("inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void outl(uint32_t port, uint32_t val) {
  __asm__ volatile("outl %k0, %k1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint32_t inl(uint32_t port) {
  uint32_t val;
  __asm__ volatile("inl %k1, %k0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void outw(uint16_t port, uint16_t val) {
  __asm__ volatile("outw %w0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint16_t inw(uint16_t port) {
  uint32_t val;
  __asm__ volatile("inw %w1, %w0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void interrupt_disable() { __asm__ volatile("cli"); }

static inline void interrupt_enable() { __asm__ volatile("sti"); }
