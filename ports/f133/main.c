// (C) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: AGPL-3.0-only

#include <stdio.h>
#include <stdint.h>
#include "sys.h"
#include "py/mphal.h"

int main(void)
{
  void uart_probe(void);
  uart_probe();

  for(int i=0; ; ++i)
  {
    char msg[]="Hello from F133 micropython\r\n";
    mp_hal_stdout_tx_strn(msg, sizeof(msg)-1);
    delay_us(500*1000);
  }

  return 0;
}

