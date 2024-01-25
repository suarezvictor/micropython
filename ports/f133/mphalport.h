// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "sys.h" //CPU functions
#warning IMPLEMENT UART FUNCTIONS

static int uart_read_nonblock(void)
{
  static int count = 0;
  return (++count) & 1;
}

static int uart_read(void)
{
  static int count = 0;
  if(count++ < 5)
    return '1';
  count = 0;
  return '\n';
}
void driver_uart_putc(char c);

#include "lib/utils/interrupt_char.h"
#include <stdbool.h>

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
       driver_uart_putc(*str++);
    }
}

#if 1
#warning why is this needed?
static inline size_t strlen_x(const char *s)
{
  const char *tmp = s;
  while (*tmp) ++tmp;
  return tmp - s;
}
#define strlen strlen_x
#endif

extern void mp_handle_pending(bool);
#define MICROPY_EVENT_POLL_HOOK mp_handle_pending(true)

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void) {
    char c;

    //while(!uart_read_nonblock()) //passes here
    	MICROPY_EVENT_POLL_HOOK;

    c = uart_read();
    // \n to \r conversion
    if (c == '\n')
        return '\r';
    else
        return c;
}


#endif // INCLUDED_MPHALPORT_H
