// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include "py/obj.h"

extern const mp_obj_type_t litex_pin_type;
extern const mp_obj_type_t litex_led_type;
extern const mp_obj_type_t litex_dmawriter_type;
extern const mp_obj_type_t litex_dmareader_type;

STATIC const mp_rom_map_elem_t litex_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_litex) },
#warning: litex.PIN should be deprecated since replaced by the standard one umachine.Pin
    { MP_ROM_QSTR(MP_QSTR_PIN),    MP_ROM_PTR(&litex_pin_type) }, //TODO: replace by machine.Pin
    { MP_ROM_QSTR(MP_QSTR_LED),    MP_ROM_PTR(&litex_led_type) },
#ifdef CSR_DMA_WRITER_BASE
    { MP_ROM_QSTR(MP_QSTR_DMAWriter), MP_ROM_PTR(&litex_dmawriter_type) },
#endif
#ifdef CSR_DMA_READER_BASE
    { MP_ROM_QSTR(MP_QSTR_DMAReader), MP_ROM_PTR(&litex_dmareader_type) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(litex_module_globals, litex_module_globals_table);

const mp_obj_module_t mp_module_litex = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&litex_module_globals,
};
