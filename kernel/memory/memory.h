#pragma once

#include <stdint.h>

uint8_t memseq(char *, char *, uint32_t len);
uint8_t streq(char *, char *);
void    memset(char *, char, uint32_t);
void    memcpy(char *, char *, uint32_t);
void    strncpy(char *, char *, uint32_t);
