// This file is Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
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

#include <stdio.h> //for printf// This file is Copyright (c) 2017-2021 Fupy/LiteX-MicroPython Developers
// This file is Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objexcept.h"


const mp_obj_type_t litex_dpg_type;

typedef struct _litex_dpg_obj_t {
    mp_obj_base_t base;
    int num;
} litex_dpg_obj_t;


STATIC mp_obj_t litex_dpg_make_new(const mp_obj_type_t *type_in,
		size_t n_args, size_t n_kw, const mp_obj_t *args) {

    litex_dpg_obj_t *self = m_new_obj(litex_dpg_obj_t);
    self->base.type = &litex_dpg_type;
    self->num = 1;

		nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
			"not a valid dpg"));
}

void litex_dpg_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	litex_dpg_obj_t *self = self_in;
	mp_printf(print, "dpg (%u)", self->num);
}

STATIC mp_obj_t litex_dpg_demo(mp_obj_t self_in) {
	litex_dpg_obj_t *dpg = self_in;

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(litex_dpg_demo_obj, litex_dpg_demo);


STATIC const mp_map_elem_t litex_dpg_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR_demo),     (mp_obj_t)&litex_dpg_demo_obj   },
};
STATIC MP_DEFINE_CONST_DICT(litex_dpg_locals_dict, litex_dpg_locals_dict_table);

const mp_obj_type_t litex_dpg_type = {
	{ &mp_type_type },
	.name = MP_QSTR_dpg,
	.print = litex_dpg_print,
	.make_new = litex_dpg_make_new,
	.locals_dict = (mp_obj_t)&litex_dpg_locals_dict,
};

