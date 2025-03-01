#pragma once

#include "../asm/asm.h"
#include <stdint.h>

enum eflags_t {
  EFLAG_CARRY = 0x0001,
  EFLAG_RES = 0x0002,
  EFLAG_PARITY = 0x0004,
  EFLAG_INTERRUPT = 0x0200
};

void initialize_registers(struct regs *, char *, uint32_t, uint32_t);
void store_registers(struct regs *, struct regs *);
void switch_context(struct regs *, struct regs *);
