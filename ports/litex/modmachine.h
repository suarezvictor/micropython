// This file is Copyright (c) 2021 Fupy/LiteX-MicroPython Developers
// License: BSD-2-Clause

#ifndef MICROPY_INCLUDED_LITEX_MODMACHINE_H
#define MICROPY_INCLUDED_LITEX_MODMACHINE_H

extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_pwm_type;

void machine_timer_deinit_all(void);

#endif
