#pragma once

#ifdef ARCH_I386
#include "../arch/i386/scheduler.c"
#endif

#include "./memory.c"
#include "./vga.c"
#include <stdint.h>

#define ADDRESS_SPACE_SIZE 1000000
#define MAX_PROCESSES 128

enum task_state_t { TASKSTATE_unused, TASKSTATE_ready, TASKSTATE_blocked };

struct task_struct {
  enum task_state_t state;
  struct regs registers;
  unsigned int address_base;
  char *allocd_addr;
  char *process_name;
};

static struct task_struct process_table[MAX_PROCESSES];
static uint32_t current_pid;

void init_process_table() {
  for (uint32_t i = 0; i < MAX_PROCESSES; i++) {
    process_table[i].state = TASKSTATE_unused;
  }
  current_pid = 129;
}

uint32_t get_next_pid(uint32_t start, enum task_state_t state) {
  uint32_t i = start % MAX_PROCESSES;
  for (; process_table[i].state != state; i = (i + 1) % MAX_PROCESSES) {
  }
  return i;
}

char *balloc_pid(uint32_t size, uint32_t pid) {
  char *addr = process_table[pid].allocd_addr;
  process_table[pid].allocd_addr += size;
  return addr;
}

char *balloc(uint32_t size) { return balloc_pid(size, current_pid); }

void start_process(char *name, char *entrypoint) {
  uint32_t pid = get_next_pid(0, TASKSTATE_unused);

  process_table[pid].address_base =
      (unsigned int)(ADDRESS_SPACE_SIZE * (pid + 6));

  // TODO: Copy code into process' address space
  // memcpy(entrypoint, (char*) process_table[pid].address_base, 500000);
  // entrypoint = (char*) process_table[pid].address_base;

  process_table[pid].allocd_addr = (char *)process_table[pid].address_base;
  process_table[pid].state = TASKSTATE_ready;
  process_table[pid].process_name = balloc_pid(16, pid);
  strncpy(name, process_table[pid].process_name, 16);

  initialize_registers(&process_table[pid].registers, entrypoint,
                       process_table[pid].address_base, ADDRESS_SPACE_SIZE);
}

void single_process_debug(uint32_t pid) {
  struct task_struct proc = process_table[pid];

  printf_nolock("pid: %u (%s)\n", pid, proc.process_name);
  printf_nolock("eip: %x esp: %x (%d)\n", proc.registers.eip,
                proc.registers.useresp, proc.registers.useresp);
  printf_nolock("ass: %d ase: %d\n\n", proc.address_base, proc.registers.ebp);
}

void process_debug() {
  printf_nolock("RAZZLE Debugger: Process Table | current pid: %u\n",
                current_pid);
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (process_table[i].state != TASKSTATE_unused)
      single_process_debug(i);
  }
}

void schedule(struct regs *r) {
  if (current_pid < MAX_PROCESSES) {
    store_registers(r, &process_table[current_pid].registers);
  }

  current_pid =
      get_next_pid((current_pid + 1) % MAX_PROCESSES, TASKSTATE_ready);

  switch_context(r, &process_table[current_pid].registers);
}
