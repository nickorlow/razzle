#pragma once

#include "./scheduler.c"
#include "./vga.c"
#include <stdint.h>

enum key_code_t {
  backtick_tilde = 1,
  one_bang = 2,
  two_at = 3,
  three_crunch = 4,
  four_dollar = 5,
  five_percent = 6,
  six_carat = 7,
  seven_amp = 8,
  eight_star = 9,
  nine_lparen = 10,
  zero_rparen = 11,
  minus_underscore = 12,
  equal_plus = 13,
  backspace = 14,
  tab = 15,
  lbracket_lbrace = 26,
  rbracket_rbrace = 27,
  backslash_pipe = 43,
  caps_lock = 58,
  semicolon_colon = 39,
  quote_dquote = 40,
  a = 30,
  b = 48,
  c = 46,
  d = 32,
  e = 18,
  f = 33,
  g = 34,
  h = 35,
  i = 23,
  j = 36,
  k = 37,
  l = 38,
  m = 50,
  n = 49,
  o = 24,
  p = 25,
  q = 16,
  r = 19,
  s = 31,
  t = 20,
  u = 22,
  v = 47,
  w = 17,
  x = 45,
  y = 21,
  z = 44,
  space = 57,
  l_shift = 42,
  r_shift = 54,
  period_rarr = 52,
  enter = 28,
  comma_larr = 51,
  slash_question = 53
};

struct key_event_t {
  enum key_code_t key;
  uint8_t is_down;
};

struct kb_state {
  uint8_t l_shift;
  uint8_t r_shift;
  uint8_t l_ctrl;
  uint8_t r_ctrl;
  uint8_t caps;
};

static struct kb_state kstate;

uint8_t is_shifted() { return kstate.caps | kstate.r_shift | kstate.l_shift; }

char keycode_to_char(enum key_code_t key_code) {
  switch (key_code) {
  case a:
    return is_shifted() ? 'A' : 'a';
    break;
  case b:
    return is_shifted() ? 'B' : 'b';
    break;
  case c:
    return is_shifted() ? 'C' : 'c';
    break;
  case d:
    return is_shifted() ? 'D' : 'd';
    break;
  case e:
    return is_shifted() ? 'E' : 'e';
    break;
  case f:
    return is_shifted() ? 'F' : 'f';
    break;
  case g:
    return is_shifted() ? 'G' : 'g';
    break;
  case h:
    return is_shifted() ? 'H' : 'h';
    break;
  case i:
    return is_shifted() ? 'I' : 'i';
    break;
  case j:
    return is_shifted() ? 'J' : 'j';
    break;
  case k:
    return is_shifted() ? 'K' : 'k';
    break;
  case l:
    return is_shifted() ? 'L' : 'l';
    break;
  case m:
    return is_shifted() ? 'M' : 'm';
    break;
  case n:
    return is_shifted() ? 'N' : 'n';
    break;
  case o:
    return is_shifted() ? 'O' : 'o';
    break;
  case p:
    return is_shifted() ? 'P' : 'p';
    break;
  case q:
    return is_shifted() ? 'Q' : 'q';
    break;
  case r:
    return is_shifted() ? 'R' : 'r';
    break;
  case s:
    return is_shifted() ? 'S' : 's';
    break;
  case t:
    return is_shifted() ? 'T' : 't';
    break;
  case u:
    return is_shifted() ? 'U' : 'u';
    break;
  case v:
    return is_shifted() ? 'V' : 'v';
    break;
  case w:
    return is_shifted() ? 'W' : 'w';
    break;
  case x:
    return is_shifted() ? 'X' : 'x';
    break;
  case y:
    return is_shifted() ? 'Y' : 'y';
    break;
  case z:
    return is_shifted() ? 'Z' : 'z';
    break;
  case space:
    return ' ';
    break;
  case comma_larr:
    return is_shifted() ? '<' : ',';
    break;
  case period_rarr:
    return is_shifted() ? '>' : '.';
    break;
  case enter:
    return '\n';
    break;
  case backspace:
    return '\b';
    break;
  case backtick_tilde:
    return is_shifted() ? '~' : '`';
    break;
  case one_bang:
    return is_shifted() ? '!' : '1';
    break;
  case two_at:
    return is_shifted() ? '@' : '2';
    break;
  case three_crunch:
    return is_shifted() ? '#' : '3';
    break;
  case four_dollar:
    return is_shifted() ? '$' : '4';
    break;
  case five_percent:
    return is_shifted() ? '%' : '5';
    break;
  case six_carat:
    return is_shifted() ? '^' : '6';
    break;
  case seven_amp:
    return is_shifted() ? '&' : '7';
    break;
  case eight_star:
    return is_shifted() ? '*' : '8';
    break;
  case nine_lparen:
    return is_shifted() ? '(' : '9';
    break;
  case zero_rparen:
    return is_shifted() ? ')' : '0';
    break;
  case minus_underscore:
    return is_shifted() ? '_' : '-';
    break;
  case equal_plus:
    return is_shifted() ? '+' : '=';
    break;
  case tab:
    return '\t';
    break;
  case lbracket_lbrace:
    return is_shifted() ? '{' : '[';
    break;
  case rbracket_rbrace:
    return is_shifted() ? '}' : ']';
    break;
  case semicolon_colon:
    return is_shifted() ? ':' : ';';
    break;
  case quote_dquote:
    return is_shifted() ? '"' : '\'';
    break;
  case backslash_pipe:
    return is_shifted() ? '|' : '\\';
    break;
  case slash_question:
    return is_shifted() ? '?' : '/';
    break;
  case caps_lock:
  case r_shift:
  case l_shift:
    break;
    // default:
    //     printf("%d", key_code);
    //     break;
  }
  return '\0';
}


struct ps2_waiter_t {
    uint8_t alloc;
    uint32_t pid;
};

struct ps2_waiter_t ps2_waiters[MAX_PROCESSES];

void init_ps2() {
    for(uint32_t i = 0; i < MAX_PROCESSES; i++) {
        ps2_waiters[i].alloc = 0;
    }
}

void handle_keypress(uint8_t char_code) {
  struct key_event_t event;
  if ((char_code & 0b11000000) == 0b10000000) {
    event.is_down = 0;
    event.key = char_code & 0b00111111;
  } else {
    event.is_down = 1;
    event.key = char_code;
  }

  switch (event.key) {
  case l_shift:
    kstate.l_shift = event.is_down;
    break;

  case r_shift:
    kstate.r_shift = event.is_down;
    break;

  case caps_lock:
    if (event.is_down) {
      kstate.caps = !kstate.caps;
    }
    break;

  default: {
            if (event.is_down) {
                char c = keycode_to_char(event.key);
                if (c != '\0') {
              for(uint32_t i = 0; i < MAX_PROCESSES; i++) {
                  if(ps2_waiters[i].alloc) {
                      process_table[ps2_waiters[i].pid].registers.eax = c;
                      ps2_waiters[i].alloc = 0;
                      process_table[ps2_waiters[i].pid].state = TASKSTATE_ready;
                  }
              }
                }
            }
            break;
           }
  }
}
