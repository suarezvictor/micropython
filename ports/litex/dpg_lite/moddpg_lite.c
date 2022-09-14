// Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include "py/obj.h"
#include "py/objstr.h"

#define USE_CIMGUI

#define EXPERIMENTAL_CYTHON
//TODO Cython:https://github.com/cython/cython/blob/master/Cython/Compiler/Code.py#L1787

#include <stdio.h> //printf
#include "usbhost/usb_host.h" //USB_HID_PROTO_MOUSE, USB_HID_PROTO_KEYBOARD

//TODO: add API header
void dpg_create_context(void);
void dpg_new_frame(float dt);
void dpg_render(void);
void dpg_end_frame(void);
int dpg_hidevent_mouse(int dx, int dy, int buttons, int wheel);
int dpg_hidevent_keyboard(uint8_t modifiers, uint8_t key, int pressed, char inputchar);

#ifdef USE_CIMGUI
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#endif
#ifndef EXPERIMENTAL_CYTHON
#include <string.h>
#endif

STATIC mp_obj_t create_context() {
	dpg_create_context();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(create_context_obj, create_context);

STATIC mp_obj_t new_frame() {
#warning fix the timing issue
#if 0//def USE_CIMGUI
    printf("calling igNewFrame\n");
	igNewFrame();
#else
	dpg_new_frame(1./60);
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(new_frame_obj, new_frame);

STATIC mp_obj_t render() {
	dpg_render();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(render_obj, render);

STATIC mp_obj_t end_frame() {
	dpg_end_frame();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(end_frame_obj, end_frame);


#ifdef USE_CIMGUI

#ifdef EXPERIMENTAL_CYTHON
STATIC mp_obj_t begin(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
  mp_obj_t __pyx_pw_4core_33begin(mp_obj_t __pyx_self, mp_obj_t __pyx_args, mp_obj_t __pyx_kwds);
  
  return __pyx_pw_4core_33begin(NULL, mp_obj_new_tuple(n_args, pos_args), kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(begin_obj, 0, begin);

#else
STATIC mp_obj_t begin(mp_obj_t label, mp_obj_t closable) {

#ifdef EXPERIMENTAL_CYTHON
  mp_obj_t __pyx_pf_4core_32begin(mp_obj_t __pyx_self, mp_obj_t __pyx_v_label, mp_obj_t __pyx_v_closable, ImGuiWindowFlags __pyx_v_flags);
  return __pyx_pf_4core_32begin(NULL, label, closable, 0);
#else
    GET_STR_DATA_LEN(label, s, len);
    bool opened = true;
    bool r = igBegin((const char*)s, mp_obj_is_true(closable) ? &opened : NULL, 0);
  return mp_obj_new_tuple(2, (mp_obj_t []) {
        mp_obj_new_bool(r),
        mp_obj_new_bool(opened),
     });
#endif
}
MP_DEFINE_CONST_FUN_OBJ_2(begin_obj, begin);
#endif

STATIC mp_obj_t end() {
#ifdef EXPERIMENTAL_CYTHON
  mp_obj_t __pyx_pf_4core_36end(mp_obj_t __pyx_self);
  return __pyx_pf_4core_36end(NULL);
#else
	igEnd();
    return mp_const_none;
#endif
}
MP_DEFINE_CONST_FUN_OBJ_0(end_obj, end);

STATIC mp_obj_t text(mp_obj_t arg) {
#ifdef EXPERIMENTAL_CYTHON
  mp_obj_t __pyx_pf_4core_160text(mp_obj_t __pyx_self, mp_obj_t __pyx_v_text);
  return __pyx_pf_4core_160text(NULL, arg);
#else
    GET_STR_DATA_LEN(arg, s, len);
    igTextUnformatted((const char*)s, (const char*)s+len);
    return mp_const_none;
#endif
}
MP_DEFINE_CONST_FUN_OBJ_1(text_obj, text);

STATIC mp_obj_t input_text(mp_obj_t label, mp_obj_t value, mp_obj_t buffer_length) {
#ifdef EXPERIMENTAL_CYTHON
  mp_obj_t __pyx_pf_4core_218input_text(mp_obj_t __pyx_self, mp_obj_t __pyx_v_label, mp_obj_t __pyx_v_value, int __pyx_v_buffer_length, int __pyx_v_flags);
  return __pyx_pf_4core_218input_text(NULL, label, value, mp_obj_get_int(buffer_length), 0);
#else
  GET_STR_DATA_LEN(label, s_label, label_len);
  GET_STR_DATA_LEN(value, s_value, value_len);
  char buf[256];
  strncpy(buf, (const char*)s_value, sizeof(buf));
  mp_int_t blen = mp_obj_get_int(buffer_length);
  if(blen > sizeof(buf))
    blen = sizeof(buf);
  bool changed = igInputText((const char*)s_label, buf, blen, 0, NULL, NULL);
  return mp_obj_new_tuple(2, (mp_obj_t []) {
        mp_obj_new_bool(changed),
        mp_obj_new_str(buf, strlen(buf)),
     });
#endif
}
MP_DEFINE_CONST_FUN_OBJ_3(input_text_obj, input_text);

#else //!USE_CIMGUI

STATIC mp_obj_t add_text(mp_obj_t arg) {
    GET_STR_DATA_LEN(arg, s, len);
    printf("add_text: %.*s\n", len, s);
    dpg_demo();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(add_text_obj, add_text);
#endif

STATIC mp_obj_t on_mouse(mp_obj_t dx, mp_obj_t dy, mp_obj_t buttons/*, mp_obj_t wheel*/) {
	int r = dpg_hidevent_mouse(mp_obj_get_int(dx), mp_obj_get_int(dy), mp_obj_get_int(buttons), 0);
    return MP_OBJ_NEW_SMALL_INT(r);
}
MP_DEFINE_CONST_FUN_OBJ_3(on_mouse_obj, on_mouse);

STATIC mp_obj_t on_keyboard(mp_obj_t modifiers, mp_obj_t key, mp_obj_t pressed/*, mp_obj_t inputchar*/) {
	int r = dpg_hidevent_keyboard(mp_obj_get_int(modifiers), mp_obj_get_int(key), mp_obj_get_int(pressed), 0);
    return MP_OBJ_NEW_SMALL_INT(r);
}
MP_DEFINE_CONST_FUN_OBJ_3(on_keyboard_obj, on_keyboard);

STATIC const mp_rom_map_elem_t dpg_lite_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dpg_lite) },
    { MP_ROM_QSTR(MP_QSTR_create_context), MP_ROM_PTR(&create_context_obj) },
    { MP_ROM_QSTR(MP_QSTR_new_frame), MP_ROM_PTR(&new_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&render_obj) },
    { MP_ROM_QSTR(MP_QSTR_end_frame), MP_ROM_PTR(&end_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_mouse), MP_ROM_PTR(&on_mouse_obj) },
    { MP_ROM_QSTR(MP_QSTR_on_keyboard), MP_ROM_PTR(&on_keyboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_HID_PROTO_NONE), MP_ROM_INT(USB_HID_PROTO_NONE) },
    { MP_ROM_QSTR(MP_QSTR_HID_PROTO_MOUSE), MP_ROM_INT(USB_HID_PROTO_MOUSE) },
    { MP_ROM_QSTR(MP_QSTR_HID_PROTO_KEYBOARD), MP_ROM_INT(USB_HID_PROTO_KEYBOARD) },
#ifdef USE_CIMGUI
    { MP_ROM_QSTR(MP_QSTR_begin), MP_ROM_PTR(&begin_obj) },
    { MP_ROM_QSTR(MP_QSTR_end), MP_ROM_PTR(&end_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&text_obj) },
    { MP_ROM_QSTR(MP_QSTR_input_text), MP_ROM_PTR(&input_text_obj) },
#else
    { MP_ROM_QSTR(MP_QSTR_add_text), MP_ROM_PTR(&add_text_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(dpg_lite_module_globals, dpg_lite_module_globals_table);

const mp_obj_module_t mp_module_dpg_lite = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&dpg_lite_module_globals,
};
