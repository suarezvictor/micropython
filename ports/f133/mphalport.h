// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include <stdbool.h>
#include "sys.h" //CPU functions
#include "driver_uart.h"

static inline void mp_hal_set_interrupt_char(char c) {}

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    while (len--)
       driver_uart_putc(*str++);
}

extern void mp_handle_pending(bool);
#define MICROPY_EVENT_POLL_HOOK mp_handle_pending(true)

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void)
{
    while(!driver_uart_getc_wontblock())
    	MICROPY_EVENT_POLL_HOOK;

    char c = driver_uart_getc();
    return c == '\n' ? '\r' : c;
}


#endif // INCLUDED_MPHALPORT_H
