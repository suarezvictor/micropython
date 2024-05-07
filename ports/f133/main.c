// (C) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "py/mphal.h"
#include "py/compile.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"

__attribute__((constructor)) void foo(void)
{
  //printf("Starting F133 platform\n");
}


int main(int argc, char *argv[])
{
  uart_probe(UART_COMM); //DEBUG UART is already initialized in the startup assembly

  void _init(void);
  _init(); //call init functions, include above one

  void start_micropython(int argc, char **argv);
  start_micropython(argc, argv);

  return 0;
}

#if MICROPY_USE_INTERNAL_PRINTF
//It's missing in libs/utils/printf.c, a hack to be able to replace it with a float capable one
int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int ret = mp_vprintf(&mp_plat_print, fmt, ap);
    va_end(ap);
    return ret;
}
#endif
