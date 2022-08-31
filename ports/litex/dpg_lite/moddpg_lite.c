// Copyright (C) 2021-2022 Victor Suarez Rovere <suarezvictor@gmail.com>
// License: BSD-2-Clause

#include "py/obj.h"
#include "py/objstr.h"

#define USE_CIMGUI

#include <stdio.h> //printf

//TODO: add API header
void dpg_create_context(void);
void dpg_new_frame(float dt);
void dpg_render(void);
void dpg_end_frame(void);

#ifdef USE_CIMGUI
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#endif

#ifdef USE_CIMGUI
STATIC mp_obj_t text(mp_obj_t arg) {

    GET_STR_DATA_LEN(arg, s, len);
    printf("text: %.*s\n", len, s);
    igBegin("Color", NULL, 0);
    static int color_r = 128;
    igSliderInt("R", &color_r, 0, 255, "%d", 0);
    color_r = (--color_r)  & 0xFF;
    igEnd();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(text_obj, text);
#else
STATIC mp_obj_t add_text(mp_obj_t arg) {

    GET_STR_DATA_LEN(arg, s, len);
    printf("add_text: %.*s\n", len, s);
    dpg_demo();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(add_text_obj, add_text);
#endif

STATIC mp_obj_t create_context() {
#if 0//def USE_CIMGUI
    printf("calling igCreateContext\n");
	igCreateContext(NULL);
#else
	dpg_create_context();
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(create_context_obj, create_context);

STATIC mp_obj_t new_frame() {
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
#if 0 //def USE_CIMGUI
    printf("calling igRender\n");
	igRender();
#else
	dpg_render();
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(render_obj, render);

STATIC mp_obj_t end_frame() {
#if 0//def USE_CIMGUI
    printf("calling igNewFrame\n");
	igEndFrame();
#else
	dpg_end_frame();
#endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(end_frame_obj, end_frame);


STATIC const mp_rom_map_elem_t dpg_lite_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dpg_lite) },
    { MP_ROM_QSTR(MP_QSTR_create_context), MP_ROM_PTR(&create_context_obj) },
    { MP_ROM_QSTR(MP_QSTR_new_frame), MP_ROM_PTR(&new_frame_obj) },
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&render_obj) },
    { MP_ROM_QSTR(MP_QSTR_end_frame), MP_ROM_PTR(&end_frame_obj) },
#ifndef USE_CIMGUI
    { MP_ROM_QSTR(MP_QSTR_add_text), MP_ROM_PTR(&add_text_obj) },
#else
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&text_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(dpg_lite_module_globals, dpg_lite_module_globals_table);

const mp_obj_module_t mp_module_dpg_lite = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&dpg_lite_module_globals,
};
