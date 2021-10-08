// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// License: BSD-2-Clause

#include <generated/csr.h>
#include <irq.h>
#include <uart.h>

void isr(void)
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & (1 << UART_INTERRUPT))
		uart_isr();
}
