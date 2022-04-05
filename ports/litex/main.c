// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "uart.h"

void /*NORETURN*/ hard_reset() { ctrl_reset_write(1); for(;;); } //TODO: move to SDK
int upython_main(int argc, char **argv, char *stack_top_arg);
void start_micropython(int argc, char **argv)
{
    //safe way to determine stack top: no other variables in this function //TODO: use alloca()
    int stack_dummy;
    while(upython_main(argc, argv, (char*)&stack_dummy) == 0)
        /*soft_reset()*/;
}

int main(int argc, char **argv) {
    irq_setmask(0);
    irq_setie(1);
    uart_init();

    start_micropython(argc, argv);

    irq_setie(0);
    irq_setmask(~0);

    hard_reset();
}

#ifndef CSR_TIMER0_UPTIME_CYCLES_ADDR
#warning DELAY FUNCTION IS NOT ACCURATE, use uptime method (--timer-uptime option at SoC generation)
#endif

