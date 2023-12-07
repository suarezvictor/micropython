// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// This file is Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
// This file is Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD

#include <generated/csr.h>
#include <generated/soc.h>
#include <irq.h>
#include <uart.h>
#include <litex_sdk/litex.h>

void __attribute__((weak)) timer0_isr(void);
void __attribute__((weak)) i2s_tx_isr(void);
void __attribute__((weak)) i2s_rx_isr(void);

#ifdef CONFIG_CPU_HAS_INTERRUPT
void FAST_CODE isr(void)
{
	__attribute__((unused)) unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

#if defined(UART_INTERRUPT) && !defined(UART_POLLING)
	if(irqs & (1 << UART_INTERRUPT))
	{
		uart_isr();
	}
#endif
#if defined(TIMER0_INTERRUPT) && !defined(TIMER0_POLLING)
	if(irqs & (1 << TIMER0_INTERRUPT))
	{
		if(timer0_isr)
			timer0_isr();
	}
#endif
#if defined(I2S_TX_INTERRUPT) && !defined(I2S_TX_POLLING)
	if(irqs & (1 << I2S_TX_INTERRUPT))
	{
		if(i2s_tx_isr)
			i2s_tx_isr();
	}
#endif
#if defined(I2S_RX_INTERRUPT) && !defined(I2S_RX_POLLING)
	if(irqs & (1 << I2S_RX_INTERRUPT))
	{
		if(i2s_rx_isr)
			i2s_rx_isr();
	}
#endif
}

#else

#error CONFIG_CPU_HAS_INTERRUPT should be enabled
#endif
