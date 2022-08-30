// Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause


/*
https://dearpygui.readthedocs.io/en/latest/tutorials/first-steps.html

import dearpygui.dearpygui as dpg

dpg.create_context()
dpg.create_viewport(title='Custom Title', width=600, height=300)

with dpg.window(label="Example Window"):
    dpg.add_text("Hello, world")
    dpg.add_button(label="Save")
    dpg.add_input_text(label="string", default_value="Quick brown fox")
    dpg.add_slider_float(label="float", default_value=0.273, max_value=1)

dpg.setup_dearpygui()
dpg.show_viewport()
dpg.start_dearpygui()
dpg.destroy_context()

WITH example (with statement calls __enter__ and __exit__):
class Demo:
    def __enter__(self):
        print(" calling to __enter__ method")
        return "True"

    def __exit__(self, exc_type, exc_val, exc_tb):
        print(" calling to __exit__ method")
*/

#include "py/obj.h"
#include "py/objstr.h"

STATIC mp_obj_t add_text(mp_obj_t arg) {

    GET_STR_DATA_LEN(arg, s, len);
    printf("add_text: %.*s\n", len, s);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(add_text_obj, add_text);


STATIC const mp_rom_map_elem_t dpg_lite_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dpg_lite) },
    { MP_ROM_QSTR(MP_QSTR_add_text), MP_ROM_PTR(&add_text_obj) },
};

STATIC MP_DEFINE_CONST_DICT(dpg_lite_module_globals, dpg_lite_module_globals_table);

const mp_obj_module_t mp_module_dpg_lite = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&dpg_lite_module_globals,
};
