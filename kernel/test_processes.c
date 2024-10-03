/*
 * Since RAZZLE doesn't have disk support yet, all code for test processes are 
 * included in this file.
 */

#pragma once

#include "./scheduler.c"
#include "./spin_lock.c"
#include "./syscall.c"
#include "./vga.c"

uint32_t lock2 = 0;

void test_razzle() {
  for (uint32_t i = 0;; i++) {
    sl_acquire(&lock2);
    row = 1;
    col = 0;
    printf("%s (pid %d): %u \n", process_table[current_pid].process_name,
           current_pid, i);
    sl_release(&lock2);
    yield();
  }
}

void cpuid() {
    char model[13];
    uint32_t res[3];
    __asm__ volatile ("mov $0, %%eax; cpuid; mov %%ebx, %0; mov %%edx, %1; mov %%ecx, %2;" : "=r" (res[0]), "=r" (res[1]), "=r" (res[2]) ::"ebx", "ecx", "edx");
    for(uint32_t i = 0; i < 3; i++) {
        model[(i*4) + 0] = (res[i] >> 0) & 0xFF;
        model[(i*4) + 1] = (res[i] >> 8) & 0xFF;
        model[(i*4) + 2] = (res[i] >> 16) & 0xFF;
        model[(i*4) + 3] = (res[i] >> 24) & 0xFF;
    }
    model[12] = '\0';
    printf("%s", model);
}

void test_dazzle() {
  char prompt[] = "you@razzle ~$";
  char command[2048];

  uint32_t cr = 2;
  printf("%s", prompt);
  for (uint32_t i = sizeof(prompt);;) {
    char c = getc();

    sl_acquire(&lock2);

    row = cr;
    col = i;
    
    switch(c) {
        case '\b':
            if (i == 0) {
                i = 79;
                cr--;
            } else {
                i--;
            }
            col = i;
            row = cr;
            printf(" ");
            break;
        
        case '\n':
            i = 0;
            cr++;
            row++;
            col = 0;
            cpuid();
            cr++;
            col = i;
            row = cr;
            printf("%s", prompt);
            i = sizeof(prompt);
            break;

        default: 
            printf("%c", c);
            command[((cr-2)*80) + i] = c;
            i++;
            break;
    }

    if (i >= 80) {
        i = 0;
        cr++;
    }

    sl_release(&lock2);
    yield();
  }
}
