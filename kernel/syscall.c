#pragma once

#include "scheduler.c"
#include "ps2.c"
#include "pci.c"
#include "vga.c"

#ifdef ARCH_I386
#include "../arch/i386/syscall.c"
#endif

enum syscall_type_d { SYSCALL_yield, SYSCALL_getc, SYSCALL_exit, SYSCALL_lspci, SYSCALL_lsproc };

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
uint32_t lspci(struct pci_device* devlist, uint32_t len) {
    int result = 0;
    __asm__ volatile (
        "movl %0, %%ebx"  
        : : "r"((uint32_t) devlist)  : "%ebx"
    );

    __asm__ volatile (
        "movl %0, %%ecx"  
        : : "r"(len)  : "%ecx"
    );

    
    INVOKE_SYSCALL(SYSCALL_lspci);

    __asm__ volatile (
        "movl %%eax, %k0"  
        : "=r"(result)  
    );


    return result;
}

uint32_t lsproc(struct task_struct* proclist, uint32_t len) {
    int result = 0;
    __asm__ volatile (
        "movl %0, %%ebx"  
        : : "r"((uint32_t) proclist)  : "%ebx"
    );

    __asm__ volatile (
        "movl %0, %%ecx"  
        : : "r"(len)  : "%ecx"
    );

    
    INVOKE_SYSCALL(SYSCALL_lsproc);

    __asm__ volatile (
        "movl %%eax, %k0"  
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

  case SYSCALL_lspci: {
    uint32_t len = r->ecx;
    len = len < pci_devices_count ? len : pci_devices_count;
    struct pci_device* devlist = (struct pci_device*) r->ebx;

    for (uint32_t i = 0; i < len;  ++i) {
        devlist[i] = pci_devices_list[i];
    }

    r->eax = len;

    break;
  };
  
  case SYSCALL_lsproc: {
    uint32_t len = r->ecx;
    struct task_struct*proclist = (struct task_struct*) r->ebx;

    uint32_t i = 0, j = 0;
    for (; i < len;  ++i, ++j) {
        for(; j < MAX_PROCESSES && process_table[j].state == TASKSTATE_unused; ++j) {}
        if (j == MAX_PROCESSES) break;

        proclist[i] = process_table[j];
    }

    r->eax = i;

    break;
  };

  case SYSCALL_exit: {
    process_table[current_pid].state = TASKSTATE_unused;
    __syscall_yield(r);
    break;
  };
  }
}
