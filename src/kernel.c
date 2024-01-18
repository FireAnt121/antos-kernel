#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
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

static struct paging_4gb_chunk *kernel_chunk = 0;
void kernel_main() {
  term_init();
  print("hellow eorlf fdsfds fds \n fds fds fdsf ds f dsf dsf  dsf");
  // Initialze hte heap
  kheap_init();
  // Initialze hte interrupt descriptor table
  idt_init();
  // setup paging
  kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT |
                                PAGING_ACCESS_FROM_ALL);

  //  switch to paging kernel chunk
  paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
  char *ptr = kzalloc(4096);
  paging_set(paging_4gb_chunk_get_directory(kernel_chunk), (void *)0x1000,
             (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT |
                 PAGING_IS_WRITABLE);
  enable_paging();
  char *ptr2 = (char *)0x1000;
  ptr2[0] = 'A';
  ptr2[1] = 'B';
  print(ptr2);
  enable_interrupts();
}
