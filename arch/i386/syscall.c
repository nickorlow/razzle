#pragma once

#define INVOKE_SYSCALL(syscall_num)                                            \
  __asm__ volatile("movl %0, %%eax;"                                           \
                   "int $0x80;"                                                \
                   :                                                           \
                   : "r"(syscall_num)                                          \
                   : "%eax")
