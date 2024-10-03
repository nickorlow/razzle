#pragma once

#include "scheduler.c"
#include "ps2.c"

#ifdef ARCH_I386
#include "../arch/i386/syscall.c"
#endif

enum syscall_type_d { SYSCALL_yield, SYSCALL_getc, SYSCALL_exit };

void yield() { INVOKE_SYSCALL(SYSCALL_yield); }
char getc() { 
    INVOKE_SYSCALL(SYSCALL_getc); 

    char result;
    __asm__ volatile (
        "movb %%al, %0"  
        : "=r"(result)  
    );

    return result;
}

void __syscall_yield(struct regs *r) {
  // for now, just reschedule
  // TODO: reset PIT timer so
  // next process doesn't get an
  // unreasonably small quantum

  schedule(r);
}

void syscall(struct regs *r) {
  int syscall_no = r->eax;

  switch (syscall_no) {
  case SYSCALL_yield: {
    __syscall_yield(r);
    break;
  };

  case SYSCALL_getc: {
    process_table[current_pid].state = TASKSTATE_blocked;

    for(uint32_t i = 0; i < MAX_PROCESSES; i++) {
        if(!ps2_waiters[i].alloc) {
            ps2_waiters[i].alloc = 1;
            ps2_waiters[i].pid = current_pid;
            break;
        }
    }

    __syscall_yield(r);
    break;
  };

  case SYSCALL_exit: {
    process_table[current_pid].state = TASKSTATE_unused;
    __syscall_yield(r);
    break;
  };
  }
}
