#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include <stddef.h>
#include <stdint.h>

uint16_t *v_mem = 0;
uint16_t term_row = 0;
uint16_t term_col = 0;

uint16_t term_make_char(char c, char color) { return (color << 8) | c; }

void term_putchar(int x, int y, char c, char color) {
  v_mem[(y * VGA_WIDTH) + x] = term_make_char(c, color);
}

void term_writechar(char c, char color) {
  if (c == '\n') {
    term_row += 1;
    term_col = 0;
    return;
  }

  term_putchar(term_col, term_row, c, color);
  term_col += 1;
  if (term_col >= VGA_WIDTH) {
    term_col = 0;
    term_row += 1;
  }
}

void term_init() {
  v_mem = (uint16_t *)(0xB8000);
  term_col = 0;
  term_row = 0;
  for (int i = 0; i < VGA_HEIGHT; i++) {
    for (int j = 0; j < VGA_WIDTH; j++) {
      term_putchar(i, j, ' ', 0);
    }
  }
}

size_t strlen(const char *str) {
  size_t l = 0;
  while (str[l]) {
    l++;
  }
  return l;
}

void print(const char *str) {
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    term_writechar(str[i], 15);
  }
}
void kernel_main() {
  term_init();
  print("hellow eorlf fdsfds fds \n fds fds fdsf ds f dsf dsf  dsf");
  idt_init();

  outb(0x60, 0xff);
}
