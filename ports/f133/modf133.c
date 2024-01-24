// This file is Copyright (c) 2017-2021 MicroPython Developers
// This file is Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause

#include "py/obj.h"

extern const mp_obj_type_t f133_led_type;
extern const mp_obj_type_t f133_dmawriter_type;
extern const mp_obj_type_t f133_dmareader_type;
extern const mp_obj_type_t f133_video_type;
extern const mp_obj_type_t f133_usbhmi_type;

STATIC const mp_rom_map_elem_t f133_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_f133) },
};

STATIC MP_DEFINE_CONST_DICT(f133_module_globals, f133_module_globals_table);

const mp_obj_module_t mp_module_f133 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&f133_module_globals,
};
