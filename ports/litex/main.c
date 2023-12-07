// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "irq.h"
#include "uart.h"

extern uint8_t _fast_text[1], _efast_text[1], _fast_text_loadaddr[1];
extern uint8_t _fast_data[1], _efast_data[1], _fast_data_loadaddr[1];

void crt0_post(void)
{
    //copies fast code and data sections from load address on DRAM to SRAM
    memcpy(&_fast_text, &_fast_text_loadaddr, &_efast_text-&_fast_text);
    memcpy(&_fast_data, &_fast_data_loadaddr, &_efast_data-&_fast_data);
    //printf("_fast_text 0x%p, _efast_text 0x%p, _fast_text_loadaddr 0x%p, size=%08X\n", &_fast_text, &_efast_text, &_fast_text_loadaddr, &_efast_text-&_fast_text);
    //printf("_fast_data 0x%p, _efast_data 0x%p, _fast_data_loadaddr 0x%p, size=%08X\n", &_fast_data, &_efast_data, &_fast_data_loadaddr, &_efast_data-&_fast_data);
}


void /*NORETURN*/ hard_reset() { ctrl_reset_write(1); for(;;); } //TODO: move to SDK
void start_micropython(int argc, char **argv);

int main(int argc, char **argv) {
    crt0_post();
    
    irq_setmask(0);
    irq_setie(1);
    uart_init();
#warning argc, char **argv is garbage
    start_micropython(argc, argv);

    irq_setie(0);
    irq_setmask(~0);

    hard_reset();
}


size_t strlen(const char *s) {
    const char *ss = s;
    while (*ss) {
        ++ss;
    }
    return ss - s;
}

#ifndef CSR_TIMER0_UPTIME_CYCLES_ADDR
#warning DELAY FUNCTION IS NOT ACCURATE, use uptime method (--timer-uptime option at SoC generation)
#endif

