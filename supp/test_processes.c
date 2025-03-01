/*
 * Since RAZZLE doesn't have disk support yet, all code for test processes are
 * included in this file.
 */

#pragma once

#include "../kernel/io/io.h"
#include "../kernel/memory/memory.h"
#include "../kernel/pci/pci.h"
#include "../kernel/scheduler/scheduler.h"
#include "../kernel/spin_lock.h"
#include "../kernel/syscall/syscall.h"

uint32_t lock2 = 0;

uint32_t counter;

void test_razzle() {
  sl_acquire(&lock2);
  counter = 0;
  sl_release(&lock2);
  for (;;) {
    sl_acquire(&lock2);
    counter++;
    // row = 1;
    // col = 0;
    // printf("%s (pid %d): %u \n", process_table[current_pid].process_name,
    //        current_pid, i);
    sl_release(&lock2);
    yield();
  }
}

void sh_cpuid() {
  char     model[13];
  uint32_t res[3];
  __asm__ volatile(
      "mov $0, %%eax; cpuid; mov %%ebx, %0; mov %%edx, %1; mov %%ecx, %2;"
      : "=r"(res[0]), "=r"(res[1]), "=r"(res[2])::"ebx", "ecx", "edx");
  for (uint32_t i = 0; i < 3; i++) {
    model[(i * 4) + 0] = (res[i] >> 0) & 0xFF;
    model[(i * 4) + 1] = (res[i] >> 8) & 0xFF;
    model[(i * 4) + 2] = (res[i] >> 16) & 0xFF;
    model[(i * 4) + 3] = (res[i] >> 24) & 0xFF;
  }
  model[12] = '\0';
  printf("%s\n", model);
}

void sh_lspci() {
  struct pci_device devlist[7];
  int               devices = lspci(&devlist[0], 7);
  for (int j = 0; j < devices; ++j) {
    printf("%s %s (%d %d)\n", devlist[j].vendor_name, devlist[j].device_name,
           devlist[j].class_code, devlist[j].subclass_code);
    printf("Device ID: %x\n", devlist[j].device_id);
    printf("Vendor ID: %x\n", devlist[j].vendor_id);
    printf("Bar 5    : %x %u\n\n", devlist[j].bar[5], devlist[j].bar[5]);
  }
}

void sh_lsproc() {
  struct task_struct proclist[7];
  int                procs = lsproc(&proclist[0], 7);
  for (int j = 0; j < procs; ++j) {
    switch (proclist[j].state) {
    case TASKSTATE_ready:
      printf("R");
      break;
    case TASKSTATE_blocked:
      printf("B");
      break;
    default:
      printf("?");
      break;
    }
    printf(" %d: %s", proclist[j].pid, proclist[j].process_name);
    printf("\n");
  }
}

void test_dazzle() {
  char prompt[] = "you@razzle ~$ ";
  char command[2048];

  printf("%s", prompt);
  uint32_t i = 0;

  for (;;) {
    char c = getc();
    sl_acquire(&lock2);

    switch (c) {
    case '\b':
      if (i == 0)
        break;
      command[i--] = '\0';
      printf("\b \b");
      break;

    case '\n':
      command[i] = '\0';
      i = 0;

      printf("\n");

      if (streq(command, "lspci")) {
        sh_lspci();
      } else if (streq(command, "cpuid")) {
        sh_cpuid();
      } else if (streq(command, "help")) {
        printf("dazzle, the shell for RAZZLE\n");
        printf("--------------------------------------------------------\n");
        printf("help    : print this menu\n");
        printf("cpuid   : print CPU vendor id string\n");
        printf("lspci   : print list of PCI devices\n");
        printf("lsproc  : print list of running processes\n");
        printf("counter : print counter maintained by background process\n");
        printf("shutdown:  shuts down the VM using VM-specific methods\n");
      } else if (streq(command, "counter")) {
        printf("%d\n", counter);
      } else if (streq(command, "lsproc")) {
        sh_lsproc();
      } else if (streq(command, "shutdown")) {
        powerctl(POWERCTL_shutdown);
        printf("If the computer is still on, you must manually power it off. "
               "ACPI is not yet implemented.\n");
      } else {
        printf("Unknown command\n");
      }

      printf("%s", prompt);
      break;

    default:
      printf("%c", c);
      command[i++] = c;
      break;
    }

    sl_release(&lock2);
    yield();
  }
}
