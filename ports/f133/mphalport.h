// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include <stdbool.h>
#include "sys.h" //CPU functions
#include "ioregs.h"

//FIXME: move to driver_uart.c
static inline int driver_uart_wontblock(void)
{
  const intptr_t UART_BASE = 0x02500000;
  const int UART_LSR = 0x0014;
  const int UART_LSR_DR = 0x01; // Receiver data ready
  return (io_read32(UART_BASE + UART_LSR) & UART_LSR_DR) != 0;
}

void driver_uart_putc(char c);
static inline int driver_uart_getc(void)
{
  if(!driver_uart_wontblock())
    return -1;

  const intptr_t UART_BASE = 0x02500000;
  const int UART_RBR = 0x0000;
  return io_read32(UART_BASE + UART_RBR);
}


static inline void mp_hal_set_interrupt_char(char c) {}

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
       driver_uart_putc(*str++);
    }
}

extern void mp_handle_pending(bool);
#define MICROPY_EVENT_POLL_HOOK mp_handle_pending(true)

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void) {
    char c;

    while(!driver_uart_wontblock())
    	MICROPY_EVENT_POLL_HOOK;

    c = driver_uart_getc();
    // \n to \r conversion
    if (c == '\n')
        return '\r';
    else
        return c;
}


#endif // INCLUDED_MPHALPORT_H
