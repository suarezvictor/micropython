// (C) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#include <stdio.h>
#include <stdint.h>

#include "py/mphal.h"
#include "py/compile.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"

__attribute__((constructor)) void foo(void)
{
  mp_hal_stdout_tx_str("Starting F133 platform\r\n");
}


int main(int argc, char *argv[])
{
  //UART is initialized in the startup assembly

  void _init(void);
  _init(); //call init functions, include above one

  void start_micropython(int argc, char **argv);
  start_micropython(argc, argv);

  return 0;
}

