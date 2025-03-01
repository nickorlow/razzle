#pragma once

#include <stdint.h>

#ifdef ARCH_I386
#include "../../arch/i386/scheduler/scheduler.h"
#endif

#define ADDRESS_SPACE_SIZE 1000000
#define MAX_PROCESSES 128

enum task_state_t { TASKSTATE_unused, TASKSTATE_ready, TASKSTATE_blocked };

struct task_struct {
  uint32_t          pid;
  enum task_state_t state;
  struct regs       registers;
  unsigned int      address_base;
  char             *allocd_addr;
  char             *process_name;
};

extern struct task_struct process_table[MAX_PROCESSES];
extern uint32_t           current_pid;

void init_process_table();
void start_process(char *, char *);
void process_debug();
void schedule(struct regs *);
