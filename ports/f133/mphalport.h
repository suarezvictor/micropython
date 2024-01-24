// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include "sys.h" //CPU functions
#warning IMPLEMENT UART FUNCTIONS

static int uart_read_nonblock() { return 0; }
static int uart_read() { return 0; }
void driver_uart_putc(char c);

#include "lib/utils/interrupt_char.h"
#include <stdbool.h>

extern void mp_handle_pending(bool);
#define MICROPY_EVENT_POLL_HOOK mp_handle_pending(true)

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void) {
    char c;
    while(!uart_read_nonblock())
    	MICROPY_EVENT_POLL_HOOK;
    		
    c = uart_read();
    // \n to \r conversion
    if (c == '\n')
        return '\r';
    else
        return c;
}

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
       driver_uart_putc(*str++);
    }
}


// C-level pin HAL
#include "py/obj.h"

#if 0
//no GPIO enabled in SoC definition
#define mp_hal_pin_input(p) (void)(p)
#define mp_hal_pin_output(p) (void)(p)
#define mp_hal_pin_open_drain(p) (void)(p)
#define mp_hal_pin_od_low(p) (void)(p)
#define mp_hal_pin_od_high(p) (void)(p)
#define mp_hal_get_pin_obj(p) (-1) //no pins possible
#define mp_hal_pin_read(p) 0
#define mp_hal_pin_write(p, v) (void)(p)

#endif

#endif // INCLUDED_MPHALPORT_H
