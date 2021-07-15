
#include "py/runtime.h"
#include "py/obj.h"
#include "extmod/machine_mem.h"
#include "extmod/machine_pinbase.h"
#include "extmod/machine_pulse.h"

#include <generated/csr.h>
#include <generated/soc.h>

#if MICROPY_PY_MACHINE

size_t strlen(const uint8_t *s) {
    const uint8_t *ss = s;
    while (*ss) {
        ++ss;
    }
    return ss - s;
}

STATIC mp_obj_t machine_identifier(void) {
    int i;
    uint8_t id[256];
    for(i=0;i<256;i++)
        id[i] = MMPTR(CSR_IDENTIFIER_MEM_BASE + 4*i);
    return mp_obj_new_bytes(id, strlen(id));
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_identifier_obj, machine_identifier);

STATIC mp_obj_t machine_reset(void) {
    ctrl_reset_soc_rst_write(1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    return MP_OBJ_NEW_SMALL_INT(CONFIG_CLOCK_FREQUENCY);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);
STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),   MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_identifier), MP_ROM_PTR(&machine_identifier_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),      MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_freq),       MP_ROM_PTR(&machine_freq_obj) },

    { MP_ROM_QSTR(MP_QSTR_mem8),  MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

#endif // MICROPY_PY_MACHINE
