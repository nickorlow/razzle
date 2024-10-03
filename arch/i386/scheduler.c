#pragma once

#include "asm.c"

enum eflags_t {
  EFLAG_CARRY = 0x0001,
  EFLAG_RES = 0x0002,
  EFLAG_PARITY = 0x0004,
  EFLAG_INTERRUPT = 0x0200
};

void initialize_registers(struct regs *new_regs, char *entrypoint,
                          uint32_t address_base, uint32_t address_space_size) {
  new_regs->eip = (unsigned int)entrypoint;

  new_regs->edi = 0;
  new_regs->esi = 0;
  new_regs->ebx = 0;
  new_regs->edx = 0;
  new_regs->ecx = 0;
  new_regs->eax = 0;

  new_regs->eflags = EFLAG_INTERRUPT;
  new_regs->useresp = (unsigned int)address_base + address_space_size - 1;
  new_regs->ebp = new_regs->useresp;

  new_regs->ds = 35;
  new_regs->es = 35;
  new_regs->fs = 35;
  new_regs->gs = 35;
  new_regs->ss = 27;
}

void store_registers(struct regs *machine_regs, struct regs *current_regs) {
  current_regs->edi = machine_regs->edi;
  current_regs->esi = machine_regs->esi;
  current_regs->ebx = machine_regs->ebx;
  current_regs->edx = machine_regs->edx;
  current_regs->ecx = machine_regs->ecx;
  current_regs->eax = machine_regs->eax;
  current_regs->eip = machine_regs->eip;
  current_regs->eflags = machine_regs->eflags;
  current_regs->useresp = machine_regs->useresp;
  current_regs->ebp = machine_regs->ebp;
}

void switch_context(struct regs *machine_regs, struct regs *new_regs) {
  machine_regs->edi = new_regs->edi;
  machine_regs->esi = new_regs->esi;
  machine_regs->ebx = new_regs->ebx;
  machine_regs->edx = new_regs->edx;
  machine_regs->ecx = new_regs->ecx;
  machine_regs->eax = new_regs->eax;
  machine_regs->eip = new_regs->eip;
  machine_regs->eflags = new_regs->eflags;
  machine_regs->esp = machine_regs->esp;
  machine_regs->useresp = new_regs->useresp;
  machine_regs->ebp = new_regs->ebp;

  machine_regs->ds = 35;
  machine_regs->es = 35;
  machine_regs->fs = 35;
  machine_regs->gs = 35;
  machine_regs->ss = 35;
  machine_regs->cs = 27;
}
