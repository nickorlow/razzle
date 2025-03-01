#pragma once

#ifdef ARCH_I386
#include "../arch/i386/asm/asm.h"
#endif

static inline void sl_acquire(uint32_t *lock) { __sl_acquire(lock); }

static inline void sl_release(uint32_t *lock) { __sl_release(lock); }
