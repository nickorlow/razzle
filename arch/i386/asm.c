#pragma once

#include <stdint.h>

struct regs {
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

void __sl_acquire(uint32_t *lock_id) {
  __asm__("retry_lock: lock bts $0,(%0); pause; jc retry_lock" : "+g"(lock_id));
}

void __sl_release(uint32_t *lock_id) {
  __asm__("lock btr $0, (%0)" : "+g"(lock_id));
}

static inline void outb(int port, int val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline unsigned char inb(int port) {
  unsigned char val;
  __asm__ volatile("inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
  return val;
}

static inline void interrupt_disable() { __asm__ volatile("cli"); }

static inline void interrupt_enable() { __asm__ volatile("sti"); }
