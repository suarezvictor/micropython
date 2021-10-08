// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// License: BSD-2-Clause

#include <uart.h>
#include "lib/utils/interrupt_char.h"

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void) {
    char c;
    c = uart_read();
    /* \n to \r conversion for litex_term */
    if (c == '\n')
        return '\r';
    else
        return c;
}

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
        uart_write(*str++);
    }
}
