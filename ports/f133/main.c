// (C) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#include <stdio.h>
#include <stdint.h>

#include "py/mphal.h"
#include "py/compile.h"
#include "py/gc.h"
#include "lib/utils/pyexec.h"


//TODO: use ((constuctor)) attribute to manage things in C and not in the startup assembly
// (i.e. clear bss section)

typedef void (*func_ptr) (void);
extern func_ptr _init_array_start[], _init_array_end[];
extern func_ptr _fini_array_start[], _fini_array_end[];

void _init(void)
{
  for ( func_ptr* func = _init_array_start; func != _init_array_end; ++func )
  {
    mp_hal_stdout_tx_str("Call constructor\r\n");
    (*func)();
  }
}
 
void _fini(void)
{
  for ( func_ptr* func = _fini_array_start; func != _fini_array_end; ++func )
    (*func)();
}

__attribute__((constructor)) void foo(void)
{
  mp_hal_stdout_tx_str("-> Example init function\r\n");
}



int main(int argc, char *argv[])
{
  _init(); //call constuctors
 
 
  //UART is initialized in the startup assembly
  
  for(int i=0; i < 5; ++i)
  {
    mp_hal_stdout_tx_str("Hello from F133 micropython\r\n");
    delay_us(500*1000);
  }

#if 1
  mp_hal_stdout_tx_str("About to start micropython REPL...\r\n");
  void start_micropython(int argc, char **argv);
  start_micropython(argc, argv);
#else
  static char heap[4096];
  gc_init(heap, heap + sizeof(heap));

  mp_init();
  mp_hal_stdout_tx_str("About to start micropython REPL...\r\n");

  // Start a normal REPL; will exit when ctrl-D is entered on a blank line.
  pyexec_friendly_repl();
#endif

  // Deinitialise the runtime.
  gc_sweep_all();
  mp_deinit();
    
  return 0;
}

