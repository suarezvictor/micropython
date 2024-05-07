// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (c) 2021, 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#ifndef INCLUDED_MPHALPORT_H
#define INCLUDED_MPHALPORT_H

#include <stdbool.h>
#include "sys.h" //CPU functions
#include "driver_uart.h"
#include "ioregs.h" //Registers FIXME: use GPIO hal functions
#include "py/obj.h"

static inline void mp_hal_set_interrupt_char(char c) {}

// Send the string of given length.
static inline void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    while (len--)
       driver_uart_putc(UART_COMM, *str++);
}

extern void mp_handle_pending(bool);
#define MICROPY_EVENT_POLL_HOOK mp_handle_pending(true)

// Receive single character, blocking until one is available.
static inline int mp_hal_stdin_rx_chr(void)
{
    while(!driver_uart_getc_wontblock(UART_COMM))
    	MICROPY_EVENT_POLL_HOOK;

    char c = driver_uart_getc(UART_COMM);
    return c == '\n' ? '\r' : c;
}

#define mp_hal_delay_us_fast(u) delay_us(u)

//GPIO----------------------------------------------------------------------------------------------
#warning move this GPIO code

/*
F133 docs:
9.7.5.11 0x0090 PD Configure Register 0 (Default Value: 0xFFFF_FFFF)
0x0094 PD1 (GPIO 8-15)
0x0098 PD2 (GPIO 16-24)
0000:Input 0001:Output 0010:LCD0-Dxxx 1111:IO Disable

9.7.5.18 0x00B4 PD Pull Register 0 (Default Value: 0x0000_0000)
00: Pull_up/down disable 01: Pull_up 10: Pull_down 11: Reserved
*/

#define MP_HAL_PIN_FMT "%u"
#define mp_hal_pin_name(p)      (p)
#define mp_hal_pin_obj_t int

//#define mp_hal_pin_input(p) printf("pin input " MP_HAL_PIN_FMT "\n", p)
//#define mp_hal_pin_output(p) printf("pin ouput" MP_HAL_PIN_FMT "\n", p)
//#define mp_hal_pin_write(p, v) printf("pin write " MP_HAL_PIN_FMT " = %d\n", p, v)

#define mp_hal_pin_read(p) ((io_read32(GPIO_pe_dat) >> p) & 1)
#define mp_hal_pin_od_low(p) mp_hal_pin_od_write(p, 0)
#define mp_hal_pin_od_high(p) mp_hal_pin_od_write(p, 1)

static void mp_hal_pin_od_write(mp_hal_pin_obj_t p, int v)
{
  //FIXME: use mp_hal_pin_input / mp_hal_pin_output
  switch(p) //only support some pins for I2C
  {
    case 12: //pins 8-15 uses GPIO_pe_cfg1
      io_write32(GPIO_pe_cfg1, (io_read32(GPIO_pe_cfg1) & ~GPIO_pe_cfg1_pe12_select)
        | ((v ? 0b0000 : 0b0001) << GPIO_pe_cfg1_pe12_select_SHIFT)); //0000:Input=High 0001:Output=Low
      break;
      
    case 13: //pins 8-15 uses GPIO_pe_cfg1
      io_write32(GPIO_pe_cfg1, (io_read32(GPIO_pe_cfg1) & ~GPIO_pe_cfg1_pe13_select)
        | ((v ? 0b0000 : 0b0001) << GPIO_pe_cfg1_pe13_select_SHIFT)); //0000:Input=High 0001:Output=Low
      break;
  }

  io_write32(GPIO_pe_dat, io_read32(GPIO_pe_dat) & ~(1<<p)); //reset bit (always, just in case it will be set as output)
}

static void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin)
{
  if(pin != 12 && pin != 13)
    mp_raise_ValueError(MP_ERROR_TEXT("Not all pins are supported for I2C"));

  //io_write32(GPIO_pe_pull0, (io_read32(GPIO_pe_pull0) & ~(0b11<<(2*p))) | (0b01 << (2*p))); //0b01: pull-up TODO: test if this works
  mp_hal_pin_od_high(pin);
}

static mp_hal_pin_obj_t mp_hal_get_pin_obj(const mp_obj_t pin_in)
{
    if (mp_obj_is_small_int(pin_in)) {
        mp_hal_pin_obj_t value = MP_OBJ_SMALL_INT_VALUE(pin_in);
        return value;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("pin should be an integer"));
}


#endif // INCLUDED_MPHALPORT_H
